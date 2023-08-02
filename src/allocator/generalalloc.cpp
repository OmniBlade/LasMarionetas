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
#include <new>
#include <unistd.h>

namespace Allocator
{
GeneralAllocator::Snapshot::Snapshot(size_t nSizeOfThis, int nBlockTypeFlags)
{
    memset(this, 0, nSizeOfThis);
    mnMagicNumber = kSnapshotMagicNumber;
    mnSizeOfThis = nSizeOfThis;
    mnBlockTypeFlags = nBlockTypeFlags;
    mbUserAllocated = false;
    mbReport = false;
    mbDynamic = false;
    mbCoreBlockReported = false;
    mpCurrentCoreBlock = NULL;
    mpCurrentChunk = NULL;
    mpCurrentMChunk = NULL;
    mnBlockInfoCount = 0;
    mnBlockInfoIndex = 0;
}

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
#ifdef ALLOCATOR_USEHOOKS
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
    PPMAutoMutex lock(mpMutex);

    size_t nCoreTrimmed = 0;

    for (CoreBlock *pCoreBlock = mHeadCoreBlock.mpPrevCoreBlock; pCoreBlock != &mHeadCoreBlock;
         pCoreBlock = pCoreBlock->mpPrevCoreBlock) {
        if (pCoreBlock->mbShouldFree) {
            const Chunk *const pFenceChunk = GetFenceChunk(pCoreBlock);

            if (!GetPrevChunkIsInUse(pFenceChunk)) {
                Chunk *const pHighestChunk = GetPrevChunk(pFenceChunk);
#ifndef _WIN32 // TODO, found in ARM Fifa binary but is newer version of code with extra functionality.
                if ((char *)pHighestChunk < ((char *)pCoreBlock->mpCore + kMinChunkSize)) {
#ifdef ALLOCATOR_USEHOOKS
                    if (mpHookFunction) {
                        const HookInfo hookInfo = { this,
                            true,
                            kHookTypeCoreFree,
                            kHookSubTypeNone,
                            pCoreBlock->mnSize,
                            pCoreBlock->mpCore,
                            0,
                            pCoreBlock->mnSize,
                            NULL,
                            0,
                            0,
                            NULL,
                            NULL,
                            pCoreBlock->mnSize,
                            0 };
                        mpHookFunction(&hookInfo, mpHookFunctionContext);
                    }
#endif

                    UnlinkChunk(pHighestChunk);
                    CoreBlock *const pCoreBlockSaved = pCoreBlock;
                    pCoreBlock = pCoreBlock->mpNextCoreBlock;
                    UnlinkCoreBlock(pCoreBlockSaved);
                    const size_t nCoreBlockSize = pCoreBlockSaved->mnSize;

                    if (FreeCore(pCoreBlockSaved, false)) {
                        nCoreTrimmed += nCoreBlockSize;

                        if (pHighestChunk == mpTopChunk) {
                            mpTopChunk = GetInitialTopChunk();
                            // FindAndSetNewTopChunk();
                        }
                    } else {
                        // PlaceUnsortedChunkInBin(pHighestChunk, GetChunkSize(pHighestChunk), false);
                        LinkCoreBlock(pCoreBlockSaved, pCoreBlock);
                        pCoreBlock = pCoreBlockSaved;
                    }

                    continue;
                }
#endif
                if (pCoreBlock->mbShouldTrim) {
                    const size_t nHighestChunkSize = GetChunkSize(pHighestChunk);
                    const size_t nPaddingSize = (size_t)(nPadding + (2 * kMinChunkSize) + sizeof(CoreBlock));

                    if (nHighestChunkSize > nPaddingSize) {
#ifdef _WIN32
                        size_t nDecommitSizeIncrement = AlignUp(mnCoreIncrementSize / 2, 65536);
                        size_t nDecommitSize = nDecommitSizeIncrement;
                        size_t nDecommitLimit = nHighestChunkSize - nPaddingSize;

                        while ((nDecommitSize + nDecommitSizeIncrement) < nDecommitLimit) {
                            nDecommitSize += nDecommitSizeIncrement;
                        }

                        if (nDecommitSize < nDecommitLimit) {
                            char *const pVirtualFreeLocation = (char *)pCoreBlock + pCoreBlock->mnSize - nDecommitSize;

                            if (VirtualFree(pVirtualFreeLocation, nDecommitSize, MEM_DECOMMIT)) {
                                nCoreTrimmed += nDecommitSize;
                                pCoreBlock->mnSize -= nDecommitSize;

                                if (pHighestChunk != mpTopChunk) {
                                    UnlinkChunk(pHighestChunk);
                                }

                                SetChunkSizePreserveFlags(
                                    pHighestChunk, (size_t)((nHighestChunkSize - nDecommitSize) + kDoubleFenceChunkSize));
                                AddDoubleFencepost(pHighestChunk, 0);

                                if (pHighestChunk != mpTopChunk) {
                                    Chunk *const pUnsortedBinHeadChunk = GetUnsortedBin();
                                    LinkChunk(pHighestChunk, pUnsortedBinHeadChunk, pUnsortedBinHeadChunk->mpNextChunk);
                                }
                            }
                        }
#endif
                    }
                }
            }
        }
    }

    return nCoreTrimmed;
}

void GeneralAllocator::ClearCache()
{
    PPMAutoMutex lock(mpMutex);
    ClearFastBins();
}

void GeneralAllocator::SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void *pContext)
{
    mpMallocFailureFunction = pMallocFailureFunction;
    mpMallocFailureFunctionContext = pContext;
}

void GeneralAllocator::SetHookFunction(HookFunction pHookFunction, void *pContext)
{
#ifdef ALLOCATOR_USEHOOKS
    mpHookFunction = pHookFunction;
    mpHookFunctionContext = pContext;
#endif
}

void GeneralAllocator::SetOption(int32_t option, int32_t nValue)
{
    switch (option) {
        case kOptionEnableThreadSafety:
            if (nValue) {
                if (!mpMutex) {
                    mpMutex = PPMMutexCreate(mMutexData);
                }
            } else {
                if (mpMutex) {
                    PPMMutexLock(mpMutex);
                    void *const pMutex = mpMutex;
                    mpMutex = NULL;
                    PPMMutexUnlock(pMutex);
                    PPMMutexDestroy(pMutex);
                }
            }

            break;
        case kOptionEnableHighAllocation:
            if (nValue) {
                if (!mbHighFenceInternallyDisabled && !mpHighFence) {
                    mpHighFence = (void *)(1);
                }
            } else {
                mpHighFence = nullptr;
            }
            break;

        case kOptionEnableSystemAlloc:
            mbSystemAllocEnabled = (nValue != 0);
            break;

        case kOptionNewCoreSize:
            if (nValue == 0) {
                mnNewCoreSize = kDefaultNewCoreSize;
            } else {
                mnNewCoreSize = ((size_t)nValue > mnPageSize ? (size_t)nValue : mnPageSize);
                mnNewCoreSize = AlignUp(mnNewCoreSize, mnPageSize);
            }

            break;

        case kOptionCoreIncrementSize:
            if (nValue == 0) {
                mnCoreIncrementSize = kDefaultCoreIncrementSize;
            } else {
                mnCoreIncrementSize = ((size_t)nValue > mnPageSize ? (size_t)nValue : mnPageSize);
                mnCoreIncrementSize = AlignUp(mnCoreIncrementSize, mnPageSize);

                if (mnCoreIncrementSize > mnNewCoreSize) {
                    mnCoreIncrementSize = mnNewCoreSize;
                }
            }
            break;

        case kOptionMaxFastBinRequestSize:
            ClearFastBins();
            SetMaxFastBinDataSize((size_t)nValue);
            break;

        case kOptionTrimThreshold:
            mnTrimThreshold = (size_t)nValue;
            break;

        case kOptionTopPad:
            mnTopPad = (size_t)nValue;
            break;

        case kOptionMMapThreshold:
            mnMMapThreshold = (size_t)nValue;
            break;

        case kOptionMMapMaxAllowed:
            mnMMapMaxAllowed = (int32_t)nValue;
            break;

        case kOptionMMapTopDown:
            mbMMapTopDown = (nValue != 0);
            break;

        case kOptionTraceInternalMemory:
            mbTraceInternalMemory = (nValue != 0);
            break;

        case kOptionMaxMallocFailureCount:
            mnMaxMallocFailureCount = (uint32_t)nValue;
            break;
    }
}

size_t GeneralAllocator::GetUsableSize(const void *pData) const
{
    PPMAutoMutex lock(mpMutex);

    if (pData) {
        const Chunk *const pChunk = GetChunkPtrFromDataPtr(pData);
        const size_t nChunkSize = GetChunkSize(pChunk);

        if (GetChunkIsMMapped(pChunk)) {
            return nChunkSize - kDataPtrOffset;
        } else if (GetChunkIsInUse(pChunk)) {
            return (nChunkSize - kDataPtrOffset);
        }
    }

    return 0;
}

size_t GeneralAllocator::GetBlockSize(const void *pData, bool bNetSize)
{
    PPMAutoMutex lock(mpMutex);

    const Chunk *const pChunk = GetChunkPtrFromDataPtr(pData);
    size_t nChunkSize = GetChunkSize(pChunk);

    if (bNetSize) {
        nChunkSize -= kSizeTypeSize;
    }

    return nChunkSize;
}

size_t GeneralAllocator::GetLargestFreeBlock(bool bClearCache)
{
    PPMAutoMutex lock(mpMutex);
    size_t nLargest = 0;

    if (mpTopChunk) {
        if (bClearCache) {
            ClearFastBins();
        }

        nLargest = GetChunkSize(mpTopChunk);

        for (size_t i((kBinCount)-1); i >= 1; --i) {
            const Chunk *const pBin = GetBin((int32_t)i);

            if (pBin->mpNextChunk != pBin) {
                const size_t n = GetChunkSize(pBin->mpNextChunk);
                if (n > nLargest) {
                    nLargest = n;
                }

                break;
            }
        }

        const Chunk *const pBegin = GetUnsortedBin();
        for (const Chunk *pChunk = pBegin->mpNextChunk; pChunk != pBegin; pChunk = pChunk->mpNextChunk) {
            const size_t n = GetChunkSize(pChunk);

            if (n > nLargest) {
                nLargest = n;
            }
        }

        // BFME2ex WB
        if (!bClearCache && nLargest < mnMaxFastBinChunkSize) {
            for (int i = (kFastBinCount - 1); i >= 0; i--) {
                const Chunk *pChunk = mpFastBinArray[i];

                if (pChunk) {
                    const size_t n = GetChunkSize(pChunk);

                    if (n > nLargest) {
                        nLargest = n;
                    }

                    break;
                }
            }
        }
    }

    return nLargest;
}

void GeneralAllocator::SetTraceFunction(TraceFunction pTraceFunction, void *pContext)
{
    mpTraceFunction = pTraceFunction;
    mpTraceFunctionContext = pContext;
}

void GeneralAllocator::SetTraceFieldDelimiters(uint8_t fieldDelimiter, uint8_t recordDelimiter)
{
    mcTraceFieldDelimiter = fieldDelimiter;
    mcTraceRecordDelimiter = recordDelimiter;
}

bool GeneralAllocator::ValidateHeap(HeapValidationLevel heapValidationLevel)
{
    int nErrorCount = 0;

    if (!mbHeapValidationActive) {
        mbHeapValidationActive = true;
        nErrorCount += CheckState(heapValidationLevel);
        mbHeapValidationActive = false;
    }

    return nErrorCount == 0;
}

void GeneralAllocator::SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency)
{
    mnAutoHeapValidationFrequency = nFrequency > 0 ? nFrequency : 1;
    mAutoHeapValidationLevel = heapValidationLevel;
    mnAutoHeapValidationEventCount = 0;
}

void GeneralAllocator::TraceAllocatedMemory(
    TraceFunction pTraceFunction, void *pTraceFunctionContext, void *pStorage, size_t nStorageSize, int32_t nBlockTypeFlags)
{
    char pTraceBuffer[4000];
    PPMAutoMutex lock(mpMutex);

    if (GetFastBinChunksExist()) {
        ClearFastBins();
    }

    if (!pTraceFunction) {
        pTraceFunction = mpTraceFunction;
    }

    if (!pTraceFunctionContext) {
        pTraceFunctionContext = mpTraceFunctionContext;
    }

    if (pTraceFunction) {
        const void *const pContext = ReportBegin(NULL, nBlockTypeFlags, false, pStorage, nStorageSize);

        for (const BlockInfo *pBlockInfo = ReportNext(pContext); pBlockInfo; pBlockInfo = ReportNext(pContext)) {
            const Chunk *pChunk = (const Chunk *)pBlockInfo->mpCore;

            if (mbTraceInternalMemory || !GetChunkIsInternal(pChunk)) {
                DescribeChunk(pChunk, pTraceBuffer, sizeof(pTraceBuffer) / sizeof(pTraceBuffer[0]), true);
                pTraceFunction(pTraceBuffer, pTraceFunctionContext);
            }
        }

        ReportEnd(pContext);
    }
}

size_t GeneralAllocator::DescribeData(const void *pData, char *pBuffer, size_t nBufferLength)
{
    PPMAutoMutex lock(mpMutex);

    return DescribeChunk(GetChunkPtrFromDataPtr(pData), pBuffer, nBufferLength, true);
}

bool GeneralAllocator::ValidateAddress(const void *pAddress, bool addressType) const
{
    PPMAutoMutex lock(mpMutex);
    const CoreBlock *const pCoreBlock = FindCoreBlockForAddress(pAddress);

    if (pCoreBlock) {
        const Chunk *pCurrentChunk = (Chunk *)pCoreBlock->mpCore;
        const Chunk *pNextChunk = GetNextChunk(pCurrentChunk);

        while (pNextChunk < pAddress && pNextChunk != pCurrentChunk) {
            pCurrentChunk = pNextChunk;
            pNextChunk = GetNextChunk(pNextChunk);
        }

        if (GetChunkIsInUse(pCurrentChunk)) {
            const void *const pData = GetDataPtrFromChunkPtr(pCurrentChunk);

            if (addressType == kAddressTypeSpecific) {
                if (pAddress == pData) {
                    return true;
                }
            } else if (pAddress >= pData && (Chunk *)pAddress < pNextChunk) {
                return true;
            }
        }
    }

    return false;
}

bool GeneralAllocator::UnknownFunc()
{
    return true;
}

bool GeneralAllocator::IsAddressHigh(const void *pAddress)
{
    for (CoreBlock *pCoreBlock = mHeadCoreBlock.mpNextCoreBlock; pCoreBlock != &mHeadCoreBlock;
         pCoreBlock = pCoreBlock->mpNextCoreBlock) {
        if (pAddress >= (void *)pCoreBlock->mpCore && pAddress < (void *)(pCoreBlock->mpCore + pCoreBlock->mnSize)) {
            return pAddress > mpHighFence;
        }
    }

    return false;
}

void *GeneralAllocator::TakeSnapshot(int32_t nBlockTypeFlags, bool bMakeCopy, void *pStorage, size_t nStorageSize)
{
    PPMAutoMutex lock(mpMutex);
    Snapshot *pSnapshot = NULL;

    if (bMakeCopy) {
        if (GetFastBinChunksExist()) {
            ClearFastBins();
        }

        size_t nBlockCount = 0;

        for (size_t nPass = 0; nPass < 2; nPass++) {
            size_t nBlockIndex = 0;

            if (nPass) {
                const size_t kBlockCountExtra = 4;
                nBlockCount += kBlockCountExtra;
                const size_t nSizeOfSnapshot = sizeof(Snapshot) + (nBlockCount * sizeof(BlockInfo));

                if (pStorage && nStorageSize < nSizeOfSnapshot) {
                    pStorage = NULL;
                }

                void *const pData = pStorage ? pStorage : MallocInternal(nSizeOfSnapshot, kAllocationFlagInternal);

                if (pData) {
                    SetChunkIsInternal(GetChunkPtrFromDataPtr(pData));
                    pSnapshot = new (pData) Snapshot(nSizeOfSnapshot, nBlockTypeFlags);
                    pSnapshot->mnSizeOfThis = nSizeOfSnapshot;
                    pSnapshot->mbUserAllocated = (pStorage != NULL);
                    pSnapshot->mbDynamic = false;
                    pSnapshot->mnBlockInfoCount = nBlockCount;
                    pSnapshot->mnBlockInfoIndex = 0;
                }

                if (nBlockCount == kBlockCountExtra) {
                    break;
                }
            }

            CoreBlock *pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
            while (pCoreBlock != &mHeadCoreBlock) {
                if (nBlockTypeFlags & kBlockTypeCore) {
                    if (nPass && (nBlockIndex < nBlockCount)) {
                        GetBlockInfoForCoreBlock(pCoreBlock, &pSnapshot->mBlockInfo[nBlockIndex]);
                    }

                    nBlockIndex++;
                }

                if (nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) {
                    const Chunk *pChunk = (Chunk *)pCoreBlock->mpCore;
                    const Chunk *pFenceChunk = GetFenceChunk(pCoreBlock);

                    while (pChunk < pFenceChunk) {
                        const size_t bIsInUse = GetChunkIsInUse(pChunk);

                        if (bIsInUse) {
                            if (nBlockTypeFlags & kBlockTypeAllocated) {
                                if ((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk)) {
                                    if (nPass && (nBlockIndex < nBlockCount))
                                        GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[nBlockIndex]);
                                    nBlockIndex++;
                                }
                            }
                        } else {
                            if (nBlockTypeFlags & kBlockTypeFree) {
                                if ((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk)) {
                                    if (nPass && (nBlockIndex < nBlockCount))
                                        GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[nBlockIndex]);
                                    nBlockIndex++;
                                }
                            }
                        }

                        if (!bIsInUse && (pChunk == pChunk->mpNextChunk)) {
                            break;
                        }

                        pChunk = GetNextChunk(pChunk);
                    }
                }

                pCoreBlock = pCoreBlock->mpNextCoreBlock;
            }

            if (nBlockTypeFlags & kBlockTypeAllocated) {
                const Chunk *pChunk = mHeadMMapChunk.mpNextChunk;

                while (pChunk != &mHeadMMapChunk) {
                    const Chunk *const pMMappedChunk = GetMMapChunkFromMMapListChunk(pChunk);

                    if ((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk)) {
                        if (nPass && (nBlockIndex < nBlockCount)) {
                            GetBlockInfoForChunk(pMMappedChunk, &pSnapshot->mBlockInfo[nBlockIndex]);
                        }

                        nBlockIndex++;
                    }

                    pChunk = pChunk->mpNextChunk;
                }
            }

            nBlockCount = nBlockIndex;

            if (nPass) {
                pSnapshot->mnBlockInfoCount = nBlockIndex;
            }
        }
    } else {
        if (pStorage && (nStorageSize < sizeof(Snapshot))) {
            pStorage = NULL;
        }

        const size_t nSizeOfSnapshot = sizeof(Snapshot);
        void *const pData = pStorage ? pStorage : MallocInternal(nSizeOfSnapshot, kAllocationFlagInternal);

        if (pData) {
            SetChunkIsInternal(GetChunkPtrFromDataPtr(pData));
            pSnapshot = new (pData) Snapshot(nSizeOfSnapshot, nBlockTypeFlags);
            pSnapshot->mnSizeOfThis = nSizeOfSnapshot;
            pSnapshot->mbUserAllocated = (pStorage != NULL);
            pSnapshot->mbDynamic = true;
            pSnapshot->mbCoreBlockReported = false;
            pSnapshot->mpCurrentCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
            pSnapshot->mpCurrentChunk = NULL;
            pSnapshot->mpCurrentMChunk = mHeadMMapChunk.mpNextChunk;
        }
    }

    return pSnapshot;
}

void GeneralAllocator::FreeSnapshot(void *pContext)
{
    Snapshot *const pSnapshot = (Snapshot *)pContext;

    if (pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber) {
        if (!pSnapshot->mbUserAllocated) {
            FreeInternal(pSnapshot);
        }
    }
}

bool GeneralAllocator::ReportHeap(HeapReportFunction pHeapReportFunction,
    void *pContext,
    int32_t nBlockTypeFlags,
    bool bMakeCopy,
    void *pStorage,
    size_t nStorageSize)
{
    bool bReturnValue = false;

    if (pHeapReportFunction) {
        Snapshot *const pSnapshot = (Snapshot *)ReportBegin(NULL, nBlockTypeFlags, bMakeCopy, pStorage, nStorageSize);

        if (pSnapshot) {
            const BlockInfo *pBlockInfo = ReportNext(pSnapshot, nBlockTypeFlags);
            bReturnValue = true;

            while (bReturnValue && pBlockInfo) {
                bReturnValue = pHeapReportFunction(pBlockInfo, pContext);
                pBlockInfo = ReportNext(pSnapshot, nBlockTypeFlags);
            }
        }

        ReportEnd(pSnapshot);
    }

    return bReturnValue;
}

const void *GeneralAllocator::ReportBegin(
    void *pContext, int32_t nBlockTypeFlags, bool bMakeCopy, void *pStorage, size_t nStorageSize)
{
    Snapshot *pSnapshot;

    if (pContext) {
        pSnapshot = (Snapshot *)pContext;

        if (pSnapshot->mnMagicNumber != Snapshot::kSnapshotMagicNumber) {
            pSnapshot = NULL;
        }
    } else {
        pSnapshot = (Snapshot *)TakeSnapshot(nBlockTypeFlags, bMakeCopy, pStorage, nStorageSize);

        if (pSnapshot) {
            pSnapshot->mbReport = true;
        }
    }

    return pSnapshot;
}

const Allocator::GeneralAllocator::BlockInfo *GeneralAllocator::ReportNext(const void *pContext, int32_t nBlockTypeFlags)
{
    Snapshot *const pSnapshot = (Snapshot *)pContext;

    if (pSnapshot) {
        if (pSnapshot->mnMagicNumber == Snapshot::kSnapshotMagicNumber) {
            if (pSnapshot->mbDynamic) {
                nBlockTypeFlags &= pSnapshot->mnBlockTypeFlags;

                while (pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock) {
                    const Chunk *pFenceChunk = NULL;

                    if (pSnapshot->mpCurrentCoreBlock->mnSize) {
                        pFenceChunk = GetFenceChunk(pSnapshot->mpCurrentCoreBlock);

                        if (pSnapshot->mpCurrentChunk) {
                            pSnapshot->mpCurrentChunk = GetNextChunk(pSnapshot->mpCurrentChunk);
                        } else {
                            pSnapshot->mpCurrentChunk = (Chunk *)pSnapshot->mpCurrentCoreBlock->mpCore;
                        }

                        while ((pSnapshot->mpCurrentChunk != pFenceChunk)
                            && !ChunkMatchesBlockType(pSnapshot->mpCurrentChunk, nBlockTypeFlags)) {
                            pSnapshot->mpCurrentChunk = GetNextChunk(pSnapshot->mpCurrentChunk);
                        }

                        if (pSnapshot->mpCurrentChunk != pFenceChunk) {
                            break;
                        }
                    }

                    if (!pSnapshot->mpCurrentCoreBlock->mnSize || (pSnapshot->mpCurrentChunk == pFenceChunk)) {
                        do {
                            pSnapshot->mpCurrentCoreBlock = pSnapshot->mpCurrentCoreBlock->mpNextCoreBlock;
                        } while (
                            (pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock) && !pSnapshot->mpCurrentCoreBlock->mnSize);

                        pSnapshot->mpCurrentChunk = NULL;
                    }
                }

                if (pSnapshot->mpCurrentCoreBlock != &mHeadCoreBlock) {
                    GetBlockInfoForChunk(pSnapshot->mpCurrentChunk, &pSnapshot->mBlockInfo[0]);
                    return &pSnapshot->mBlockInfo[0];
                } else {
                    if (nBlockTypeFlags & kBlockTypeAllocated && pSnapshot->mpCurrentMChunk != &mHeadMMapChunk) {
                        const Chunk *const pChunk = GetMMapChunkFromMMapListChunk(pSnapshot->mpCurrentMChunk);
                        pSnapshot->mpCurrentMChunk = pSnapshot->mpCurrentMChunk->mpNextChunk;
                        GetBlockInfoForChunk(pChunk, &pSnapshot->mBlockInfo[0]);
                        return &pSnapshot->mBlockInfo[0];
                    }
                }
            } else {
                while ((pSnapshot->mnBlockInfoIndex < pSnapshot->mnBlockInfoCount)
                    && !(pSnapshot->mBlockInfo[pSnapshot->mnBlockInfoIndex].mBlockType & nBlockTypeFlags)) {
                    ++pSnapshot->mnBlockInfoIndex;
                }

                if (pSnapshot->mnBlockInfoIndex < pSnapshot->mnBlockInfoCount) {
                    return (pSnapshot->mBlockInfo + pSnapshot->mnBlockInfoIndex++);
                }
            }
        }
    }

    return NULL;
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
#ifdef ALLOCATOR_USEHOOKS
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

GeneralAllocator::CoreBlock *GeneralAllocator::FindCoreBlockForAddress(const void *pAddress) const
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

void GeneralAllocator::GetBlockInfoForCoreBlock(const CoreBlock *pCoreBlock, BlockInfo *pBlockInfo) const
{
    pBlockInfo->mBlockType = kBlockTypeCore;
    pBlockInfo->mpCore = pCoreBlock->mpCore;
    pBlockInfo->mnBlockSize = pCoreBlock->mnSize;
    pBlockInfo->mnDataSize = pCoreBlock->mnReservedSize;
    pBlockInfo->mbMemoryMapped = false;
}

void GeneralAllocator::GetBlockInfoForChunk(const Chunk *pChunk, BlockInfo *pBlockInfo) const
{
    const size_t bIsAllocated = GetChunkIsInUse(pChunk);
    const size_t bIsMMapped = GetChunkIsMMapped(pChunk);
    const size_t nChunkSize = GetChunkSize(pChunk);

    if (bIsAllocated) {
        const size_t bIsInternal = GetChunkIsInternal(pChunk);
        const size_t nUsableSize = GetUsableSize(GetDataPtrFromChunkPtr(pChunk));
        pBlockInfo->mBlockType = kBlockTypeAllocated;

        if (bIsMMapped) {
            pBlockInfo->mpCore = pChunk;
            pBlockInfo->mnBlockSize = pChunk->mnPriorSize + nChunkSize + kMinChunkSize;
            pBlockInfo->mpData = GetDataPtrFromChunkPtr(pChunk);
            pBlockInfo->mnDataSize = nUsableSize;
            pBlockInfo->mbMemoryMapped = true;
        } else {
            pBlockInfo->mpCore = pChunk;
            pBlockInfo->mnBlockSize = nChunkSize;
            pBlockInfo->mpData = GetDataPtrFromChunkPtr(pChunk);
            pBlockInfo->mnDataSize = nUsableSize;
            pBlockInfo->mbMemoryMapped = false;
        }
    } else {
        pBlockInfo->mBlockType = kBlockTypeFree;
        pBlockInfo->mpCore = pChunk;
        pBlockInfo->mnBlockSize = nChunkSize;
        pBlockInfo->mpData = GetPostHeaderPtrFromChunkPtr(pChunk);
        pBlockInfo->mnDataSize = GetChunkSize(pChunk) - sizeof(Chunk);
        pBlockInfo->mbMemoryMapped = false;
    }
}

int GeneralAllocator::ChunkMatchesBlockType(const Chunk *pChunk, int nBlockTypeFlags)
{
    if ((nBlockTypeFlags & kBlockTypeInternal) || !GetChunkIsInternal(pChunk)) {
        if ((nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) == (kBlockTypeAllocated | kBlockTypeFree)) {
            return 1;
        }

        const int bIsAllocated = GetChunkIsInUse(pChunk) && !GetChunkIsFastBin(pChunk);

        if (nBlockTypeFlags & kBlockTypeAllocated) {
            return bIsAllocated;
        } else if (nBlockTypeFlags & kBlockTypeFree) {
            return bIsAllocated ? 0 : 1;
        }
    }

    return 0;
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
