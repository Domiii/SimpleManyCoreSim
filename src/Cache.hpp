#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>

/// A traditional cache line
struct CacheLine
{
    bool valid;
    int tag;
    WORD words[SimConfig::CacheLineWordSize];

    CacheLine();

    /// Get the word at the given address (given the address maps to this line)
    WORD GetWord(const Address& addr);
};


/// A traditional cache or cache chunk
struct Cache
{
    // TODO: Add associativity (?)

    /// All lines
    std::vector<CacheLine> lines;

    /// Amount of lines
    int size;

    /// Address offset of this cache chunk. Must be subtracted from actual entry index. Default = 0
    int offset;


    /// Amount of all accesses and of misses in this cache (chunk)
    long long simAccessCount, simMissCount;



    void InitCache(int size, int offset = 0);


    /// The given CacheLine is updated
    CacheLine& UpdateLine(const Address& addr, WORD* words);


    /// Usually the cache is only reset when the processor is reset (i.e. when starting a new batch)
    void Reset();


    /// Get the entry of the given address
    bool GetEntry(const Address& addr, CacheLine* line);
};

#endif // CACHE_HPP
