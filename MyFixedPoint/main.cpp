//
//  main.cpp
//  MyFixedPoint
//
//  Created by Shihai Wang on 9/2/15.
//  Copyright (c) 2015 Directive. All rights reserved.
//

#include <iostream>
#include <climits>
#include <math.h>
#include "FixedPoint.h"

using namespace std;

#define check(x) \
{ \
    if (!(x)) \
    { \
        std::cout << "    " << #x << " failed" << " @Line:" << __LINE__ << std::endl << std::endl; \
    } \
}

template<unsigned int p>
bool AlmostEqual(fixed_point<p> a, float b)
{
    float allowed_error(0.0001f * fabsf(float(a) + b));
    const float kinda_small_number(0.001f);
    if (kinda_small_number > allowed_error)
    {
        allowed_error = kinda_small_number;
    }
    
    if ((float)a - b<= allowed_error && b - (float)a <= allowed_error)
    {
        return true;
    }
    else
    {
        std::cout << "    ->allowed error: " << allowed_error << "; actual error: " << (float)a - b << std::endl;
        return false;
    }
}

template<unsigned int p>
void RunTest()
{
    std::cout << "Testing " << p << " bits factional part.....\n";
    
    using fp = fixed_point<p>;
    
    fp fixed_a = 100;
    
    check (fixed_a + fp(200) == fp(300));
    check (fixed_a - fp(200) == fp(-100));
    
    //check (AmostEqual(fp(2.9f) * fp(3.8f), fp(2.9f*3.8f)));
    
    float fa = 234595.1f, fb = 123.456f;
    check (AlmostEqual(fp(fa) * fp(fb), (fa*fb)));
    check (AlmostEqual(fp(fa) / fp(fb), (fa/fb)));
    
    fa = 12345.6789f; fb = 4.f;
    check (AlmostEqual(fp(fa) / fp(fb), fa/fb))
    
    fa = -100.5f; fb = 254.8f;
    check (AlmostEqual(fp(fa) * fp(fb), (fa*fb)));
    check (AlmostEqual(fp(fa) / fp(fb), (fa/fb)));
    
    check (AlmostEqual(fp(1234.5f).inv(), (1.f/1234.5f)));
    check (AlmostEqual(fp(-12.90123f).inv(), (1.f/-12.90123f)));
    
    check (AlmostEqual(fp(234.f).invSqrt(), (1.f/sqrtf(234.f))));
    check (AlmostEqual(fp(345678.f).invSqrt(), (1.f/sqrtf(345678.f))));
	check(AlmostEqual(fp(3.f).invSqrt(), (1.f / sqrtf(3.f))));

    std::cout << "Done Testing " << p << " bits factional part\n\n";


}

int main(int argc, const char * argv[])
{
    static_assert (Mask<0>::value == 0, "");
    static_assert (Mask<3>::value == 0x7, "");
    static_assert (Mask<7>::value == 0x7F, "");
    static_assert (Mask<16>::value == 0xFFFF, "");
    static_assert (Mask<32>::value == 0xFFFFFFFF, "");
    static_assert (Mask<48>::value == 0xFFFFFFFFFFFF, "");
    static_assert (Mask<64>::value == 0xFFFFFFFFFFFFFFFF, "");
    
    
    RunTest<16>();
    RunTest<32>();
    
    return 0;
}


