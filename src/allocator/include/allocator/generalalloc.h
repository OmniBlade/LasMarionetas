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
#pragma once

#include "allocmutex.h"
#include <stddef.h>
#include <stdint.h>

// Sims 2 uses more or less the same version of the allocator also used in BFME2 ROTWK where some functions are virtual.
#ifdef ALLOCATOR_VIRTUALS
#define alloc_virtual virtual
#else
#define alloc_virtual
#endif

namespace Allocator
{
enum DataPreviewType
{
    kDataPreviewTypeBinary,
    kDataPreviewTypeChar,
    kDataPreviewTypeWChar,
};

class GeneralAllocator
{
public:
    enum OptionsType
    {
        kOptionEnableThreadSafety
    };

    enum HookType
    {
        kHookTypeMalloc,
        kHookTypeFree,
        kHookTypeCoreAlloc,
        kHookTypeCoreFree,
        kHookTypeCoreExtended
    };

    enum HookSubType
    {
        kHookSubTypeNone,
        kHookSubTypeMalloc,
        kHookSubTypeCalloc,
        kHookSubTypeRealloc,
        kHookSubTypeMallocAligned,
        kHookSubTypeMallocMultiple1,
        kHookSubTypeMallocMultiple2
    };

    struct HookInfo
    {
        GeneralAllocator *mpGeneralAllocator;
        bool mbEntry;
        HookType mHookType;
        HookSubType mHookSubType;
        size_t mnSizeInputTotal;
        const void *mpDataInput;
        size_t mnCountInput;
        size_t mnSizeInput;
        const size_t *mpSizeInputArray;
        size_t mnAlignmentInput;
        int32_t mnAllocationFlags;
        void *mpDataOutput;
        void **mpArrayOutput;
        size_t mnSizeOutput;
        uint64_t mEventId;
    };

    enum HeapValidationLevel
    {
        kHeapValidationLevelNone,
        kHeapValidationLevelBasic,
        kHeapValidationLevelDetail,
        kHeapValidationLevelFull
    };

    enum BlockType
    {
        kBlockTypeNone = 0,
        kBlockTypeInvalid = 1,
        kBlockTypeAllocated = 2,
        kBlockTypeFree = 4,
        kBlockTypeCore = 8,
        kBlockTypeInternal = 16,
        kBlockTypeAll = 31
    };

    enum AddressType
    {
        kAddressTypeOwned = -1,
        kAddressTypeAllocated,
        kAddressTypeSpecific,
    };

    struct BlockInfo
    {
        const void *mpCore;
        size_t mnBlockSize;
        void *mpData;
        size_t mnDataSize;
        char mBlockType;
        bool mbMemoryMapped;
    };

    typedef size_t (*CoreFreeFunction)(GeneralAllocator *, void *, size_t, void *);
    typedef size_t (*CoreExtendFunction)(GeneralAllocator *, void *, size_t, size_t, size_t, void *);
    typedef bool (*MallocFailureFunction)(GeneralAllocator *, size_t, size_t, void *);
    typedef void (*HookFunction)(const HookInfo *, void *);
    typedef void (*TraceFunction)(const char *, void *);
    typedef bool (*HeapReportFunction)(const BlockInfo *, void *);
    typedef void (*AssertionFailureInfoFunction)(const char *, void *);

protected:
    struct Chunk
    {
        size_t mnPriorSize;
        size_t mnSize;
        Chunk *mpPrevChunk;
        Chunk *mpNextChunk;
    };

    struct CoreBlock
    {
        Chunk *mpCore;
        size_t mnSize;
        size_t mnReservedSize;
        bool mbMMappedMemory;
        bool mbShouldFree;
#ifdef ALLOCATOR_FREEONSHUTDOWN // Seemingly the only addition in BFME2?
        bool mbShouldFreeOnShutdown;
#endif
        bool mbShouldTrim;
        CoreFreeFunction mpCoreFreeFunction;
        void *mpCoreFreeFunctionContext;
        CoreBlock *mpPrevCoreBlock;
        CoreBlock *mpNextCoreBlock;
    };

    static constexpr size_t kMinChunkSize = sizeof(Chunk);
    static constexpr size_t kSizeTypeSize = sizeof(size_t);
    static constexpr size_t kDataPtrOffset = 2 * kSizeTypeSize;
    static constexpr size_t kMinEffectiveAlignment = 2 * kSizeTypeSize;
    static constexpr size_t kMinAlignmentMask = kMinEffectiveAlignment - 1;
    static constexpr size_t kDefaultMMapThreshold = 0x20000;
    static constexpr size_t kDefaultTrimThreshold = 0x20000;
    static constexpr size_t kDefaultTopPad = 0x10000;
    static constexpr size_t kChunkInternalHeaderSize = 2 * kSizeTypeSize;
    static constexpr size_t kFenceChunkSize = kChunkInternalHeaderSize;
    static constexpr size_t kDoubleFenceChunkSize = 2 * kFenceChunkSize;
    static constexpr size_t kChunkFlagPrevInUse = 1;
    static constexpr size_t kChunkFlagMMapped = 2;
    static constexpr size_t kChunkFlagInternal = 4;
    static constexpr size_t kChunkFlagFastBin = size_t(1ull << ((sizeof(size_t) * 8) - 1));
    static constexpr size_t kChunkFlagAll =
        (kChunkFlagPrevInUse | kChunkFlagMMapped | kChunkFlagInternal | kChunkFlagFastBin);
    static constexpr size_t kChunkSizeMask = ~kChunkFlagAll;
    static constexpr uint32_t kDefaultPageSize = 4096;
    static constexpr uint32_t kDefaultNewCoreSize = 0x1000000;
    static constexpr uint32_t kDefaultCoreIncrementSize = 0x400000;
    static constexpr int32_t kDefaultMMapMaxAllowed = 65536;
    static constexpr uint8_t kDefaultFillValueFree = 0xDD;
    static constexpr uint8_t kDefaultFillValueDelayedFree = 0xDE;
    static constexpr uint8_t kDefaultFillValueNew = 0xCD;
    static constexpr uint8_t kDefaultFillValueGuard = 0xAB;
    static constexpr uint8_t kDefaultFillValueUnusedCore = 0xFE;

    enum Bin
    {
        kBinCount = 128,
        kSmallBinSizeIncrement = 8,
        kLargeBinMinChunkSize = 512,
        kMinSortedBinChunkSize = 512,
        kUnsortedBinIndex = 1,
        kUnsortedBinIndexEnd = 2,
        kUnsortedBinCount = kUnsortedBinIndexEnd - kUnsortedBinIndex,
        kSmallBinIndex = kUnsortedBinIndex + kUnsortedBinCount,
        kSmallBinIndexEnd = kLargeBinMinChunkSize / kSmallBinSizeIncrement,
        kSmallBinCount = kSmallBinIndexEnd - kSmallBinIndex,
        kLargeBinIndex = kSmallBinIndex + kSmallBinCount,
        kLargeBinIndexEnd = kBinCount,
        kLargeBinCount = kLargeBinIndexEnd - kLargeBinIndex,
        kBinPointerCount = 2,
        kBinBitmapShift = 5,
        kBitsPerBinBitmapWord = 1 << kBinBitmapShift,
        kBinBitmapWordCount = kBinCount / kBitsPerBinBitmapWord
    };

    enum FastBin
    {
        kFastBinConsolidationThreshold = 65536,
        kFastBinDefaultMaxDataSize = 64,
        kFastBinMaxDataSize = 80,
        kFastBinChunkSizeMax = (2 * kSizeTypeSize) + 80,
        kFastBinChunkSizeMin = kMinChunkSize,
        kFastBinChunkSizeIncrement = 8,
        kFastBinChunkSizeShift = 3,
        kFastBinIndexMin = 2,
        kFastBinCount = (kFastBinChunkSizeMax / kFastBinChunkSizeIncrement) - kFastBinIndexMin + 1,
        kFlagFastBinChunksExist = 1,
    };

public:
    GeneralAllocator(void *pInitialCore = nullptr,
        size_t nInitialCoreSize = 0,
        bool bShouldFreeInitialCore = true,
        bool bShouldTrimInitialCore = false,
        CoreFreeFunction pInitialCoreFreeFunction = nullptr,
        void *pInitialCoreFreeFunctionContext = nullptr);
    alloc_virtual ~GeneralAllocator();
    alloc_virtual bool Init(void *pInitialCore = nullptr,
        size_t nInitialCoreSize = 0,
        bool bShouldFreeInitialCore = true,
        bool bShouldTrimInitialCore = false,
        CoreFreeFunction pInitialCoreFreeFunction = nullptr,
        void *pInitialCoreFreeFunctionContext = nullptr);
    alloc_virtual bool Shutdown();
    alloc_virtual void *Malloc(size_t nSize, int32_t nAllocationFlags = 0);
    alloc_virtual void *Calloc(size_t nElementCount, size_t nElementSize, int32_t nAllocationFlags = 0);
    alloc_virtual void *Realloc(void *pData, size_t nNewSize, int32_t nAllocationFlags = 0);
    alloc_virtual void Free(void *pData);
    alloc_virtual void *MallocAligned(
        size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, int32_t nAllocationFlags = 0);
    alloc_virtual void **MallocMultiple(
        size_t nElementCount, size_t nElementSize, void *pResultArray[], int32_t nAllocationFlags = 0);
    alloc_virtual void **MallocMultiple(
        size_t nElementCount, const size_t nElementSizes[], void *pResultArray[], int32_t nAllocationFlags = 0);
    alloc_virtual bool AddCore(void *pCore,
        size_t nSize,
        bool bShouldFreeCore = false,
        bool bShouldTrimCore = false,
        CoreFreeFunction pCoreFreeFunction = nullptr,
        void *pCoreFreeFunctionContext = nullptr);
    alloc_virtual size_t TrimCore(size_t nPadding);
    alloc_virtual void ClearCache();
    alloc_virtual void SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void *pContext);
    alloc_virtual void SetHookFunction(HookFunction pHookFunction, void *pContext);
    alloc_virtual void SetOption(int32_t option, int32_t nValue);
    alloc_virtual size_t GetUsableSize(const void *pData);
    alloc_virtual size_t GetBlockSize(const void *pData, bool bNetSize = false);
    alloc_virtual size_t GetLargestFreeBlock(bool bClearCache = false);
    alloc_virtual void SetTraceFunction(TraceFunction pTraceFunction, void *pContext);
    alloc_virtual void SetTraceFieldDelimiters(uint8_t fieldDelimiter, uint8_t recordDelimiter);
    alloc_virtual bool ValidateHeap(HeapValidationLevel heapValidationLevel = kHeapValidationLevelBasic);
    alloc_virtual void SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency);
    alloc_virtual void TraceAllocatedMemory(TraceFunction pTraceFunction = nullptr,
        void *pTraceFunctionContext = nullptr,
        void *pStorage = nullptr,
        size_t nStorageSize = 0,
        int32_t nBlockTypeFlags = kBlockTypeAllocated);
    alloc_virtual size_t DescribeData(const void *pData, char *pBuffer, size_t nBufferLength);
    alloc_virtual const void *ValidateAddress(const void *pAddress, int32_t addressType = kAddressTypeAllocated) const;
    alloc_virtual bool UnknownFunc();
    alloc_virtual bool IsAddressHigh(const void *pAddress);
    alloc_virtual void *TakeSnapshot(
        int32_t nBlockTypeFlags = kBlockTypeAll, bool bMakeCopy = false, void *pStorage = nullptr, size_t nStorageSize = 0);
    alloc_virtual void FreeSnapshot(void *pSnapshot);
    alloc_virtual bool ReportHeap(HeapReportFunction,
        void *pContext,
        int32_t nBlockTypeFlags,
        bool bMakeCopy = false,
        void *pStorage = nullptr,
        size_t nStorageSize = 0);
    alloc_virtual const void *ReportBegin(void *pSnapshot = nullptr,
        int32_t nBlockTypeFlags = kBlockTypeAll,
        bool bMakeCopy = false,
        void *pStorage = nullptr,
        size_t nStorageSize = 0);
    alloc_virtual BlockInfo const *ReportNext(const void *pContext, int32_t nBlockTypeFlags = kBlockTypeAll);
    alloc_virtual void ReportEnd(const void *pContext);

protected:
    alloc_virtual void *MallocInternal(size_t nSize, int32_t nAllocationFlags = 0);
    alloc_virtual void *CallocInternal(size_t nElementCount, size_t nElementSize, int32_t nAllocationFlags = 0);
    alloc_virtual void *ReallocInternal(void *p, size_t nNewSize, int32_t nAllocationFlags = 0);
    alloc_virtual void FreeInternal(void *p);
    alloc_virtual void *MallocAlignedInternal(
        size_t nAlignment, size_t nSize, size_t nAlignmentOffset, int32_t nAllocationFlags = 0);
    alloc_virtual void **MallocMultipleInternal(size_t nElementCount,
        size_t nSizeCount,
        const size_t nElementSizes[],
        void *pResultArray[],
        int32_t nAllocationFlags = 0);
    alloc_virtual Chunk *ExtendCoreInternal(size_t nMinSize);
    alloc_virtual Chunk *AddCoreInternal(size_t nMinSize);
    alloc_virtual bool FreeCore(CoreBlock *pCoreBlock, bool bInShutdown);
    alloc_virtual void *MMapMalloc(size_t nSize);
    alloc_virtual void *MMapAllocInternal(size_t nSize, size_t *pSizeAllocated = nullptr);
    alloc_virtual void MMapFreeInternal(void *pAddress, size_t nSize);
    alloc_virtual void ClearFastBins();
    alloc_virtual void LinkCoreBlock(CoreBlock *pCoreBlock, CoreBlock *pNext);
    alloc_virtual void UnlinkCoreBlock(CoreBlock *pCoreBlock);
    alloc_virtual CoreBlock *FindCoreBlockForAddress(const void *pAddress);
    alloc_virtual int32_t CheckChunk(const Chunk *pChunk);
    alloc_virtual int32_t CheckFreeChunk(const Chunk *pChunk);
    alloc_virtual int32_t CheckUsedChunk(const Chunk *pChunk);
    alloc_virtual int32_t CheckRemallocedChunk(const Chunk *pChunk, size_t nRequestedChunkSize);
    alloc_virtual int32_t CheckMallocedChunk(
        const Chunk *pChunk, size_t nRequestedChunkSize, bool bNewlyMalloced, bool bAllowPrevNotInUse);
    alloc_virtual int32_t CheckMMappedChunk(const Chunk *pChunk);
    alloc_virtual int32_t CheckState(HeapValidationLevel heapValidationLevel);
    alloc_virtual size_t DescribeChunk(const Chunk *pChunk, char *pBuffer, size_t nBufferLength, bool bAppendLineEnd);
    alloc_virtual DataPreviewType GetDataPreview(
        const void *pData, size_t nDataSize, char *pBuffer, wchar_t *pBufferW, size_t nBufferLength);

    void SetNewTopChunk(Chunk *pChunk, bool bFreePreviousTopChunk);
    Chunk *GetBin(int32_t nIndex) const
    {
        return reinterpret_cast<Chunk *>(
            uintptr_t(uintptr_t(mpBinArray + (nIndex * kBinPointerCount)) - offsetof(Chunk, mpPrevChunk)));
    }
    Chunk *GetInitialTopChunk() const { return GetBin(1); }
    Chunk *GetUnsortedBin() const { return GetBin(1); }
    void SetMaxFastBinDataSize(size_t nSize)
    {
        const size_t existing = (mnMaxFastBinChunkSize & kFlagFastBinChunksExist);

        if (nSize) {
            if (nSize > kFastBinMaxDataSize) {
                nSize = kFastBinMaxDataSize;
            }

            mnMaxFastBinChunkSize = GetChunkSizeFromDataSize(nSize) | existing;
        } else {
            mnMaxFastBinChunkSize = existing;
        }
    }
    size_t GetFastBinChunksExist() const { return mnMaxFastBinChunkSize & kFlagFastBinChunksExist; }
    size_t GetMMapChunkSizeFromDataSize(size_t nDataSize)
    {
        const size_t kPageMask = 0xFFF;
        return size_t((kChunkInternalHeaderSize + nDataSize + kMinChunkSize + kMinAlignmentMask + kPageMask) & ~kPageMask);
    }

    static size_t GetChunkSizeFromDataSize(size_t nDataSize)
    {
        const size_t sum = size_t(nDataSize + (kDataPtrOffset - kSizeTypeSize) + kMinAlignmentMask);

        if (sum > kMinChunkSize) {
            return (sum & ~kMinAlignmentMask);
        }

        return kMinChunkSize;
    }
    static Chunk *GetMMapChunkFromMMapListChunk(const Chunk *pChunk)
    {
        return reinterpret_cast<Chunk *>(uintptr_t(pChunk) - pChunk->mnPriorSize);
    }
    static void *GetDataPtrFromChunkPtr(Chunk *pChunk) { return reinterpret_cast<char *>(pChunk) + kDataPtrOffset; }
    static void *AlignUp(const void *ptr, size_t nAlignment)
    {
        return reinterpret_cast<void *>((uintptr_t(ptr) + (nAlignment - 1)) & ~(nAlignment - 1));
    }
    static size_t AlignUp(size_t nValue, size_t nAlignment)
    {
        return size_t((nValue + (nAlignment - 1)) & ~(nAlignment - 1));
    }
    static size_t Misalignment(void *pAddress, size_t nAlignment) { return (uintptr_t(pAddress) & (nAlignment - 1)); }
    static bool IsAligned(void *pAddress, size_t nAlignment) { return Misalignment(pAddress, nAlignment) == 0; }
    static void SetChunkSize(Chunk *pChunk, size_t nSize) { pChunk->mnSize = nSize; }
    static void SetChunkSizePreserveFlags(Chunk *pChunk, size_t nSize)
    {
        pChunk->mnSize = ((pChunk->mnSize & kChunkFlagAll) | nSize);
    }
    static size_t GetChunkSize(const Chunk *pChunk) { return pChunk->mnSize & kChunkSizeMask; }
    static Chunk *GetChunkAtOffset(const Chunk *pChunk, ptrdiff_t nOffset)
    {
        return reinterpret_cast<Chunk *>(uintptr_t(pChunk) + nOffset);
    }
    static void LinkChunk(Chunk *pChunk, Chunk *pPrev, Chunk *pNext)
    {
        pChunk->mpPrevChunk = pPrev;
        pChunk->mpNextChunk = pNext;
        pPrev->mpNextChunk = pChunk;
        pNext->mpPrevChunk = pChunk;
    }
    static void UnlinkChunk(const Chunk *pChunk)
    {
        pChunk->mpPrevChunk->mpNextChunk = pChunk->mpNextChunk;
        pChunk->mpNextChunk->mpPrevChunk = pChunk->mpPrevChunk;
    }
    static void AssertionFailureFunctionDefault(const char *desc, void *ctx);
    static void TraceFunctionDefault(char const *desc, void *ctx);
    static void AddDoubleFencepost(Chunk *pChunk, size_t nPrevChunkFlags);
    static Chunk *MakeChunkFromCore(void *pCore, size_t nCoreSize, size_t nFlags);

protected:
    bool mbInitialized;
    size_t mnMaxFastBinChunkSize;
    Chunk *mpFastBinArray[kFastBinCount];
    void *mpBinArray[kBinCount * kBinPointerCount];
    uint32_t mBinBitmap[kBinBitmapWordCount];
    Chunk *mpTopChunk;
    Chunk *mpLastRemainderChunk;
    CoreBlock mHeadCoreBlock;
    void *mpHighFence;
    bool mbHighFenceInternallyDisabled;
    bool mbSystemAllocEnabled;
    int32_t mnCheckChunkReentrancyCount;
    uint8_t mcTraceFieldDelimiter;
    uint8_t mcTraceRecordDelimiter;
    HeapValidationLevel mAutoHeapValidationLevel;
    size_t mnAutoHeapValidationFrequency;
    size_t mnAutoHeapValidationEventCount;
    bool mbHeapValidationActive;
    int32_t mnMMapCount;
    size_t mnMMapMallocTotal;
    int32_t mnMMapMaxAllowed;
    size_t mnMMapThreshold;
    bool mbMMapTopDown;
    Chunk mHeadMMapChunk;
    HookFunction mpHookFunction;
    void *mpHookFunctionContext;
    MallocFailureFunction mpMallocFailureFunction;
    void *mpMallocFailureFunctionContext;
    uint32_t mnMaxMallocFailureCount;
    AssertionFailureInfoFunction mpAssertionFailureFunction;
    void *mpAssertionFailureFunctionContext;
    TraceFunction mpTraceFunction;
    void *mpTraceFunctionContext;
    size_t mnTrimThreshold;
    size_t mnTopPad;
    char *mpInitialTopChunk;
    size_t mnPageSize;
    size_t mnNewCoreSize;
    size_t mnCoreIncrementSize;
    bool mbTraceInternalMemory;
    void *mpMutex;
    uint32_t mMutexData[kMutexBufferSize];
    uint8_t mnFillFree;
    uint8_t mnFillDelayedFree;
    uint8_t mnFillNew;
    uint8_t mnFillGuard;
    uint8_t mnFillUnusedCore;
};
} // namespace Allocator
