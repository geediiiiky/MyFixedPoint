// Copyright 2015 Directive Games Limited - All Rights Reserved

#pragma once

#include "int64.h"

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
        (1L << (2+p)) / 8,
        (1L << (2+p)) / 9,
        (1L << (2+p)) / 10,
        (1L << (2+p)) / 11,
        (1L << (2+p)) / 12,
        (1L << (2+p)) / 13,
        (1L << (2+p)) / 14,
        (1L << (2+p)) / 15
    };
    
    int exp = Helper::CountLeadingZeros(a);
    std::int64_t x = ((std::int64_t)rcp_tab[(a>>(60-exp))&0x7]) << 2;
    exp -= (64-p);
    
    if (exp <= 0)
        x >>= -exp;
    else
        x <<= exp;
    
    /* two iterations of newton-raphson  x = x(2-ax) */
    x = fix64_mul<(p)>(x,((2L<<(p)) - fix64_mul<p>(a,x)));
    x = fix64_mul<(p)>(x,((2L<<(p)) - fix64_mul<p>(a,x)));
    
    if (sign)
        return -x;
    else
        return x;
}

template<unsigned int p>
fix64 fix64_invSqrt(fix64 a)
{
    if (a == 0)
    {
        return FIXED64_OVERFLOW;
    }
    if (a == (1L << p))
    {
        return a;
    }
    
    // sqrt(4/8), sqrt(4/9), sqrt(4/10)..., sqrt(4/15)
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
    std::int64_t x = rsq_tab[(a>>(28-exp))&0x7]<<1;
    
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
    
    x = fix64_mul<p>((x>>1),((1L<<p)*3 - fix64_mul<p>(fix64_mul<p>(a,x),x)));
    x = fix64_mul<p>((x>>1),((1L<<p)*3 - fix64_mul<p>(fix64_mul<p>(a,x),x)));
    x = fix64_mul<p>((x>>1),((1L<<p)*3 - fix64_mul<p>(fix64_mul<p>(a,x),x)));
    //x = fix64_mul<p>((x>>1),((1L<<p)*3 - fix64_mul<p>(fix64_mul<p>(a,x),x)));
    
    return x;
}
