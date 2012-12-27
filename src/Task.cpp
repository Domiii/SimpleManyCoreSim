#include "Task.hpp"

/// Whether this Task still has unscheduled TaskBlocks
bool Task::HasMoreBlocks() const
{
    return nextBlockIdx.Area() <= taskSize.Area();
}

/// Whether all TaskBlocks of this Task have already finished running
bool Task::IsFinished()
{
    return finishedCount == taskSize.Area();
}

/// Creates the next TaskBlock in this task
TaskBlock Task::CreateNextTaskBlock(CoreBlock& coreBlock)
{
    assert(HasMoreBlocks());
    TaskBlock nextBlock;
    nextBlock.InitTaskBlock(this, lastBlockIdx, coreBlock);

    nextBlockIdx.Inc(blockSize.x);

    return nextBlock;
}
