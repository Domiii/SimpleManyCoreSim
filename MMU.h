/**
 * This file already exists in ARMulator. 
 * We have to add modifications in order to simulate our new memory-access system.
 */

/// We can have a lot of concurrently outstanding requests
#define MAX_OUTSTANDING_REQUESTS 8192

#include <vector>


struct OutstandingRequest
{
    Address addr;
    bool pending;
    int totalDelay;
    int2 requesterIdx;
};

/// A per-tile MMU
struct MMU
{
    /// The tile to which this local MMU belongs
    Tile* tile;
    
    /// L1 cache and one chunk of a distributed and shared L2 cache
    Cache l1, l2;

    /// The index of this tile's L2 chunk
    int l2ChunkIndex;

    /// The last used request buffer entry index
    int lastRequestId;
    
    /**
     * Request buffer: Currently outstanding requests, going to an off-tile destination. 
     * This includes requests from the on-tile core, as well as requests from off-tile cores (i.e. off-tile L2 cache misses that happen on this tile).
     */
    std::vector<OutstandingRequest> requests;


    
    // ######### Simulation stuff #########

    /// Whether this MMU is currently causing a stall to it's processor
    bool isStalling;

    /// Total simulated time spent on memory requests
    long long simTime;

    // ...
    



    /// New custom function that we call during start-up
    void InitMMU(Tile* tile)
    {
        this->tile = tile;

        // Initialize Caches
        // TODO: Compute chunk index
        l2ChunkIndex = "index of tile within core block";
        l1.InitCache(GlobalConfig.L1CacheSize);
        l2.InitCache(GlobalConfig.L2CacheSize, chunkIndex * GlobalConfig.L2CacheSize);
        
        ResetMMU();
    }

    /// Reset MMU to initial state
    void ResetMMU()
    {
        // Clear caches & reset simulated time
        l1.Reset();
        l2.Reset();
        
        simTime = 0;
        isStalling = false;
        
        // Clear outstanding requests
        lastRequestId = 0;
        requests.clear();
        requests.resize(MAX_OUTSTANDING_REQUESTS);
    }

    
    /// Loads a word into the on-tile Core
    void LoadWord(const Address& addr)
    {
        // Lookup in caches
        
        // L1 access time
        int totalDelay = GlobalConfig.CacheL1Delay;

        CacheLine* line;
        if (!l1.GetLine(addr, line))
        {
            // L1 miss
            int2 l2TileIdx = GetL2Idx(addr);
            if (l2TileIdx != tile.tileIdx)
            {
                // Get from off-tile L2
                FetchRemoteL2(l2TileIdx, totalDelay, addr);
            }
            else
            {
                // Get from local L2
                FetchLocalL2(tile->tileIdx, totalDelay, addr);
            }
        }
        else
        {
            CommitLoad(line, totalDelay, addr);
        }
    }

    
    /// Do Z-order lookup (simple conversion from cache entry index to tile)
    int GetL2Idx(const Address& addr)
    {
        // TODO: Domi
    }
    
    /// Fetch cacheline from the given off-tile L2 cache chunk. Note that this is only used by the local Core.
    int FetchRemoteL2(int2 holderIdxr, int totalDelay, const Address& add)
    {
        SendRequest(MessageTypeRequestL2, tile->tileIdx, holderIdx, addr, totalDelay);
    }

    /// Fetch word from on-tile L2
    void FetchLocalL2(int2 requesterIdx, int totalDelay, const Address& addr)
    {
        // Add hit penalty
        totalDelay += GlobalConfig.CacheL2Delay;

        CacheLine* line;
        if (!l2.GetLine(addr, line))
        {
            // L2 miss
            FetchFromMemory(requesterIdx, addr, totalDelay);
        }
        
        if (requesterIdx == tile->tileIdx)
        {
            // Local request -> Can be committed immediately
            CommitLoad(line, totalDelay, addr);
        }
        else
        {
            // Send value back to requester
            SendResponse(MessageTypeResponseCacheline, requesterIdx, words, totalDelay);
        }
    }

    
    /// Fetch word from memory, when it is missing in this tile's L2
    int FetchFromMemory(int2 requesterIdx, const Address& addr, int totalDelay)
    {
        SendRequest(MessageTypeRequestMem, requesterIdx, int2(GlobalMemoryController::GMemIdx, GlobalMemoryController::GMemIdx), addr, totalDelay);
    }


    // ############################################# Handle incoming Messages #############################################
    
    /// Called by router when a Cacheline has been sent to this tile
    void OnCachelineReceived(int requestId, int totalDelay, int* words)
    {
        OutstandingRequest& request = requests[requestId];
        assert(request.pending);
        
        int addrChunkIdx = reqest.addr.L2Index / GlobalConfig.L2CacheSize;
        if (addrChunkIdx == l2ChunkIndex)
        {
            // Address maps to this tile's L2 chunk, so have to update it
            l2.Update(request.addr, words);
        }
        

        // TODO: Handle the case where multiple requesters requested the same line without sending multiple packets
        
        if (request.requesterIdx == tile->tileIdx)
        {
            // Cacheline was requested by this guy
            
            // -> Also goes into L1
            CacheLine& line = l1.Update(request.addr, words);

            // And we can finally restart the CPU
            CommitLoad(line, request.addr, totalDelay);
        }
        else
        {
            // CacheLine is a response to an off-tile request -> Send it to requester
            SendResponse(MessageTypeResponseCacheline, request.requesterIdx, words, totalDelay);
        }

        // Request buffer entry not in use anymore
        request.pending = false;
    }


    /// This function is always called when the MMU retrieved a word
    void CommitLoad(CacheLine* line, int totalDelay, const Address& addr)
    {
        // Add time spent on this request to total sim time
        simTime += totalDelay;
        tile->cpu->CommitLoad(line->GetWord(addr));
    }
    


    // ############################################# Handle Request buffer & Send Messages #############################################

    /// Get a free request buffer entry
    OutstandingRequest& GetFreeRequest(int& requestId)
    {
        for (int i = lastRequestId, count = 0; count < MAX_OUTSTANDING_REQUESTS; i = (i+1)%MAX_OUTSTANDING_REQUESTS)
        {
            if (!requests[i].pending)
            {
                requestId = i;
                return requests[i];
            }
        }

        PrintLine("More than MAX_OUTSTANDING_REQUESTS in request queue");
        assert(false);
    }

    /// Creates and sends a new Request Message
    void SendRequest(MessageType type, int2 requesterIdx, int2 receiver, Address addr, int totalDelay)
    {
        if (requesterIdx == tile->tileIdx)
        {
            // When we send out a request, we have to wait for a while for stuff to come back
            isStalling = true;
        }

        // Add request to request buffer
        int requestId;
        OutstandingRequest& request = GetFreeRequest(requestId);
        request.pending = true;
        request.requesterIdx = requesterIdx;
        request.addr = addr;
        request.totalDelay = totalDelay;

        // Create Message object
        Message msg;
        msg.type = type;
        msg.sender = tile->tileIdx;
        msg.receiver = receiver;
        msg.requestId = requestId;
        msg.totalDelay = totalDelay;
        msg.addr = addr;

        // Send the message
        tile->router.EnqueueMessage(msg);
    }
    

    /// Creates and sends a new Response Message
    void SendResponse(MessageType type, int2 receiver, int* words, int totalDelay)
    {
        // Create Message object
        Message msg;
        msg.type = type;
        msg.sender = tile->tileIdx;
        msg.receiver = receiver;
        msg.requestId = requestId;
        msg.totalDelay = totalDelay;
        msg.addr = addr;
        memcpy(msg.cacheLine, words, sizeof(int) * GlobalConfig::CacheLineSize);

        // Send the message
        tile->router.EnqueueMessage(msg);
    }
};