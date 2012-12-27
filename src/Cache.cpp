#include "Cache.hpp"

CacheLine::CacheLine()
{
    valid = false;
}

/// Get the word at the given address (given the address maps to this line)
WORD CacheLine::GetWord(const Address& addr)
{
    return words[addr.WordOffset];
}


void Cache::InitCache(int size, int offset = 0)
{
    this->size = size;
    this->offset = offset;

    lines.resize(size);
}


/// The given CacheLine is updated
CacheLine& Cache::UpdateLine(const Address& addr, WORD* words)
{
    CacheLine& line = lines[addr.Index - offset];
    
    // TODO: Copy words into cacheline and set valid and tag

    return line;
}


/// Usually the cache is only reset when the processor is reset (i.e. when starting a new batch)
void Cache::Reset()
{
    lines.clear();
    lines.resize(size);

    simAccessCount = simMissCount = 0;
}


/// Get the entry of the given address
bool Cache::GetEntry(const Address& addr, CacheLine* line)
{
    ++simAccessCount;

    line = &lines[addr.Index - offset];
    if (line->valid && line->tag == addr.tag)
    {
        // cache hit
        return true;
    }

    // cache miss
    ++simMissCount;
    return false;
}
