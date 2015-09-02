// Copyright 2015 Directive Games Limited - All Rights Reserved

#pragma once

#include <cstdint>

using fix64 = std::int64_t;
static_assert(sizeof(fix64) == 8, "fix64 type be 64 bits!");

const fix64 FIXED64_OVERFLOW = 0x8000000000000000;
const fix64 FIXED64_MINIMUM = 0x8000000000000001;
const fix64 FIXED64_MAXIMUM = 0x7FFFFFFFFFFFFFFF;


template <unsigned int N>
struct Mask
{
    static const fix64 value = ((fix64(1) << N) - fix64(1));
};

template <>
struct Mask<64>
{
    static const fix64 value = 0xFFFFFFFFFFFFFFFF;
};
