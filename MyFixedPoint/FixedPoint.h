// Copyright 2015 Directive Games Limited - All Rights Reserved

#pragma once

#include "int64.h"
#include "BasicOperations.h"

#include <limits>
#include <cmath>

template<unsigned int p>
class fixed_point
{
    // TODO: make this pulic for now to test
public:
    fix64 value = 0;
    
public:
    static const fixed_point One() { return fixed_point(FIXED_ONE, true); }
    static const fixed_point Zero() { return fixed_point(0, true); }
    static const fixed_point Greatest() {return fixed_point(FIXED64_MAXIMUM, true); }
    static const fixed_point Least() {return fixed_point(FIXED64_MINIMUM, true); }
    static const unsigned int fraction_bits = p;
    using underlying_type = fix64;
    
public:
    fixed_point() = default;
    
    template<class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
    fixed_point(T i) { value = i * FIXED_ONE; }
    
    explicit fixed_point(float f) { value  = (fix64)(f * FIXED_ONE); }
    explicit fixed_point(double f) { value  = (fix64)(f * FIXED_ONE); }
    
    enum raw_flag { is_raw };
    explicit constexpr fixed_point(fix64 raw, raw_flag asRaw) : value(raw) {  }
    
//    fixed_point& operator=(int i) { value  = (i * FIXED_ONE); return *this; }
//    fixed_point& operator=(float f) { value  = (fix64)(f * FIXED_ONE); return *this; }
//    fixed_point& operator=(double d) { value  = (fix64)(d * FIXED_ONE); return *this; }
    
    explicit constexpr operator float() const { return float(value) / float(FIXED_ONE); }
    explicit constexpr operator int() const { return trunc(); }
    
    constexpr bool is_nearly_equal(std::uint64_t expected_numerator, std::uint64_t expected_denominator, std::uint64_t allowed_error_raw) const
    {
        return value - std::int64_t(expected_numerator * (int64_t(1) << p)) / expected_denominator <= allowed_error_raw;
    }
    
    constexpr bool is_nearly_equal(float actual, float tolerance) const
    {
        return operator float() - actual < tolerance && operator float() - actual > -tolerance;
    }
    
    fixed_point& operator += (fixed_point r);// { value += r.value; return *this; }
    fixed_point& operator -= (fixed_point r);// { value -= r.value; return *this; }
    fixed_point& operator *= (fixed_point r);// { value = fixmul<p>(value, r.value); return *this; }
    fixed_point& operator /= (fixed_point r);// { value = fixdiv<p>(value, r.value); return *this; }
    
    fixed_point operator - () const { fixed_point x; x.value = -value; return x; }
    fixed_point operator + () const { return *this; }
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
    fixed_point sqrt() const;
    fixed_point abs() const;
    
    std::int64_t floor() const;
    std::int64_t trunc() const;
    
private:
    static const fix64 FIXED_ONE = (fix64(1) << p);
};

namespace std
{
    template<unsigned int p> class numeric_limits<fixed_point<p>>
    {
    public:
        // according to numeric_limits<float>, max is the greatest, lowest is the least
        // min, refers to the minimum (normalized) number greater than zero.
        static fixed_point<p> max() {return fixed_point<p>::Greatest();};
        static fixed_point<p> lowest() {return fixed_point<p>::Least();};
        static fixed_point<p> min() {return fixed_point<p>(0x1, true);};
    };
}

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

template<unsigned int p>
inline fixed_point<p> fixed_point<p>::sqrt() const
{
    fixed_point x;
    x.value = fix64_sqrt<p>(value);
    
    return x;
}

template <unsigned int p>
inline fixed_point<p> fixed_point<p>::abs() const
{
    fixed_point<p> r;
    r.value = value > 0 ? value : -value;
    return r;
}

template <unsigned int p>
inline std::int64_t fixed_point<p>::trunc() const
{
    return value / FIXED_ONE;
}

template <unsigned int p>
inline std::int64_t fixed_point<p>::floor() const
{
    auto fractional = (Mask<p>::value & value);
    return value / FIXED_ONE + ((value < 0 && fractional != 0) ? -1 : 0);
}

template <unsigned int p, class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
inline fixed_point<p> operator*(T a, const fixed_point<p>& b)
{
    return b.operator*(a);
}

template <unsigned int p, class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
inline fixed_point<p> operator+(T a, const fixed_point<p>& b)
{
    return b.operator+(a);
}

template <unsigned int p, class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
inline fixed_point<p> operator-(T a, const fixed_point<p>& b)
{
    return fixed_point<p>(a).operator-(b);
}

template <unsigned int p, class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
inline fixed_point<p> operator/(T a, const fixed_point<p>& b)
{
    return fixed_point<p>(a).operator/(b);
}

template <unsigned int p>
inline fixed_point<p> sin(const fixed_point<p>& a)
{
    fixed_point<p> result;
    result.value = fix64_sin<p>(a.value);
    return result;
}

template <unsigned int p>
inline fixed_point<p> cos(const fixed_point<p>& a)
{
    fixed_point<p> result;
    result.value = fix64_cos<p>(a.value);
    return result;
}

template <unsigned int p>
inline fixed_point<p> tan(const fixed_point<p>& a)
{
    fixed_point<p> result;
    result.value = fix64_tan<p>(a.value);
    return result;
}

template <unsigned int p>
inline fixed_point<p> atan2(const fixed_point<p>& y, const fixed_point<p>& x)
{
    fixed_point<p> result;
    result.value = fix64_atan2<p>(y.value, x.value);
    return result;
}

template <unsigned int p>
inline fixed_point<p> asin(const fixed_point<p>& x)
{
    fixed_point<p> result;
    result.value = fix64_asin<p>(x.value);
    return result;
}

template <unsigned int p>
inline fixed_point<p> acos(const fixed_point<p>& x)
{
    fixed_point<p> result;
    result.value = fix64_acos<p>(x.value);
    return result;
}
