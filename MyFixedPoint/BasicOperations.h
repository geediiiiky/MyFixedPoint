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
