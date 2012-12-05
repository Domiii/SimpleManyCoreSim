/**
 * All tweakable parameters of the system
 */

#define _VERBOSE

 struct SimConfig
 {
    // ################################ Tile Grid ################################

    /// Length of the grid of core blocks
    int CoreGridLen;
    
    /// Total amount of core blocks on the processor
    int CoreGridSize() { return CoreGridLen * CoreGridLen; }

    

    // ################################ Core Blocks ################################

    /// Side length of a core block. If CoreBlockLen == 1, then we have private L2s!
    int CoreBlockLen;
    
    /// Total amount of cores in a core block
    int CoreBlockSize() { return CoreBlockLen * CoreBlockLen; }


    
    // ################################ Caches ################################
 
    /// 64 bytes per cache line (= 16 x 4 byte words)
    static const int CacheLineSize = 16;

    /// L1 size & access time
    int CacheL1Size, CacheL1Delay;

    /// L2 size & access time
    int CacheL1Size, CacheL2Delay;
    
    /// Main memory access time
    int CacheMissDelay;

    
    // ################################ Networking ################################

    /// Delay when a router processes a packet
    int DispatchDelay;

    /// Delay when sending a packet from one Router to a neighbor Router
    int Route1Delay;

    /// 
    int MemDelay;
 }
 GlobalConfig;