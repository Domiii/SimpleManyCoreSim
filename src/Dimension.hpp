#ifndef DIMENSION_HPP
#define DIMENSION_HPP

#include <iostream>
#include <sstream>

#include <stdlib.h>

namespace smcsim {

class Dim1
{
public:
    int x;

public:
    Dim1(): x(0)
    { }

    explicit Dim1(int x_): x(x_)
    { }

    Dim1 operator+(const Dim1& rhs) const
    {
        return Dim1(x + rhs.x);
    }

    Dim1 operator-(const Dim1& rhs) const
    {
        return Dim1(x - rhs.x);
    }

    Dim1 operator*(const Dim1& rhs) const
    {
        return Dim1(x * rhs.x);
    }

    bool operator==(const Dim1& rhs) const
    {
        return (x == rhs.x);
    }
};

inline std::ostream &operator<<(std::ostream& os, const Dim1& rhs)
{
    std::stringstream ss;
    ss << "(" << rhs.x << ")";
    os << ss.str();
    return os;
}

class Dim2
{
public:
    int y;
    int x;

public:
    Dim2(): y(0), x(0)
    { }

    Dim2(int y_, int x_): y(y_), x(x_)
    { }

    int Product() const
    {
        return y * x;
    }

    int Area() const
    {
        return Product();
    }

    void Wrap(int width)
    {
        y += x / width;
        x %= width;
    }

    /// Increments x by one, but wraps x at width, at which point, it resets
    /// x and increments y by one.
    void Inc(int width)
    {
        ++x;
        Wrap(width);
    }

    /// Component-wise addition
    Dim2 operator+(const Dim2& rhs) const
    {
        return Dim2(y + rhs.y, x + rhs.x);
    }

    /// Component-wise subtraction
    Dim2 operator-(const Dim2& rhs) const
    {
        return Dim2(y - rhs.y, x - rhs.x);
    }

    /// Component-wise multiplication
    Dim2 operator*(const Dim2& rhs) const
    {
        return Dim2(y * rhs.y, x * rhs.x);
    }

    bool operator==(const Dim2& rhs) const
    {
        return (y == rhs.y && x == rhs.x);
    }

    int ToLinear(const Dim2& size) const
    {
        return ToLinear(size, *this);
    }

    static int ToLinear(const Dim2& size, const Dim2& idx)
    {
        return idx.y * size.x + idx.x;
    }

    static Dim2 FromLinear(const Dim2& size, int i)
    {
        div_t r = div(i, size.x);
        return Dim2(r.quot, r.rem);
    }
};

inline std::ostream &operator<<(std::ostream& os, const Dim2& rhs)
{
    std::stringstream ss;
    ss << "(" << rhs.y << ", " << rhs.x << ")";
    os << ss.str();
    return os;
}

} // end namespace smcsim

#endif // DIMENSION_HPP
