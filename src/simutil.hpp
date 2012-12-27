#ifndef SIM_UTIL_HPP
#define SIM_UTIL_HPP

// assert
#include <cassert>
#include <cmath>
#include <iostream>

// Some convenient macros
#define PrintLine(str) std::cout << str << endl;

typedef unsigned short ushort;
typedef unsigned int uint;

/// A pair of (small) integers, x and y
struct int2
{
    ushort x, y;

    int2(int x = 0, int y = 0) : x(x), y(y) {}

    int Area() const;

    /// Convert to one-dimensional index
    int Get1DIndex(int width) const;

    /// Increments x by one, but wraps x at width, at which point, it resets x and increments y by one.
    void Inc(int width);

    /// Component-wise addition
    int2 operator+ (int2 rhs);

    /// Component-wise subtraction
    int2 operator- (int2 rhs);

    /// Component-wise multiplication
    int2 operator* (int2 rhs);
};


// include config, make it available to everyone
#include "SimConfig.hpp"

/// Represents an address in 32-bit address space
struct Address
{
    /// The raw integer representation of the address
    uint Raw;

    uint GetL1Index() const;
    uint GetL1Tag() const;

    uint GetL2Index() const;
    uint GetL2Tag() const;

    /// The offset of the word that this address is referring to
    uint GetWordOffset() const;

    Address(uint raw = 0);

    /// The block-local L2 chunk index, to which this address maps
    int GetL2ChunkIdx1() const;
};

#endif // SIM_UTIL_HPP
