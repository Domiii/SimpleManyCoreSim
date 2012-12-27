/// New custom function that we call during start-up
void MMU::InitMMU(Tile* tile)
{
    this->tile = tile;

    // Initialize Caches
    l2ChunkIdx = tile->coreBlock->ComputeL2ChunkID(tile->tileIdx);

    l1.InitCache(GlobalConfig.CacheL1Size);
    l2.InitCache(GlobalConfig.CacheL2Size, l2ChunkIdx * GlobalConfig.CacheL2Size);

    ResetMMU();
}


/// Reset MMU to initial state
void MMU::ResetMMU()
{
    // Clear caches & reset simulated time
    l1.Reset();
    l2.Reset();

    simTime = 0;

    // Clear outstanding requests
    lastRequestId = 0;
    requests.clear();
    requests.resize(MAX_OUTSTANDING_REQUESTS);
}


/// Loads a word into the on-tile Core
void MMU::LoadWord(const Address& addr)
{
    // Lookup in caches

    // L1 access time
    int totalDelay = GlobalConfig.CacheL1Delay;

    CacheLine* line;
    if (!l1.GetLine(addr, line))
    {
        // L1 miss
        int addrL2ChunkIdx = tile->coreBlock->ComputeL2ChunkID(addr);
        if (addrL2ChunkIdx != l2ChunkIdx)
        {
            // Get from off-tile L2
            FetchRemoteL2(tile->coreBlock->ComputeTileIndex(addrL2ChunkIdx), totalDelay, addr);
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


/// Fetch cacheline from the given off-tile L2 cache chunk. Note that this is only used by the local Core.
int MMU::FetchRemoteL2(int2 holderIdx, int totalDelay, const Address& addr)
{
    SendRequest(MessageTypeRequestL2, tile->tileIdx, holderIdx, addr, totalDelay);
}


/// Fetch word from on-tile L2
void MMU::FetchLocalL2(int2 requesterIdx, int totalDelay, const Address& addr)
{
    // Add hit penalty
    totalDelay += GlobalConfig.CacheL2Delay;

    CacheLine* line;
    if (!l2.GetLine(addr, line))
    {
        // L2 miss
        FetchFromMemory(requesterIdx, addr, totalDelay);
    }
    else
    {
        if (requesterIdx == tile->tileIdx)
        {
            // Local request -> Can be committed immediately
            CommitLoad(line, totalDelay, addr);
        }
        else
        {
            // Send value back to requester
            SendResponse(MessageTypeResponseCacheline, requesterIdx, line->words, totalDelay);
        }
    }
}


/// Fetch word from memory, when it is missing in this tile's L2
int MMU::FetchFromMemory(int2 requesterIdx, const Address& addr, int totalDelay)
{
    SendRequest(MessageTypeRequestMem, requesterIdx, int2(GlobalMemoryController::GMemIdx, GlobalMemoryController::GMemIdx), addr, totalDelay);
}


// ############################################# Handle incoming Messages #############################################

/// Called by router when a Cacheline has been sent to this tile
void MMU::OnCachelineReceived(int requestId, int totalDelay, WORD* words)
{
    OutstandingRequest& request = requests[requestId];
    assert(request.pending);

    int addrChunkIdx = request.addr.GetL2Index() / GlobalConfig.L2CacheSize;
    if (addrChunkIdx == l2ChunkIdx)
    {
        // Address maps to this tile's L2 chunk, so have to update it
        l2.Update(request.addr, words);
    }


    // TODO: Handle coalescing (i.e. multiple requesters request the same line in a single packets)

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
void MMU::CommitLoad(CacheLine* line, int totalDelay, const Address& addr)
{
    // Add time spent on this request to total sim time
    simTime += totalDelay;
    tile->cpu->CommitLoad(line->GetWord(addr));
}



// ############################################# Handle Request buffer & Send Messages #############################################

/// Get a free request buffer entry
OutstandingRequest& MMU::GetFreeRequest(int& requestId)
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
void MMU::SendRequest(MessageType type, int2 requesterIdx, int2 receiver, Address addr, int totalDelay)
{
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
void MMU::SendResponse(MessageType type, int2 receiver, WORD* words, int totalDelay)
{
    // Create Message object
    Message msg;
    msg.type = type;
    msg.sender = tile->tileIdx;
    msg.receiver = receiver;
    msg.requestId = requestId;
    msg.totalDelay = totalDelay;
    msg.addr = addr;
    memcpy(msg.cacheLine, words, sizeof(WORD) * GlobalConfig::CacheLineSize);

    // Send the message
    tile->router.EnqueueMessage(msg);
}
