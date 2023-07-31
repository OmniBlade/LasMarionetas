/**
 * @file
 *
 * @brief General purpose memory allocator.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "allocator/generalalloc.h"
#include "allocator/allocmutex.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

namespace Allocator
{

GeneralAllocator::GeneralAllocator(void *pInitialCore,
    size_t nInitialCoreSize,
    bool bShouldFreeInitialCore,
    bool bShouldTrimInitialCore,
    CoreFreeFunction pInitialCoreFreeFunction,
    void *pInitialCoreFreeFunctionContext)
{
    mbInitialized = false;
    mnMaxFastBinChunkSize = 0;
    std::memset(mpFastBinArray, 0, sizeof(mpFastBinArray));
    std::memset(mpBinArray, 0, sizeof(mpBinArray));
    std::memset(mBinBitmap, 0, sizeof(mBinBitmap));
    mpTopChunk = nullptr;
    mpLastRemainderChunk = nullptr;
    std::memset(&mHeadCoreBlock, 0, sizeof(mHeadCoreBlock));
    mpHighFence = nullptr;
    mbHighFenceInternallyDisabled = false;
    mbSystemAllocEnabled = true;
    mnCheckChunkReentrancyCount = 0;
    mcTraceFieldDelimiter = uint8_t('\t');
    mcTraceRecordDelimiter = uint8_t('\n');
    mAutoHeapValidationLevel = kHeapValidationLevelNone;
    mnAutoHeapValidationFrequency = 0;
    mnAutoHeapValidationEventCount = 0;
    mbHeapValidationActive = false;
    mnMMapCount = 0;
    mnMMapMallocTotal = 0;
    mnMMapMaxAllowed = 0;
    mnMMapThreshold = 0;
    mbMMapTopDown = false;
    std::memset(&mHeadMMapChunk, 0, sizeof(mHeadMMapChunk));
    mpHookFunction = nullptr;
    mpHookFunctionContext = nullptr;
    mpMallocFailureFunction = nullptr;
    mpMallocFailureFunctionContext = nullptr;
    mnMaxMallocFailureCount = 256;
    mpAssertionFailureFunction = nullptr;
    mpAssertionFailureFunctionContext = nullptr;
    mpTraceFunction = nullptr;
    mpTraceFunctionContext = nullptr;
    mnTrimThreshold = 0;
    mnTopPad = 0;
    mpInitialTopChunk = nullptr;
    mnPageSize = kDefaultPageSize;
    mnNewCoreSize = kDefaultNewCoreSize;
    mnCoreIncrementSize = kDefaultCoreIncrementSize;
    mbTraceInternalMemory = false;
    mpMutex = nullptr;
    std::memset(mMutexData, 0, sizeof(mMutexData));
    mnFillFree = kDefaultFillValueFree;
    mnFillDelayedFree = kDefaultFillValueDelayedFree;
    mnFillNew = kDefaultFillValueNew;
    mnFillGuard = kDefaultFillValueGuard;
    mnFillUnusedCore = kDefaultFillValueUnusedCore;
    mpAssertionFailureFunction = &AssertionFailureFunctionDefault;
    mpAssertionFailureFunctionContext = this;
    mpTraceFunction = &TraceFunctionDefault;
    mpTraceFunctionContext = this;
    GeneralAllocator::Init(pInitialCore,
        nInitialCoreSize,
        bShouldFreeInitialCore,
        bShouldTrimInitialCore,
        pInitialCoreFreeFunction,
        pInitialCoreFreeFunctionContext);
}

GeneralAllocator::~GeneralAllocator()
{
    GeneralAllocator::Shutdown();
}

bool GeneralAllocator::Init(void *pInitialCore,
    size_t nInitialCoreSize,
    bool bShouldFreeInitialCore,
    bool bShouldTrimInitialCore,
    CoreFreeFunction pInitialCoreFreeFunction,
    void *pInitialCoreFreeFunctionContext)
{
    if (!mbInitialized) {
        mbInitialized = true;
        SetOption(kOptionEnableThreadSafety, 1);
        PPMAutoMutex lock(mpMutex);
        mnMaxFastBinChunkSize = kFastBinDefaultMaxDataSize;
        memset(mpFastBinArray, 0, sizeof(mpFastBinArray));
        memset(mpBinArray, 0, sizeof(mpBinArray));

        for (int32_t i = 1; i < kBinCount; ++i) {
            Chunk *const pBin = GetBin(i);
            pBin->mpNextChunk = pBin;
            pBin->mpPrevChunk = pBin;
        }

        memset(mBinBitmap, 0, sizeof(mBinBitmap));
        mpTopChunk = GetInitialTopChunk();
        mpLastRemainderChunk = nullptr;
        memset(&mHeadCoreBlock, 0, sizeof(mHeadCoreBlock));
        mHeadCoreBlock.mpNextCoreBlock = &mHeadCoreBlock;
        mHeadCoreBlock.mpPrevCoreBlock = &mHeadCoreBlock;
        mpHighFence = nullptr;
        mbHighFenceInternallyDisabled = false;
        SetMaxFastBinDataSize(kFastBinDefaultMaxDataSize);
        mnMMapCount = 0;
        mnMMapMallocTotal = 0;
        mnMMapMaxAllowed = kDefaultMMapMaxAllowed;
        mnMMapThreshold = kDefaultMMapThreshold;
        mbMMapTopDown = 0;
        memset(&mHeadMMapChunk, 0, sizeof(mHeadMMapChunk));
        mHeadMMapChunk.mpNextChunk = &mHeadMMapChunk;
        mHeadMMapChunk.mpPrevChunk = &mHeadMMapChunk;
        mnTrimThreshold = kDefaultTrimThreshold;
        mnTopPad = kDefaultTopPad;
        mpInitialTopChunk = reinterpret_cast<char *>(mpTopChunk);
        mnPageSize = getpagesize();
    }

    if (pInitialCore != nullptr || nInitialCoreSize) {
        AddCore(pInitialCore,
            nInitialCoreSize,
            bShouldFreeInitialCore,
            bShouldTrimInitialCore,
            pInitialCoreFreeFunction,
            pInitialCoreFreeFunctionContext);
    }

    return true;
}

bool GeneralAllocator::Shutdown()
{
    PPMAutoMutex lock(mpMutex);

    if (mbInitialized) {
        mbInitialized = false;

        if (GetFastBinChunksExist()) {
            ClearFastBins();
        }

        while (mHeadMMapChunk.mpNextChunk != &mHeadMMapChunk) {
            Chunk *const pMMappedChunk = GetMMapChunkFromMMapListChunk(mHeadMMapChunk.mpNextChunk);
            FreeInternal(GetDataPtrFromChunkPtr(pMMappedChunk));
        }

        while (mHeadCoreBlock.mpNextCoreBlock != &mHeadCoreBlock) {
            CoreBlock *const pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
            UnlinkCoreBlock(pCoreBlock);
            FreeCore(pCoreBlock, true);
        }

        mpTopChunk = GetInitialTopChunk();
        mpHighFence = nullptr;
        mbHighFenceInternallyDisabled = false;
    }

    return true;
}

void *GeneralAllocator::Malloc(size_t nSize, int32_t nAllocationFlags)
{
    void *retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = MallocInternal(nSize, nAllocationFlags);
    }

    return retval;
}

void *GeneralAllocator::Calloc(size_t nElementCount, size_t nElementSize, int32_t nAllocationFlags)
{
    void *retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = CallocInternal(nElementCount, nElementSize, nAllocationFlags);
    }

    return retval;
}

void *GeneralAllocator::Realloc(void *pData, size_t nNewSize, int32_t nAllocationFlags)
{
#ifdef ALLOCATOR_USEHOOKS
    if (mpHookFunction != nullptr) {
        HookInfo hookInfo;
        hookInfo.mpGeneralAllocator = this;
        hookInfo.mbEntry = true;
        hookInfo.mHookType = kHookTypeMalloc;
        hookInfo.mHookSubType = kHookSubTypeRealloc;
        hookInfo.mnSizeInputTotal = nNewSize;
        hookInfo.mpDataInput = pData;
        hookInfo.mnCountInput = 0;
        hookInfo.mnSizeInput = nNewSize;
        hookInfo.mpSizeInputArray = nullptr;
        hookInfo.mnAlignmentInput = 0;
        hookInfo.mnAllocationFlags = nAllocationFlags;
        hookInfo.mpDataOutput = nullptr;
        hookInfo.mpArrayOutput = nullptr;
        hookInfo.mnSizeOutput = 0;
        mpHookFunction(&hookInfo, mpHookFunctionContext);
    }
#endif
    void *retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = ReallocInternal(pData, nNewSize, nAllocationFlags);
    }
#ifdef ALLOCATOR_USEHOOKS
    if (mpHookFunction != nullptr) {
        HookInfo hookInfo;
        hookInfo.mpGeneralAllocator = this;
        hookInfo.mbEntry = true;
        hookInfo.mHookType = kHookTypeMalloc;
        hookInfo.mHookSubType = kHookSubTypeRealloc;
        hookInfo.mnSizeInputTotal = nNewSize;
        hookInfo.mpDataInput = pData;
        hookInfo.mnCountInput = 0;
        hookInfo.mnSizeInput = nNewSize;
        hookInfo.mpSizeInputArray = nullptr;
        hookInfo.mnAlignmentInput = 0;
        hookInfo.mnAllocationFlags = nAllocationFlags;
        hookInfo.mpDataOutput = retval;
        hookInfo.mpArrayOutput = nullptr;
        hookInfo.mnSizeOutput = 0;
        mpHookFunction(&hookInfo, mpHookFunctionContext);
    }
#endif
    return retval;
}

void GeneralAllocator::Free(void *pData)
{
    FreeInternal(pData);
    {
        PPMAutoMutex lock(mpMutex);
        FreeInternal(pData);
    }
}

void *GeneralAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int32_t nAllocationFlags)
{
    void *retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = MallocAlignedInternal(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);
    }

    return retval;
}

void **GeneralAllocator::MallocMultiple(
    size_t nElementCount, size_t nElementSize, void *pResultArray[], int32_t nAllocationFlags)
{
    void **retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = MallocMultipleInternal(nElementCount, 1, &nElementSize, pResultArray, nAllocationFlags);
    }

    return retval;
}

void **GeneralAllocator::MallocMultiple(
    size_t nElementCount, const size_t nElementSizes[], void *pResultArray[], int32_t nAllocationFlags)
{
    void **retval;
    {
        PPMAutoMutex lock(mpMutex);
        retval = MallocMultipleInternal(nElementCount, nElementCount, nElementSizes, pResultArray, nAllocationFlags);
    }

    return retval;
}

bool GeneralAllocator::AddCore(void *pCore,
    size_t nCoreSize,
    bool bShouldFreeCore,
    bool bShouldTrimCore,
    CoreFreeFunction pCoreFreeFunction,
    void *pCoreFreeFunctionContext)
{
    PPMAutoMutex lock(mpMutex);

    if (pCore != nullptr) {
        size_t core_size = nCoreSize;
        if (core_size >= (4 * kMinChunkSize)) {
            if (core_size % mnPageSize) {
                core_size &= ~(mnPageSize - 1);
            }

            CoreBlock *const pCoreBlock = static_cast<CoreBlock *>(pCore);
            pCoreBlock->mpCore = static_cast<Chunk *>(AlignUp(pCoreBlock + 1, kMinEffectiveAlignment));
            pCoreBlock->mbShouldFree = bShouldFreeCore;
            pCoreBlock->mbShouldTrim = bShouldTrimCore;
            pCoreBlock->mpCoreFreeFunction = pCoreFreeFunction;
            pCoreBlock->mpCoreFreeFunctionContext = pCoreFreeFunctionContext;
            pCoreBlock->mnSize = core_size;
            pCoreBlock->mnReservedSize = core_size;
            pCoreBlock->mbMMappedMemory = false;
            LinkCoreBlock(pCoreBlock, &mHeadCoreBlock);
#if ALLOCATOR_USEHOOKS
            if (mpHookFunction) {
                const HookInfo hookInfo = { this,
                    false,
                    kHookTypeCoreAlloc,
                    kHookSubTypeNone,
                    nCoreSize,
                    NULL,
                    0,
                    nCoreSize,
                    NULL,
                    0,
                    0,
                    pCoreBlock->mpCore,
                    NULL,
                    nCoreSize };
                mpHookFunction(&hookInfo, mpHookFunctionContext);
            }
#endif
#ifdef _WIN32
            if (pCoreFreeFunction == nullptr) {
                MEMORY_BASIC_INFORMATION mbi;
                mbi.BaseAddress = pCore;
                VirtualQuery(mbi.BaseAddress, &mbi, sizeof(mbi));

                if (mbi.State == MEM_RESERVE) {
                    pCoreBlock->mnSize = 0;
                }
            }
#endif

            // Make a chunk out of the new core
            Chunk *const pChunk = MakeChunkFromCore(pCoreBlock->mpCore,
                nCoreSize - size_t(ptrdiff_t(pCoreBlock->mpCore) - ptrdiff_t(pCoreBlock)),
                kChunkFlagPrevInUse);
            SetNewTopChunk(pChunk, mpTopChunk != GetInitialTopChunk());

            return true;
        }
    } else if (nCoreSize > 0) {
        return AddCoreInternal(AlignUp(nCoreSize, mnPageSize)) != nullptr;
    }

    return false;
}

size_t GeneralAllocator::TrimCore(size_t nPadding)
{
    return 0;
}

void GeneralAllocator::ClearCache() {}

void GeneralAllocator::SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void *pContext) {}

void GeneralAllocator::SetHookFunction(HookFunction pHookFunction, void *pContext) {}

void GeneralAllocator::SetOption(int32_t option, int32_t nValue) {}

size_t GeneralAllocator::GetUsableSize(const void *pData)
{
    return 0;
}

size_t GeneralAllocator::GetBlockSize(const void *pData, bool bNetSize)
{
    return 0;
}

size_t GeneralAllocator::GetLargestFreeBlock(bool bClearCache)
{
    return 0;
}

void GeneralAllocator::SetTraceFunction(TraceFunction pTraceFunction, void *pContext) {}

void GeneralAllocator::SetTraceFieldDelimiters(uint8_t fieldDelimiter, uint8_t recordDelimiter) {}

bool GeneralAllocator::ValidateHeap(HeapValidationLevel heapValidationLevel)
{
    return false;
}

void GeneralAllocator::SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency) {}

void GeneralAllocator::TraceAllocatedMemory(
    TraceFunction pTraceFunction, void *pTraceFunctionContext, void *pStorage, size_t nStorageSize, int32_t nBlockTypeFlags)
{
}

size_t GeneralAllocator::DescribeData(const void *pData, char *pBuffer, size_t nBufferLength)
{
    return 0;
}

const void *GeneralAllocator::ValidateAddress(const void *pAddress, int32_t addressType) const
{
    return nullptr;
}

bool GeneralAllocator::UnknownFunc()
{
    return false;
}

bool GeneralAllocator::IsAddressHigh(const void *pAddress)
{
    return false;
}

void *GeneralAllocator::TakeSnapshot(int32_t nBlockTypeFlags, bool bMakeCopy, void *pStorage, size_t nStorageSize)
{
    return nullptr;
}

void GeneralAllocator::FreeSnapshot(void *pSnapshot) {}

bool GeneralAllocator::ReportHeap(
    HeapReportFunction, void *pContext, int32_t nBlockTypeFlags, bool bMakeCopy, void *pStorage, size_t nStorageSize)
{
    return false;
}

const void *GeneralAllocator::ReportBegin(
    void *pSnapshot, int32_t nBlockTypeFlags, bool bMakeCopy, void *pStorage, size_t nStorageSize)
{
    return nullptr;
}

Allocator::GeneralAllocator::BlockInfo const *GeneralAllocator::ReportNext(const void *pContext, int32_t nBlockTypeFlags)
{
    return nullptr;
}

void GeneralAllocator::ReportEnd(const void *pContext) {}

void *GeneralAllocator::MallocInternal(size_t nSize, int32_t nAllocationFlags)
{
    return nullptr;
}

void *GeneralAllocator::CallocInternal(size_t nElementCount, size_t nElementSize, int32_t nAllocationFlags)
{
    return nullptr;
}

void *GeneralAllocator::ReallocInternal(void *pData, size_t nNewSize, int32_t nAllocationFlags)
{
    return nullptr;
}

void GeneralAllocator::FreeInternal(void *pData) {}

void *GeneralAllocator::MallocAlignedInternal(
    size_t nAlignment, size_t nSize, size_t nAlignmentOffset, int32_t nAllocationFlags)
{
    return nullptr;
}

void **GeneralAllocator::MallocMultipleInternal(
    size_t nElementCount, size_t nSizeCount, const size_t nElementSizes[], void *pResultArray[], int32_t nAllocationFlags)
{
    return nullptr;
}

GeneralAllocator::Chunk *GeneralAllocator::ExtendCoreInternal(size_t nMinSize)
{
    return nullptr;
}

GeneralAllocator::Chunk *GeneralAllocator::AddCoreInternal(size_t nMinSize)
{
    size_t nChunkFlags = kChunkFlagPrevInUse;
    size_t nCoreSize = AlignUp(nMinSize, mnPageSize);
    void *pCore = nullptr;

    if (nCoreSize < mnNewCoreSize) {
        nCoreSize = AlignUp(mnNewCoreSize, mnPageSize);
    }

#ifdef _WIN32
    size_t nReservedSize = AlignUp(nCoreSize, 65536);
    pCore = VirtualAlloc(nullptr, nReservedSize, MEM_RESERVE, PAGE_READWRITE);
#else
    while (true) {
        pCore = std::malloc(nCoreSize);

        if (pCore != nullptr) {
            break;
        }

        nCoreSize = nCoreSize * 3 / 4;

        if (nCoreSize < nMinSize) {
            break;
        }
    }
#endif
    if (pCore == nullptr) {
        pCore = MMapAllocInternal(nCoreSize, &nCoreSize);
#ifdef _WIN32
        if (pCore != nullptr) {
            nReservedSize = nCoreSize;
        }
#endif
    }

    if (pCore) {
        CoreBlock *const pCoreBlock = (CoreBlock *)pCore;
        pCoreBlock->mpCore = static_cast<Chunk *>(AlignUp(pCoreBlock + 1, kMinEffectiveAlignment));
        pCoreBlock->mnSize = nCoreSize;
#ifdef _WIN32
        pCoreBlock->mnReservedSize = nReservedSize;
#endif
        pCoreBlock->mbMMappedMemory = ((nChunkFlags & kChunkFlagMMapped) != 0);
        pCoreBlock->mbShouldFree = (mHeadCoreBlock.mpNextCoreBlock != &mHeadCoreBlock);
#ifdef ALLOCATOR_FREEONSHUTDOWN
        pCoreBlock->mbShouldFreeOnShutdown = true;
#endif
        pCoreBlock->mbShouldTrim = pCoreBlock->mbShouldFree;
        pCoreBlock->mpCoreFreeFunction = nullptr;
        pCoreBlock->mpCoreFreeFunctionContext = nullptr;
        LinkCoreBlock(pCoreBlock, &mHeadCoreBlock);
#ifdef _WIN32
        pCoreBlock->mnReservedSize = nReservedSize;
#endif
#if ALLOCATOR_USEHOOKS
        if (mpHookFunction) {
            // We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
            const HookInfo hookInfo = { this,
                false,
                kHookTypeCoreAlloc,
                kHookSubTypeNone,
                nCoreSize,
                NULL,
                0,
                nCoreSize,
                NULL,
                0,
                0,
                pCoreBlock->mpCore,
                NULL,
                nCoreSize };
            mpHookFunction(&hookInfo, mpHookFunctionContext);
        }
#endif

        return MakeChunkFromCore(
            pCoreBlock->mpCore, nCoreSize - size_t(ptrdiff_t(pCoreBlock->mpCore) - ptrdiff_t(pCoreBlock)), nChunkFlags);
    }

    return nullptr;
}

bool GeneralAllocator::FreeCore(CoreBlock *pCoreBlock, bool bInShutdown)
{
    bool shutdown = false;
#ifdef ALLOCATOR_FREEONSHUTDOWN
    shutdown = bInShutdown && pCoreBlock->mbShouldFreeOnShutdown;
#endif

    if (pCoreBlock->mbShouldFree || shutdown) {
        if (pCoreBlock->mpCoreFreeFunction != nullptr) {
            pCoreBlock->mpCoreFreeFunction(
                this, pCoreBlock, pCoreBlock->mnReservedSize, pCoreBlock->mpCoreFreeFunctionContext);
        } else {
#ifdef _WIN32
            VirtualFree(pCoreBlock, pCoreBlock->mnReservedSize, MEM_RELEASE);
#else
            std::free(pCoreBlock);
#endif
        }
        return true;
    }

    return false;
}

void *GeneralAllocator::MMapMalloc(size_t nSize)
{
    size_t nMMapChunkSize = GetMMapChunkSizeFromDataSize(nSize);

    if (nMMapChunkSize >= nSize) {
        uint8_t *pMMapMemory = static_cast<uint8_t *>(MMapAllocInternal(nMMapChunkSize, &nMMapChunkSize));

        if (pMMapMemory != nullptr) {
            ++mnMMapCount;
            mnMMapMallocTotal += nMMapChunkSize;
            nMMapChunkSize -= kMinChunkSize;
            size_t nMisalign = Misalignment(pMMapMemory + 8, kMinEffectiveAlignment);
            Chunk *pResultChunk = nullptr;

            if (!IsAligned(pMMapMemory + 8, kMinEffectiveAlignment)) {
                size_t nAlignDiff = kMinEffectiveAlignment - nMisalign;
                pResultChunk = reinterpret_cast<Chunk *>(pMMapMemory + nAlignDiff);

                pResultChunk->mnPriorSize = nAlignDiff;
                nMMapChunkSize -= nAlignDiff;
                SetChunkSize(pResultChunk, nMMapChunkSize | kChunkFlagMMapped);
            } else {
                pResultChunk = reinterpret_cast<Chunk *>(pMMapMemory);
                pResultChunk->mnPriorSize = 0;
                SetChunkSize(pResultChunk, nMMapChunkSize | kChunkFlagMMapped);
            }

            Chunk *const pFenceChunk = GetChunkAtOffset(pResultChunk, ptrdiff_t(nMMapChunkSize));
            pFenceChunk->mnPriorSize = nMMapChunkSize;
            pFenceChunk->mnSize = kMinChunkSize | kChunkFlagPrevInUse | kChunkFlagMMapped;
            LinkChunk(pFenceChunk, &mHeadMMapChunk, mHeadMMapChunk.mpNextChunk);
            return GetDataPtrFromChunkPtr(pResultChunk);
        }
    }

    return nullptr;
}

void *GeneralAllocator::MMapAllocInternal(size_t nSize, size_t *pSizeAllocated)
{
    return nullptr;
}

void GeneralAllocator::MMapFreeInternal(void *pAddress, size_t nSize) {}

void GeneralAllocator::ClearFastBins() {}

void GeneralAllocator::LinkCoreBlock(CoreBlock *pCoreBlock, CoreBlock *pNext)
{
    pCoreBlock->mpNextCoreBlock = pNext;
    pCoreBlock->mpPrevCoreBlock = pNext->mpPrevCoreBlock;
    pNext->mpPrevCoreBlock = pCoreBlock;
    pCoreBlock->mpPrevCoreBlock->mpNextCoreBlock = pCoreBlock;
}

void GeneralAllocator::UnlinkCoreBlock(CoreBlock *pCoreBlock)
{
    pCoreBlock->mpPrevCoreBlock->mpNextCoreBlock = pCoreBlock->mpNextCoreBlock;
    pCoreBlock->mpNextCoreBlock->mpPrevCoreBlock = pCoreBlock->mpPrevCoreBlock;
}

GeneralAllocator::CoreBlock *GeneralAllocator::FindCoreBlockForAddress(const void *pAddress)
{
    return nullptr;
}

int32_t GeneralAllocator::CheckChunk(const Chunk *pChunk)
{
    return 0;
}

int32_t GeneralAllocator::CheckFreeChunk(const Chunk *pChunk)
{
    return 0;
}

int32_t GeneralAllocator::CheckUsedChunk(const Chunk *pChunk)
{
    return 0;
}

int32_t GeneralAllocator::CheckRemallocedChunk(const Chunk *pChunk, size_t nRequestedChunkSize)
{
    return 0;
}

int32_t GeneralAllocator::CheckMallocedChunk(
    const Chunk *pChunk, size_t nRequestedChunkSize, bool bNewlyMalloced, bool bAllowPrevNotInUse)
{
    return 0;
}

int32_t GeneralAllocator::CheckMMappedChunk(const Chunk *pChunk)
{
    return 0;
}

int32_t GeneralAllocator::CheckState(HeapValidationLevel heapValidationLevel)
{
    return 0;
}

size_t GeneralAllocator::DescribeChunk(const Chunk *pChunk, char *pBuffer, size_t nBufferLength, bool bAppendLineEnd)
{
    return 0;
}

DataPreviewType GeneralAllocator::GetDataPreview(
    const void *pData, size_t nDataSize, char *pBuffer, wchar_t *pBufferW, size_t nBufferLength)
{
    return DataPreviewType();
}

void GeneralAllocator::SetNewTopChunk(Chunk *pNewTopChunk, bool bFreePreviousTopChunk)
{
    Chunk *const pPreviousTopChunk = mpTopChunk;
    mpTopChunk = pNewTopChunk;
    mpTopChunk->mpNextChunk = mpTopChunk;
    mpTopChunk->mpPrevChunk = mpTopChunk->mpNextChunk;

    if (pPreviousTopChunk != GetInitialTopChunk() && bFreePreviousTopChunk) {
        Chunk *const pUnsortedBinHeadChunk = GetUnsortedBin();
        LinkChunk(pPreviousTopChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
    }
}

GeneralAllocator::Chunk *GeneralAllocator::MakeChunkFromCore(void *pCore, size_t nCoreSize, size_t nFlags)
{
    Chunk *const pChunk = reinterpret_cast<Chunk *>(pCore);
    pChunk->mnPriorSize = 0;
    SetChunkSize(pChunk, size_t(nCoreSize | nFlags));
    AddDoubleFencepost(pChunk, 0);
    return pChunk;
}

void GeneralAllocator::AddDoubleFencepost(Chunk *pChunk, size_t nPrevChunkFlags)
{
    size_t nChunkSize = GetChunkSize(pChunk);
    nChunkSize = (nChunkSize - kDoubleFenceChunkSize) & ~kMinAlignmentMask;
    SetChunkSizePreserveFlags(pChunk, size_t(nChunkSize));
    Chunk *pFenceChunk = GetChunkAtOffset(pChunk, ptrdiff_t(nChunkSize));
    pFenceChunk->mnPriorSize = size_t(nChunkSize);
    SetChunkSize(pFenceChunk, kFenceChunkSize | nPrevChunkFlags);
    pFenceChunk = GetChunkAtOffset(pChunk, ptrdiff_t(nChunkSize + kFenceChunkSize));
    pFenceChunk->mnPriorSize = kFenceChunkSize;
    SetChunkSize(pFenceChunk, kFenceChunkSize | kChunkFlagPrevInUse);
}
} // namespace Allocator
