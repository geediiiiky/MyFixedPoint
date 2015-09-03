// Copyright 2015 Directive Games Limited - All Rights Reserved

#pragma once

#include "int64.h"
#include "BasicOperations.h"

template<unsigned int p>
class fixed_point
{
    // TODO: make this pulic for now to test
public:
    fix64 value = 0;
    
public:
    static const fix64 FIXED_ONE = (fix64(1) << p);
    
public:
    fixed_point() = default;
    fixed_point(int i) { value = i * FIXED_ONE; }
    explicit fixed_point(float f) { value  = (fix64)(f * FIXED_ONE); }
    explicit fixed_point(double f) { value  = (fix64)(f * FIXED_ONE); }
    
    operator float() { return float(value) / float(FIXED_ONE); }
    
    fixed_point& operator += (fixed_point r);// { value += r.value; return *this; }
    fixed_point& operator -= (fixed_point r);// { value -= r.value; return *this; }
    fixed_point& operator *= (fixed_point r);// { value = fixmul<p>(value, r.value); return *this; }
    fixed_point& operator /= (fixed_point r);// { value = fixdiv<p>(value, r.value); return *this; }
    
    fixed_point operator - () const { fixed_point x; x.value = -value; return x; }
    fixed_point operator + (fixed_point r) const { fixed_point x = *this; x += r; return x;}
    fixed_point operator - (fixed_point r) const { fixed_point x = *this; x -= r; return x;}
    fixed_point operator * (fixed_point r) const { fixed_point x = *this; x *= r; return x;}
    fixed_point operator / (fixed_point r) const { fixed_point x = *this; x /= r; return x;}
    
    bool operator == (fixed_point r) const { return value == r.value; }
    bool operator != (fixed_point r) const { return !(*this == r); }
    bool operator <  (fixed_point r) const { return value < r.value; }
    bool operator >  (fixed_point r) const { return value > r.value; }
    bool operator <= (fixed_point r) const { return value <= r.value; }
    bool operator >= (fixed_point r) const { return value >= r.value; }
    
    fixed_point inv() const;
    fixed_point invSqrt() const;
};

template<unsigned int p>
inline fixed_point<p>& fixed_point<p>::operator+= (fixed_point<p> r)
{
    value += r.value;
    return *this;
}

template<unsigned int p>
inline fixed_point<p>& fixed_point<p>::operator-= (fixed_point<p> r)
{
    value -= r.value;
    return *this;
}

template<unsigned int p>
inline fixed_point<p>& fixed_point<p>::operator*= (fixed_point<p> r)
{
    value = fix64_mul<p>(value, r.value);
    return *this;
}

template<unsigned int p>
inline fixed_point<p>& fixed_point<p>::operator/= (fixed_point<p> r)
{
    value = fix64_div<p>(value, r.value);
    return *this;
}


template<unsigned int p>
inline fixed_point<p> fixed_point<p>::inv() const
{
    fixed_point x;
    x.value = fix64_inv<p>(value);
    return x;
}

template<unsigned int p>
inline fixed_point<p> fixed_point<p>::invSqrt() const
{
    fixed_point x;
    x.value = fix64_invSqrt<p>(value);
    return x;
}

