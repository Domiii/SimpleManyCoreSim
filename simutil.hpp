/**
 * This file is included by all classes
 */

// include guard
#ifndef _SMUTIL_HPP
#define _SMUTIL_HPP

typedef unsigned short ushort;

// assert
#include <cassert>
#include <cmath>
#include <iostream>

// include config, make it available to everyone
#include "SimConfig.hpp"


// Some convenient macros
#define PrintLine(str) std::cout << str << endl;

// TODO: Can we use alignment magic to squeeze this into a single int?

/// Represents an address in 32-bit address space
struct Address
{
    union
    {
        L1Index;
        L1Tag;
    };
    
    union
    {
        L2Index;
        L2Tag;
    };
    
    /// The offset of the word that this address is referring to
    WordOffset;
    
    Address(int addr)
    {
        // TODO: Read index, tag and offset from address
    }
};


/// A pair of (small) integers, x and y
struct int2
{
    ushort x, y;

    int2(int x = 0, int y = 0) : x(x), y(y) {}

    int Area() const { return (int)x * (int)y; }
     
    /// Convert to one-dimensional index
    int Get1DIndex(int width) const { return y * width + x; }

    /// Increments x by one, but wraps x at width, at which point, it resets x and increments y by one.
    void Inc(int width)
    {
        ++x;

        if (x == width)
        {
            x = 0;
            ++y;
        }
    }

    /// Component-wise multiplication
    int2 operator* (int2 rhs)
    {
        return int2(x * rhs.x, y * rhs.y);
    }
}


#endif