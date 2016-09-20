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
#include <iomanip>
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
    float allowed_error(0.0005f * fabsf(b));
    return AlmostEqual(a, b, allowed_error);
    
}

template<unsigned int p>
bool AlmostEqual(fixed_point<p> a, float b, float tolerance)
{
    const float kinda_small_number(0.001f);
    if (kinda_small_number > tolerance)
    {
        tolerance = kinda_small_number;
    }
    
    if ((float)a - b<= tolerance && b - (float)a <= tolerance)
    {
        return true;
    }
    else
    {
        std::cout << "    ->allowed error: " << tolerance << "; actual error: " << (float)a - b << std::endl;
        std::cout << "    ->should be: " << b << "; actual: " << (float)a << std::endl;
        return false;
    }
}

template<unsigned int p>
bool AlmostCorretForTan(float rad, fixed_point<p> result)
{
    const float pi = (float)std::acos(-1);
    auto float_result = (float)result;
    auto reverse_rad = std::atan(float_result);
    auto diff = reverse_rad - rad;
    while (diff > pi / 2.f) diff -= pi;
    while (diff < -pi / 2.f) diff += pi;
    return std::abs(diff) < pi / 2.f / 1024.f;
}

template<unsigned int p>
void RunTest()
{
    std::cout << "Testing " << p << " bits factional part.....\n";
    
    using fp = fixed_point<p>;
    
    fp fixed_a(100);
    
    check (fixed_a + fp(200) == fp(300));
    check (fixed_a - fp(200) == fp(-100));
    
    //check (AmostEqual(fp(2.9f) * fp(3.8f), fp(2.9f*3.8f)));
    
    float fa = 234595.1f, fb = 123.456f;
    check (AlmostEqual(fp(fa) * fp(fb), (fa*fb)));
    check (AlmostEqual(fp(fa) / fp(fb), (fa/fb), 4.f));
    
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
    
    check(AlmostEqual(fp(1.69f).sqrt(), 1.3f));
    check(AlmostEqual(fp(3.f).sqrt(), sqrtf(3.f)));
    check(AlmostEqual(fp(3.2761f).sqrt(), 1.81f));
    check(AlmostEqual(fp(3805.9544f).sqrt(), sqrtf(3805.9544f)));
    check(AlmostEqual(fp(0.0396f).sqrt(), sqrtf(0.0396f)));
    
    check (fp(-109.12f).abs() == fp(109.12f));
    check (fp(1029.12f).abs() == fp(1029.12f));
    check (fp(0).abs() == fp(0));
    
    check (fp(-109.12f).floor() == -110);
    check (fp(1029.12f).floor() == 1029);
    check (fp(-0.f).floor() == -0);
    
    check (fp(-109.12f).trunc() == -109);
    check (fp(1029.12f).trunc() == 1029);
    check (fp(-0.f).trunc() == -0);
    
    check (AlmostEqual(sin(fp(3.14159f/6.f)), 0.5f));
    check (AlmostEqual(sin(fp(3.14159f/4.f)), 0.707106781186548f));
    check (AlmostEqual(sin(fp(3.14159f/4.f*3.f)), 0.707106781186548f));
    check (AlmostEqual(sin(fp(3.14159f/3.f)), 0.866025403784439f));
    
    check (AlmostEqual(cos(fp(3.14159f/6.f)), 0.866025403784439f));
    check (AlmostEqual(cos(fp(3.14159f/4.f)), 0.707106781186548f));
    check (AlmostEqual(cos(fp(3.14159f/3.f)), 0.5));
    
#define check_tan(rad) check (AlmostCorretForTan(rad, tan(fp(rad))))
    check_tan(3.14159f / 3.f);
    check_tan(0.3f);
    check_tan(6.03456f);

#define check_atan2(y, x) check (AlmostEqual(atan2(fp(y), fp(x)), std::atan2(y, x), 0.017f)) // less than 1 degree
    check_atan2(4.f, 0.1f);
    check_atan2(4.f, 10.f);
    check_atan2(5.f, -5.f);
    check_atan2(-20.f, -5.f);
    check_atan2(2.3f, 8.9f);
    
#define check_asin(x) check (AlmostEqual(asin(fp(x)), std::asin(x), 0.017f)) // less than 1 degree
    check_asin(1.f);
    check_asin(-1.f);
//    check_asin(0.89);
    check_asin(0.98f);
    check_asin(0.707f);
    check_asin(0.5f);
    check_asin(0.0f);
    
    std::cout << "Done Testing " << p << " bits factional part\n\n";

    std::cout << std::dec;
}

using UnitType = fixed_point<32>;

constexpr UnitType DG_SMALL_NUMBER(0x0068db8L, UnitType::is_raw);
constexpr UnitType pi(0x3243f6c00L, UnitType::is_raw);

static_assert(DG_SMALL_NUMBER.is_nearly_equal(10L, 100000L, 10L), "");
static_assert(pi.is_nearly_equal(314159L, 100000L, DG_SMALL_NUMBER.value), "");

static_assert(DG_SMALL_NUMBER.is_nearly_equal(0.0001f, 0.0000001f), "");



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
    
    UnitType fa(3.14f);
    UnitType fb(6.66f);
    UnitType fc(-2.99874f);
    UnitType fd(-0.2468f);
    
    cout << "internal value:" << endl;
    cout << std::hex << fa.value << endl;
    cout << std::hex << fb.value << endl;
    cout << std::hex << fc.value << endl;
    cout << std::hex << fd.value << endl;

#define unit_test(expr) cout << std::hex << "REQUIRE(" << #expr <<  " == 0x" << expr << ");" << endl;
    
    cout << endl << "added" << endl;
    cout << std::hex << "0x" << (fa + UnitType(5.666f)).value << endl;
    cout << std::hex << "0x" << (fb + UnitType(7.889f)).value << endl;
    cout << std::hex << "0x" << (fc + UnitType(-0.214f)).value << endl;
    cout << std::hex << "0x" << (fd + UnitType(0.669f)).value << endl;
    
    cout << endl << "substracted" << endl;
    unit_test((fa - UnitType(5.666f)).value);
    unit_test((fb - UnitType(8000.123f)).value);
    unit_test((fc - UnitType(-123.f)).value);
    unit_test((fd - UnitType(0.254f)).value);
    
    cout << endl << "multiplied" << endl;
    unit_test((fa * UnitType(1.2f)).value);
    unit_test((fb * UnitType(0.3f)).value);
    unit_test((fc * UnitType(-32)).value);
    unit_test((fd * UnitType(4521)).value);
    
    cout << endl << "multiplied" << endl;
    unit_test((fa / UnitType(0.5f)).value);
    unit_test((fb / UnitType(1.58f)).value);
    unit_test((fc / UnitType(9.88f)).value);
    unit_test((fd / UnitType(-123.456f)).value);
    
    cout << endl << "inv" << endl;
    unit_test((fa.inv()).value);
    unit_test((fb.inv()).value);
    unit_test((fc.inv()).value);
    unit_test((fd.inv()).value);
    
    cout << endl << "sqrt" << endl;
    unit_test((fa.sqrt()).value);
    unit_test((fb.sqrt()).value);
    unit_test((fc.sqrt()).value);
    unit_test((fd.sqrt()).value);
    
    cout << endl << "invSqrt" << endl;
    unit_test((fa.invSqrt()).value);
    unit_test((fb.invSqrt()).value);
    unit_test((fc.invSqrt()).value);
    unit_test((fd.invSqrt()).value);
    
    cout << endl << "sin" << endl;
    unit_test(sin(fa).value);
    unit_test(sin(fb).value);
    unit_test(sin(fc).value);
    unit_test(sin(fd).value);
    
    cout << endl << "cos" << endl;
    unit_test(cos(fa).value);
    unit_test(cos(fb).value);
    unit_test(cos(fc).value);
    unit_test(cos(fd).value);
    
    cout << endl << "tan" << endl;
    unit_test(tan(fa).value);
    unit_test(tan(fb).value);
    unit_test(tan(fc).value);
    unit_test(tan(fd).value);
    
    cout << endl << "sin" << endl;
    unit_test(asin(fa/10).value);
    unit_test(asin(fb/10).value);
    unit_test(asin(fc/10).value);
    unit_test(asin(fd).value);
    
    cout << endl << "cos" << endl;
    unit_test((fa/10).value);
    unit_test(acos(UnitType(0x50624dfe, UnitType::is_raw)).value);
    
    unit_test(acos(fa/10).value);
    unit_test(acos(fb/10).value);
    unit_test(acos(fc/10).value);
    unit_test(acos(fd).value);
    
    cout << endl << "tan" << endl;
    unit_test(atan2(fa, UnitType(1)).value);
    unit_test(atan2(fb, UnitType(1)).value);
    unit_test(atan2(fc, UnitType(1)).value);
    unit_test(atan2(fd, UnitType(1)).value);
//    cout << std::hex << "0x" << (fa + UnitType(5.666)).value << endl;
//    cout << std::hex << "0x" << (fb + UnitType(7.889)).value << endl;
//    cout << std::hex << "0x" << (fc + UnitType(-0.214)).value << endl;
//    cout << std::hex << "0x" << (fd + UnitType(0.669)).value << endl;
    
//    const auto pi = std::acos(-1);
//    auto count = 0;
//    for (std::uint64_t i = 0; i <= 0x40000000; i += 0x100000)
//    {
//        count++;
//        fixed_point<32> fp(i, fixed_point<32>::is_raw);
//        float result = std::tan((float)fp * pi * 2.f);
//        cout << "0x" << std::hex << fixed_point<32>(result).value << ", /* " << std::dec << std::setprecision(4) << std::fixed << (float)fp * 2.f << "Pi,\t" << result << " */" << endl;
//    }
    
    return 0;
}


