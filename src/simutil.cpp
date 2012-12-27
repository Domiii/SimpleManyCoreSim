#include "simutil.hpp"

int int2::Area() const
{
    return (int)x * (int)y;
}

/// Convert to one-dimensional index
int int2::Get1DIndex(int width) const
{
    return y * width + x;
}

/// Increments x by one, but wraps x at width, at which point, it resets x and increments y by one.
void int2::Inc(int width)
{
    ++x;

    if (x == width)
    {
        x = 0;
        ++y;
    }
}

/// Component-wise addition
int2 int2::operator+ (int2 rhs)
{
    return int2(x + rhs.x, y + rhs.y);
}

/// Component-wise subtraction
int2 int2::operator-(int2 rhs)
{
    return int2(x - rhs.x, y - rhs.y);
}

/// Component-wise multiplication
int2 int2::operator* (int2 rhs)
{
    return int2(x * rhs.x, y * rhs.y);
}

uint Address::GetL1Index() const
{
    return GlobalConfig.CacheL1Size TODO;
}

uint Address::GetL1Tag() const
{
    return TODO;
}

uint Address::GetL2Index() const
{
    return TODO;
}

uint Address::GetL2Tag() const
{
    return TODO;
}

/// The offset of the word that this address is referring to
uint Address::GetWordOffset() const
{
    return Raw & GlobalConfig.CacheLineBits;
}

Address::Address(uint raw = 0)
{
    // TODO: Read index, tag and offset from address
}

/// The block-local L2 chunk index, to which this address maps
int Address::GetL2ChunkIdx1() const
{
    return (GetL2Index() / GlobalConfig.CacheL2Size) % GlobalConfig.CoreBlockSize();
}
