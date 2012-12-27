#ifndef CPU_HPP
#define CPU_HPP

#include "simutil.hpp"
#include "Tile.hpp"
#include "Thread.hpp"
#include "MMU.h"

/// This is an ARMulator class
struct CPU
{
    /// The tile to which this CPU belongs
    Tile* tile;

    /// The currently running thread
    Thread* currentThread;
    
    /// Whether this Core is currently waiting for off-chip data
    bool isLoadingData;

    /// The total amount of instructions
    long long simInstructionCount, simLoadInstructionCount;

    // ...
	MMU mmu;

    CPU(Tile* tile);


    /// This Core starts running the given Thread
    void StartThread(Thread* thread)


    /// Dispatches and, if possible, executes one simulated instruction. Returns false, if there are no more instructions to execute (i.e. EOF reached).
    bool DispatchNext();


    /// Forwards a load instruction to the MMU
    void DispatchLoad(int addrWord)

    
    /// Called by MMU when it received data that this CPU is waiting for
    void CommitLoad(WORD data);
};


/// ARMulator calls Core "CPU", so we rename it accordingly
typedef CPU Core;

#endif // CPU_HPP
