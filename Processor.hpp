

#include <vector>


#include "simutil.hpp"
#include "CoreBlock.hpp"
#include "Task.hpp"
#include "GlobalMemoryController.hpp"

/// The Processor contains and does it all
struct Processor
{
    /// The amount and size of blocks on this processor
    int2 coreGridSize, coreBlockSize;

    /// All blocks available on this processor
    CoreBlock* blocks;
    
    /// The Global Memory Controller supplies data that is not in any cache
    GlobalMemoryController gMemController;

    /// The current batch of tasks to run
    std::vector<Task> tasks;

    /// Whether the current batch of tasks has finished running
    bool batchFinished;

    /// The number of the current batch
    int batchNum;
    
    Processor()
    {
        coreGridSize = GlobalConfig.CoreGridSize();
        coreBlockSize = GlobalConfig.CoreBlockSize();

        batchNum = 0;
        blocks = new CoreBlock[coreGridSize.Area()];
        batchFinished = true;
        gMemController.InitGMemController(this);
        
        for (int i = 0; i < coreGridSize.Area(); ++i)
        {
            // TODO: Pass initial settings to block
            blocks[i].InitCoreBlock(this, otherArgs);
        }
    }

    
    ~Processor()
    {
        delete [] blocks;
    }
    

    // ################################################## Simulation ##################################################

    /// The entry point of any simulation: Run a batch of tasks
    void StartBatch(const std::vector<Task>& tasks)
    {
        assert(tasks.size() > 0);

        ++batchNum;
        batchFinished = false;

        PrintLine("Batch starting: " << batchNum);

        this->tasks = tasks;

        // Assign initial task blocks in round-robin fashion
        int t = 0;
        for (int i = 0; i < gridSize.Area(); ++i)
        {
            Task* task = GetNextTask(task);
            if (!task) break;

            ScheduleTaskBlock(*task, blocks[i]);
        }

        // Start running the simulation
        while (!batchFinished)
        {
            SimSteps();
        }
        
        // Done!
        EvaluateStats();
    }


    /// Take a few simulation steps
    void SimSteps()
    {
        // Simulate one GMem step
        gMemController.DispatchNext();

        // Simulate one step for each tile
        // Run on all available cores, using OpenMP
        #pragma omp parallel for private(i, tile, s)
        for (int bi = 0; bi < coreGridSize.Area(); ++bi)
        {
            // One iteration per block
            CoreBlock& block = blocks[bi];
            
            for (int i = 0; i < coreBlockSize.Area(); i)
            {
                // One iteration per tile
                Tile& tile = block.tiles[i];
                //for (int s = 0; s < !tile.cpu->IsStalling() && !tile.; ++s)
                {
                    tile.cpu->DispatchNext();
                    tile.router->DispatchNext();
                }
            }
        }
    }


    /// Evaluates and/or writes stats to file
    void EvaluateStats()
    {
        // TODO: Evaluate and/or write stats to file (for visualization)
    }
    
    // ################################################## Task management ##################################################

    /// Gets the next task that still has unscheduled blocks or null, if there is none
    Task* GetNextTask()
    {
        int t = 0;
        for (std::vector<Task>::iterator it = tasks.begin(); it != tasks.end(); ++it)
        {
            if (it->HasMoreBlocks())
            {
                return *it;
            }
        }
        return nullptr;
    }
    

    /// Put the next TaskBlock of the given Task on the given CoreBlock
    void ScheduleTaskBlock(Task& task, CoreBlock& coreBlock)
    {
        // Create TaskBlock
        TaskBlock taskBlock = task.CreateNextTaskBlock(coreBlock);
        

        PrintLine("TaskBlock starting: " << task.name << " (" << taskBlock.taskBlockIdx.x << ", " << taskBlock.taskBlockIdx.y << ") on Core Block (" <<
            coreBlock.blockIdx.x << ", " << coreBlock.blockIdx.x << ")");


        int tileCount = coreBlock.blockIdx.Area();
        int threadCount = task.blockSize.Area();
        
        // Put all initial threads on tiles
        for (int i = 0; i < min(tileCount, threadCount); ++i)
        {
            coreBlock.ScheduleThread(taskBlock, *tile);
        }
    }


    /// Called by a CoreBlock when it finished executing the given TaskBlock
    void OnTaskBlockFinished(TaskBlock& taskBlock)
    {
        PrintLine("TaskBLock finished: " << task.name << " (" << taskBlock.taskBlockIdx.x << ", " << taskBlock.taskBlockIdx.y << ")");

        if (taskBlock.task->HasMoreBlocks())
        {
            // Schedule next TaskBlock on the now free core block
            ScheduleTaskBlock(*taskBlock.task, *taskBlock.coreBlock);
        }
        else
        {
            Task* task = GetNextTask(task);
            if (task)
            {
                // Schedule next TaskBlock of a different Task on the now free core block
                ScheduleTaskBlock(newTask, *taskBlock.coreBlock);
            }

            if (taskBlock.task->IsFinished())
            {
                // All blocks of this task have already been scheduled
                OnTaskFinished(*taskBlock.task);
            }
        }
    }


    /// Called when the given Task has completed execution of all it's threads
    void OnTaskFinished(Task& task)
    {
        PrintLine("Task finished: " << task.name);

        if (!GetNextTask())
        {
            // The batch has finished
            OnBatchFinished();
        }
    }


    /// Called when all tasks have been worked through
    void OnBatchFinished()
    {
        PrintLine("Batch finished: " << batchNum);
        batchFinished = true;
    }
};