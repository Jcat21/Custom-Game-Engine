#pragma once

void *AlignendMalloc(size_t size, size_t align);
void AlignedFree(void *inMem);
bool AlignedMalloc_UnitTest();

template <typename T, uint32_t numBlocks>
class PoolAllocator
{
public:
    PoolAllocator()
    {
        for (uint32_t i = 0; i < numBlocks - 1; ++i)
        {
            mArray[i].mNext = &mArray[i + 1];
        }
        mArray[numBlocks - 1].mNext = nullptr;
        mFirstFree = &mArray[0];
    }

    T* Allocate()
    {
        if (nullptr == mFirstFree)
        {
            return nullptr;
        }
        PoolBlock* pBlock = mFirstFree;
        mFirstFree = pBlock->mNext;
        return &pBlock->mData;
    }

    void Free(T* pData)
    {
        PoolBlock* pBlock = static_cast<PoolBlock*>(pData);
        pBlock->mNext = mFirstFree;
        mFirstFree = pBlock;
    }

private:
    struct PoolBlock
    {
        T mData;
        PoolBlock* mNext;
    };
    PoolBlock mArray[numBlocks];
    PoolBlock* mFirstFree;
};
