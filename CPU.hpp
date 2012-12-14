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

    CPU(Tile* tile)
    {
        this->tile = tile;
        
        mmu.InitMMU(thread->tile);
    }


    /// This Core starts running the given Thread
    void StartThread(Thread* thread)
    {
        currentThread = thread;
        
        isLoadingData = false;
        simInstructionCount = simLoadInstructionCount = 0;

        mmu.ResetMMU();
        
        // TODO: Copy thread.code into mmu.code
    }


    /// Dispatches and, if possible, executes one simulated instruction. Returns false, if there are no more instructions to execute (i.e. EOF reached).
    bool DispatchNext()
    {
        if (isLoadingData) return true;
        
        ++simInstructionCount;

        // TODO: Only run/start a single instruction

        // TODO: Call DispatchLoad when encountering a load instruction

        // TODO: If the current instruction was the last instruction (or EOF), call tile->coreBlock->OnThreadFinished(*currentThread);
    }


    /// Forwards a load instruction to the MMU
    void DispatchLoad(int addrWord)
    {
        ++simLoadInstructionCount;
        Address addr(addrWord);
        mmu.LoadWord(addr);
        isLoadingData = true;
    }

    
    /// Called by MMU when it received data that this CPU is waiting for
    void CommitLoad(WORD data)
    {
        assert(isLoadingData);

        // TODO: Separate LOAD instruction into two parts:
        //      1. The instruction handler calls MMU.LoadWord(address)
        //      2. This function is called by MMU upon request completion (might be immediate or might take a while)
        //          -> Execute the rest of the load instruction here 
        
        // TODO: Figure out which part of the requested word is needed (which byte, which half-word, or the entire word?) 
        //      Possibly by just storing the requested length in a variable before in DispatchLoad
        
        isLoadingData = false;
    }

    // ...
};


/// ARMulator calls Core "CPU", so we rename it accordingly
typedef CPU Core;