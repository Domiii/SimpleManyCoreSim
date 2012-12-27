#ifndef TASK_BLOCK_HPP
#define TASK_BLOCK_HPP

struct Tile;

/**
 * Every Task is logically partitioned into TaskBlocks. 
 * One TaskBlock has a fixed size of threads that can be scheduled on an assigned CoreBlock.
 */
struct TaskBlock
{
    /// The task to which this block belongs
    Task* task;
    int2 taskBlockIdx;
    CoreBlock* assignedBlock;

    /// The id of the first thread that has not been scheduled yet
    int2 nextThreadId;

    TaskBlock();

    /// Initializes this TaskBlock
    void InitTaskBlock();

    /// Instruments the Task code for this block (i.e. insert block-id, thread-id etc into special placeholders within the code)
    Code GetInjectedCode(int2 threadId);

    /// Whether this TaskBlock still has unscheduled threads
    bool HasMoreThreads() const;

    /// Whether all TaskBlocks of this Task have already finished running
    bool IsFinished();

    /// Creates the next Thread from this TaskBlock to run on the given tile
    Thread CreateNextThread(Tile& tile);
};

#endif // TASK_BLOCK_HPP
