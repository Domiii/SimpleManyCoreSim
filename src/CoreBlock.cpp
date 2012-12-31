#include "CoreBlock.hpp"

#include "Address.hpp"
#include "CPU.hpp"
#include "Debug.hpp"
#include "Dimension.hpp"
#include "Processor.hpp"
#include "SimConfig.hpp"
#include "TaskBlock.hpp"
#include "Thread.hpp"
#include "Tile.hpp"

CoreBlock::CoreBlock()
{
    tiles = new Tile[GlobalConfig.CoreBlockSize().Area()];
}

CoreBlock::~CoreBlock()
{
    delete [] tiles;
}


/// Initializes this CoreBlock
void CoreBlock::InitCoreBlock()
{
    /*
    foreach(tile in tiles)
    {
        // TODO: Pass init parameters to each tile
        tile.InitTile(...);
    }
    */
}


/// The index of the first tile within this core block
Dim2 CoreBlock::ComputeCoreBlockOrigin() const
{
    return blockIdx * processor->coreBlockSize;
}


/// Computes the global 2D index of the given block-local 1D index
Dim2 CoreBlock::ComputeTileIndex(int inCoreID) const
{
    return ComputeCoreBlockOrigin() + GlobalConfig.ComputeInCoreBlockIdx2(inCoreID);
}

/// Computes the L2 chunk index of the given memory address
int CoreBlock::ComputeL2ChunkID(const Address& addr) const
{
    return ComputeL2ChunkID(GlobalConfig.ComputeInCoreBlockIdx2(addr.GetL2ChunkIdx1()));
}


/// Computes the L2 chunk index of the given global tile index
int CoreBlock::ComputeL2TileChunkID(const Dim2& globalIdx) const
{
    Dim2 blockOrigin = ComputeCoreBlockOrigin();
    return ComputeL2ChunkID(globalIdx - blockOrigin);
}


/// Computes the L2 chunk ID of the given block-local 2D index
int CoreBlock::ComputeL2ChunkID(const Dim2& inCoreBlockIdx2) const
{
    // Converts to 1D index, using some order
    return GlobalConfig.ComputeInCoreBlockIdx1(inCoreBlockIdx2);
}


/// Put the next thread of the given TaskBlock on the given tile
void CoreBlock::ScheduleThread(TaskBlock& taskBlock, Tile& tile)
{
    Thread nextThread = taskBlock.CreateNextThread(tile);

    if (nextThread.threadIdx.Area() % 100)
    {
        PrintLine(
            "Thread starting: " << runningTaskBlock->task->name <<
            " (" << nextThread.threadIdx.x << ", "
                 << nextThread.threadIdx.y << ") " <<
            "on Tile (" << tile.tileIdx.x << ", "
                        << tile.tileIdx.y << ")");
    }

    tile.core->StartThread(&nextThread);
}


/// Is called by Core when it reached the end of it's current instruction stream
void CoreBlock::OnThreadFinished(Thread& thread)
{
    if (thread.taskBlock->HasMoreThreads())
    {
        // Schedule next thread
        ScheduleThread(*thread.taskBlock, *thread.tile);
    }
    else if (thread.taskBlock->IsFinished())
    {
        // All blocks of this task have already been scheduled
        processor->OnTaskBlockFinished(*thread.taskBlock);
    }
    else
    {
        // Do nothing: The tile is now idle
        PrintLine("Tile idle: " << " (" << thread.tile->tileIdx.x << ", " << thread.tile->tileIdx.y << ")");
    }
}
