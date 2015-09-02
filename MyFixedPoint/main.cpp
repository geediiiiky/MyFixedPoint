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
        std::cout << #x << " failed" << std::endl; \
    } \
}

template<unsigned int p>
bool AmostEqual(fixed_point<p> a, fixed_point<p> b)
{
    fixed_point<p> kinda_small_number(0.0001f * fabsf(a +b));
    
    if (a - b<= kinda_small_number && b - a <= kinda_small_number)
    {
        return true;
    }
    else
    {
        std::cout << a - b << std::endl;
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
    
    float fa = 0.1f, fb = 10000.9f;
    check (AmostEqual(fp(fa) * fp(fb), fp(fa*fb)));
    
    fa = -100.5f; fb = 254.8f;
    check (AmostEqual(fp(fa) * fp(fb), fp(fa*fb)));
    
    
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
    
    static_assert ((1>>1) == 0, "");
    static_assert ((-1>>1) == -1, "");
    
    
    return 0;
}


