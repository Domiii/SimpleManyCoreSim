#include "Address.hpp"

#include "SimConfig.hpp"

#include <math.h>

using namespace smcsim;

uint32_t Address::GetL1Index() const
{
    return (GlobalConfig.CacheL1Size-1 &
            (raw >> GlobalConfig.CacheLineBits)); // TODO: Check?
}

uint32_t Address::GetL1Tag() const
{
    int shift = GlobalConfig.CacheLineBits +
                int(log(GlobalConfig.CacheL1Size)/log(2));
    return raw>>shift; // TODO: Check?
}

uint32_t Address::GetL2Index() const
{
    return (GlobalConfig.CacheL2Size-1 &
            (raw >> GlobalConfig.CacheLineBits)); // TODO: Check?
}

uint32_t Address::GetL2Tag() const
{
    int shift = GlobalConfig.CacheLineBits +
                int(log(GlobalConfig.CacheL2Size)/log(2));
    return raw>>shift; // TODO: Check?
}

/// The offset of the word that this address is referring to
uint32_t Address::GetWordOffset() const
{
    return raw & GlobalConfig.CacheLineBits;
}

/// The block-local L2 chunk index, to which this address maps
int Address::GetL2ChunkIdx1() const
{
    return (GetL2Index() / GlobalConfig.CacheL2Size) %
                                GlobalConfig.CoreBlockSize().Area();
}
