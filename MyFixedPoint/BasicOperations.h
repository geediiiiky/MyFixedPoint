// Copyright 2015 Directive Games Limited - All Rights Reserved

#pragma once

#include "int64.h"

namespace Helper {
    inline int CountLeadingZeros(std::uint64_t x)
    {
        int exp = 63;
        
        if (x & 0xffffffff00000000) {
            exp -= 32;
            x >>= 32;
        }
        
        if (x & 0xffff0000) {
            exp -= 16;
            x >>= 16;
        }
        
        if (x & 0xff00) {
            exp -= 8;
            x >>= 8;
        }
        
        if (x & 0xf0) {
            exp -= 4;
            x >>= 4;
        }
        
        if (x & 0xc) {
            exp -= 2;
            x >>= 2;
        }
        
        if (x & 0x2) {
            exp -= 1;
        }
        
        return exp;
    }
}

template<unsigned int p>
fix64 fix64_mul(fix64 inArg0, fix64 inArg1)
{
    // Each argument is divided to 64-p and p parts.
    //					AB
    //			*	 CD
    // -----------
    //					BD	(64-p) * (64-p) -> (128-2p) bit products
    //				 CB
    //				 AD
    //				AC
    //			 |----| 64 bit product
    std::int64_t A = (inArg0 >> p), C = (inArg1 >> p);
    std::uint64_t B = (inArg0 & Mask<p>::value), D = (inArg1 & Mask<p>::value);
    
    std::int64_t AC = A*C;    // TODO: overflow check
    std::int64_t AD_CB = A*D + C*B;
    std::uint64_t BD = B*D;
    
    std::int64_t product_hi = AC + (AD_CB >> p);    // TODO: overflow check
    
    // Handle carry from lower 32 bits to upper part of result.
    std::uint64_t ad_cb_temp = ((AD_CB & Mask<p>::value) << p) & Mask<2*p>::value;
    std::uint64_t product_lo = (BD + ad_cb_temp) & Mask<2*p>::value;
    if (product_lo < BD)
    {
        product_hi++;
    }
    
    // TODO: negative numbers are actually implemenation defined. properly check the overflow and don't use this!!!
    // this is not correctly if p < 32 anyways
    if (product_hi >> 63 != product_hi >> (64-p))
    {
        return FIXED64_OVERFLOW;
    }
    
    //return (product_hi << p) | (product_lo >> p);
    auto result = (product_hi << p) | (product_lo >> p);
    return result;
}

template<unsigned int p>
fix64 fix64_inv(fix64 a);

template<unsigned int p>
fix64 fix64_div(fix64 a, fix64 b)
{
    int leadingZeroes = Helper::CountLeadingZeros(b);
    if ((b << (leadingZeroes + 1)) == 0)
    {
        int shiftBits = 63 - p - leadingZeroes;
        if (shiftBits >= 0)
        {
            return a >> shiftBits;
        }
        else
        {
            return a << -shiftBits;
        }
    }
    
#if 0
    std::uint64_t remainder = a >= 0 ? a : -a;
    std::uint64_t divider = b >= 0 ? b : -b;
    std::uint64_t quotient = 0;
    std::uint64_t bit = fix64(1) << p;
    
    while (divider < remainder)
    {
        divider <<= 1;
        bit <<= 1;
    }
    
    while (bit != 0 && remainder != 0)
    {
        if (remainder >= divider)
        {
            quotient |= bit;
            remainder -= divider;
        }
        
        remainder <<= 1;
        bit >>= 1;
    }
    
    fix64 result = quotient;
    
    if ((a ^ b) & 0x8000000000000000)
    {
        result = -result;
    }
    
    return result;
#else
    // a faster implementation using newton-raphson to get 1/b first
    if ((b >> (32 + p/2)) && (b >> (32 + p/2)) + 1)
    {
        // if b is relatively big number
        fix64_mul<p>((a >> (32 - p/2)), fix64_inv<p>(b >> (32 - p/2)));
    }
    return fix64_mul<p>(a, fix64_inv<p>(b));
#endif
}



template<unsigned int p>
fix64 fix64_inv(fix64 a)
{
    if (a == 0)
    {
        return FIXED64_OVERFLOW;
    }
    
    bool sign = false;
    
    if (a < 0) {
        sign = true;
        a = -a;
    }
    
    // 4/8, 4/9, 4/10, ... 4/15
    
    // when p == 32
    //    static const std::uint64_t rcp_tab[] = {
    //        0x80000000, 0x71c71c71, 0x66666666, 0x5d1745d1, 0x55555555, 0x4ec4ec4e, 0x49249249, 0x44444444
    //    };
    
    static const std::uint64_t rcp_tab[] =
    {
        (fix64(1) << (2+p)) / 8,
        (fix64(1) << (2 + p)) / 9,
        (fix64(1) << (2 + p)) / 10,
        (fix64(1) << (2 + p)) / 11,
        (fix64(1) << (2 + p)) / 12,
        (fix64(1) << (2 + p)) / 13,
        (fix64(1) << (2 + p)) / 14,
        (fix64(1) << (2 + p)) / 15
    };
    
    int exp = Helper::CountLeadingZeros(a);
    std::int64_t x = ((std::int64_t)rcp_tab[(a>>(60-exp))&0x7]) << 2;
    exp -= (64-p);
    
    if (exp <= 0)
        x >>= -exp;
    else
        x <<= exp;
    
    /* two iterations of newton-raphson  x = x(2-ax) */
    x = fix64_mul<(p)>(x,((fix64(2)<<(p)) - fix64_mul<p>(a,x)));
    x = fix64_mul<(p)>(x, ((fix64(2)<< (p)) - fix64_mul<p>(a, x)));
    
    if (sign)
        return -x;
    else
        return x;
}

template<unsigned int p>
fix64 fix64_invSqrt(fix64 a)
{
    if (a <= 0)
    {
        return FIXED64_OVERFLOW;
    }
    if (a == (fix64(1) << p))
    {
        return a;
    }
    
    // invSqrt(2), invSqrt(2.25), invSqrt(2.5)..., invSqrt(3.75)
    // this is for p == 32
    static const std::uint64_t rsq_tab[] =
    {
        0xb504f333 >> (32 - p),
        0xaaaaaaaa >> (32 - p),
        0xa1e89b12 >> (32 - p),
        0x9a5fb1e8 >> (32 - p),
        0x93cd3a2c >> (32 - p),
        0x8e00d501 >> (32 - p),
        0x88d6772b >> (32 - p),
        0x8432a516 >> (32 - p)
    };
    
    int exp = Helper::CountLeadingZeros(a);
    std::int64_t x = rsq_tab[(a>>(60-exp))&0x7]<<1;
    
    exp -= (64-p);
    if (exp <= 0)
    {
        x >>= -exp>>1;
    }
    else
    {
        x <<= (exp>>1)+(exp&1);
    }
    
    if (exp & 1)
    {
        x = fix64_mul<p>(x, rsq_tab[0]);
    }
    
    // one itration should suffice
    x = fix64_mul<p>((x >> 1), ((fix64(1) << p) * 3 - fix64_mul<p>(fix64_mul<p>(a, x), x)));
    //x = fix64_mul<p>((x >> 1), ((fix64(1) << p) * 3 - fix64_mul<p>(fix64_mul<p>(a, x), x)));
    //x = fix64_mul<p>((x >> 1), ((fix64(1) << p) * 3 - fix64_mul<p>(fix64_mul<p>(a, x), x)));
    //x = fix64_mul<p>((x >> 1), ((fix64(1) << p) * 3 - fix64_mul<p>(fix64_mul<p>(a, x), x)));
    
    return x;
}

template<unsigned int p>
fix64 fix64_sqrt(fix64 a)
{
    if (a == 0 || a == (fix64(1) << p))
    {
        return a;
    }
    
    // well... this is kinda undefined behavior here
    if (a < 0)
    {
        a = -a;
    }
    
    fix64 s = (int64_t(1) << p);
    auto leadingZero = Helper::CountLeadingZeros(a);
    const int integral_bits = 64 - (int)p - leadingZero;
    if (integral_bits > 0)
    {
        s = fix64(1) << (p + ((integral_bits+1) / 2));
    }
    else
    {
        // a number less than 1.0
        const int fractinal_leading_zero = -integral_bits;
        s = fix64(1) << (p - fractinal_leading_zero / 2);
    }
    
    /* 4 iterations to converge */
    for (auto i = 0; i < 3; i++)
        s = (s + fix64_div<p>(a, s)) >> 1;
    return s;
}

template <int p>
fix64 float2fix(float f)
{
    return (fix64)(f * (fix64(1) << p));
}

//template<unsigned int p>
//int32_t fix64_cos(fix64 a)
//{
//    static const int32_t FIX_2PI	= float2fix<p>(6.28318530717958647692f);
//    static const int32_t FIX_R2PI = float2fix<p>(1.0f/6.28318530717958647692f);
//    
//    fix64 v;
//    /* reduce to [0,1) */
//    while (a < 0) a += FIX_2PI;
//    a = fix64_mul<p>(a, FIX_R2PI);
//    a += 0x4000;
//    
//    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
//    a >>= 4;
//    
//    v = (a & 0x400) ? sin_tab[0x3ff - (a & 0x3ff)] : sin_tab[a & 0x3ff];
//    v = fixmul<16>(v, 1 << 16);
//    return (a & 0x800) ? -v : v;
//}

extern const fix64 sin_tab[1025];

template<unsigned int p>
fix64 fix64_sin(fix64 a)
{
    const fix64 FIX_2PI	= float2fix<p>(6.28318530717958647692f);
    const fix64 FIX_R2PI = float2fix<p>(1.0f/6.28318530717958647692f);
    
    /* reduce to [0,1) */
    while (a < 0) a += FIX_2PI;
    while (a >= FIX_2PI) a -= FIX_2PI;
    a = fix64_mul<p>(a, FIX_R2PI);
    
    /* now in the range [0, 0x10000], reduce to [0, 0x1000] */
    auto normalized = (a >> (p - 12));
    normalized += (a & (1 << (p - 13))) ? 1 : 0;    //rounding
    
    fix64 v = (normalized & 0x400) ? sin_tab[0x400 - (normalized & 0x3ff)] : sin_tab[normalized & 0x3ff];
    v >>=  (32 - p);
    return (normalized & 0x800) ? -v : v;
}

template<unsigned int p>
fix64 fix64_cos(fix64 a)
{
    const fix64 FIX_PI_2	= float2fix<p>(1.5707963267949f);
    
    // +pi/2 so as to convert to calculation of sin(a)
    a += FIX_PI_2;
    
    return fix64_sin<p>(a);
}

extern const fix64 tan_tab[1025];

template<unsigned int p>
fix64 fix64_tan(fix64 a)
{
    const fix64 FIX_2PI	= float2fix<p>(6.28318530717958647692f);
    const fix64 FIX_R2PI = float2fix<p>(1.0f/6.28318530717958647692f);
    
    /* reduce to [0,1) */
    while (a < 0) a += FIX_2PI;
    while (a >= FIX_2PI) a -= FIX_2PI;
    a = fix64_mul<p>(a, FIX_R2PI);
    
    /* now in the range [0, 0x10000], reduce to [0, 0x1000] */
    auto normalized = (a >> (p - 12));
    normalized += (a & (1 << (p - 13))) ? 1 : 0;    //rounding
    
    fix64 v = (normalized & 0x400) ? tan_tab[0x400 - (normalized & 0x3ff)] : tan_tab[normalized & 0x3ff];
    v >>=  (32 - p);
    return (normalized & 0x400) ? -v : v;
}

template<unsigned int p>
fix64 fix64_atan2(fix64 inY , fix64 inX)
{
    const fix64 PI_DIV_4	= float2fix<p>(3.14159f / 4.f);
    const fix64 THREE_PI_DIV_4	= float2fix<p>(3.f * 3.14159f / 4.f);
    const fix64 PI_DIV_16	= float2fix<p>(3.14159f / 16.f);
    const fix64 FIVE_PI_DIV_16	= float2fix<p>(5.f * 3.14159f / 16.f);
    const fix64 PI_DIV_2	= float2fix<p>(3.14159f / 2.f);
    
    if (inX == 0)
    {
        if (inY >= 0)
        {
            return PI_DIV_2;
        }
        else
        {
            return -PI_DIV_2;
        }
    }
    
    fix64 angle = 0;
    
    /* Absolute inY */
    // fancy way
    // fix64 mask = (inY >> 63);
    // (inY + mask) ^ mask;
    fix64 abs_inY = inY > 0 ? inY : -inY;
    
    if (inX >= 0)
    {
        fix64 r = fix64_div<p>( (inX - abs_inY), (inX + abs_inY));
        fix64 r_3 = fix64_mul<p>(fix64_mul<p>(r, r),r);
        angle = fix64_mul<p>(PI_DIV_16 , r_3) - fix64_mul<p>(FIVE_PI_DIV_16,r) + PI_DIV_4;
    } else {
        fix64 r = fix64_div<p>( (inX + abs_inY), (abs_inY - inX));
        fix64 r_3 = fix64_mul<p>(fix64_mul<p>(r, r),r);
        angle = fix64_mul<p>(PI_DIV_16 , r_3) - fix64_mul<p>(FIVE_PI_DIV_16,r) + THREE_PI_DIV_4;
    }
    
    if (inY < 0)
    {
        angle = -angle;
    }
    
    return angle;
}

template<unsigned int p>
fix64 fix64_asin(fix64 x)
{
    const fix64 ONE	= float2fix<p>(1);
    
    if((x > ONE) || (x < -ONE))
    {
        return 0;
    }
    
    fix64 result = (ONE - fix64_mul<p>(x, x));
    result = fix64_atan2<p>(x, fix64_sqrt<p>(result));
    return result;
}

template<unsigned int p>
fix64 fix64_acos(fix64 x)
{
    const fix64 PI_DIV_2	= float2fix<p>(1.5707963267949f);
    return (PI_DIV_2 - fix64_asin<p>(x));
}
