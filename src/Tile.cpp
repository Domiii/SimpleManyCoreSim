#include "Tile.hpp"

void Tile::InitTile(const Index& tileIdx, CoreBlock* coreBlock)
{
    // TODO: Init index, Core etc.

    core = new Core(params);
    mmu = core->mmu;
}


/// Whether this tile is at the core's x = 0, y = 0, x = w-1 or y = h-1
bool Tile::IsBoundaryTile()
{
    int2 blockSize = coreBlock->processor->coreBlockSize;
    int2 coreOrigin = coreBlock->ComputeCoreBlockOrigin();
    return tileIdx.x == coreOrigin.x && tileIdx.y == coreOrigin.y &&
        tileIdx.x == coreOrigin.x + blockSize.x - 1 && tileIdx.y == coreOrigin.y + blockSize.y - 1;
}
