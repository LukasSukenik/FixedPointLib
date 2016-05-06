#include <iostream>
#include <cassert>
#include <bitset>

#include "fixed.h"

using namespace std;

/**
 * @brief test of uintInf_t addition and comparison
 * @return
 */
bool test();
bool testMult();
bool testMinus();
bool testComp();
bool testFixed1();
bool divisionAlgoTest();
bool shiftOperatorsTest();

uint64_t merge(uint32_t i, uint32_t j) {
    uint64_t result = i;
    result <<= 32;
    return result + j;
}

uint64_t division(uint64_t dividend, uint64_t divisor, uint64_t origdiv, uint64_t * remainder)
{
    int quotient = 1;

    if (dividend == divisor)
    {
        *remainder = 0;
        return 1;
    }

    else if (dividend < divisor)
    {
        *remainder = dividend;
        return 0;
    }

    while (divisor <= dividend)
    {
        divisor = divisor << 1;
        quotient = quotient << 1;
    }

    if (dividend < divisor)
    {
        divisor >>= 1;
        quotient >>= 1;
    }

    quotient = quotient + division(dividend - divisor, origdiv, origdiv, remainder);

    return quotient;
}

uint64_t division2(uint64_t dividend, uint64_t divisor, uint64_t * remainder) {

    uint64_t origdiv = divisor;
    uint64_t quotient;
    uint64_t sum_q=0;
    while(true) {
        quotient = 1;

        if (dividend == divisor) {
            *remainder = 0;
            sum_q += 1;
            break;
        } else if (dividend < divisor) {
            *remainder = dividend;
            break;
        }

        while (divisor <= dividend) {
            divisor <<= 1;
            quotient <<= 1;
        }

        if (dividend < divisor) {
            divisor >>= 1;
            quotient >>= 1;
        }

        sum_q += quotient;
        dividend -= divisor;
        divisor = origdiv;
    }

    return sum_q;
}

int main()
{
    //cout << "inf int addition and comparison, only 64bit and positive: " << std::boolalpha << test() << endl;
    //cout << "inf int multiplication, only 64bit and positive: " << std::boolalpha << testMult() << endl;
    //cout << "inf int subtraction, only 64bit and positive: " << std::boolalpha << testMinus() << endl;
    //cout << "Easy Fixed test, only 64bit and positive: " << std::boolalpha << testFixed1() << endl;
    //cout << "shift operators test : " << std::boolalpha << shiftOperatorsTest() << endl;
    cout << "division algo test : " << std::boolalpha << divisionAlgoTest() << endl;

    return 0;
}

bool shiftOperatorsTest() {
    srand(654654);
    uintInf_t n;
    uint64_t test;
    uint64_t r;

    for(uint64_t i = 0; i < 1000*1000; ++i) {

        test = rand();
        n = test;

        r = rand()%31;
        n <<= r;
        test <<= r;
        if(n.merge2() != test) {
            cout << test << " " << n.merge2() << endl;
            return false;
        }

        r = rand()%31;
        n >>= r;
        test >>= r;

        if(n.merge2() != test) {
            cout << test << " " << n.merge2() << endl;
            return false;
        }
    }
    return true;
}



bool divisionAlgoTest() {
    srand(64645);

    uintInf_t n;
    uintInf_t d;
    uintInf_t rem;
    uint64_t correct = 0;
    uintInf_t res;
    const uint64_t steps = 1000*10;

    for(uint64_t i=0; i<steps; ++i) {
        n = rand();
        n = n * (2<<20);
        d = rand();
        d = d * (2<<20);
        rem = 0;
        //res = division(n,d,d,&rem);
        res = n.division3(n,d,&rem);

        if(res.merge2() != n.merge2()/d.merge2() && rem.merge2() != n.merge2()%d.merge2()) {
            cout << "Error: wrong division" << n.merge2() << " / " << d.merge2() << " = " << (n.merge2()/d.merge2()) << " != ";
            cout << res.merge2() << " | " << (n.merge2()%d.merge2()) << "!=" << rem.merge2() << endl;
        }
        else
            correct++;
    }

    return steps == correct;
}

bool testFixed1() {
    Fixed num(1,1,1,"1/4");
    num.num.push_back(1);
    num.scale.push_back(4);

    Fixed num2(1,1,1,"1/4");
    num2.num.push_back(1);
    num2.scale.push_back(20);

    Fixed num3(1,1,1,"1/4");
    num3 = num + num2;

    cout << num.toString64bit() + " + " + num2.toString64bit() + " = " + (num3).toString64bit() << endl;

    return true;
}

bool testMinus() {
    srand(0);
    uintInf_t t1,t2;
    uint64_t x,y,res;

    for(int i=0; i<1000*1000; i++) {
        uint32_t n1,n2,n3,n4;
        n1 = UINT32_MAX - rand();
        n2 = UINT32_MAX - rand()%10000;
        n3 = UINT32_MAX - rand();
        n4 = UINT32_MAX - rand()%10000 - 100000;

        t1.push_back(n1);
        t1.push_back(n2);

        t2.push_back(n3);
        t2.push_back(n4);

        x = merge(n2,n1);
        y = merge(n4,n3);

        assert(x == merge(t1[1], t1[0])); // x == t1
        assert(y == merge(t2[1], t2[0])); // y == t2

        t1 = t1 - t2;
        res = x - y;

        if(res != merge(t1[1], t1[0])) {
            cout << x << "-" << y << "=" << x-y << endl;
            cout << "is :" << res << "\nbut:" << merge(t1[1], t1[0]) << endl;
            return false;
        }

        t1.clear();
        t2.clear();
    }

    return true;
}

bool test() {
    uintInf_t t1, t2;
    uint64_t x,y;

    srand(0);

    for(int i=0; i<1000*1000; i++) {
        uint32_t n1,n2,n3,n4;
        n1 = UINT32_MAX - rand();
        n2 = UINT32_MAX - rand();
        n3 = UINT32_MAX - rand();
        n4 = UINT32_MAX - rand();

        t1.push_back(n1);
        t1.push_back(n2);

        t2.push_back(n3);
        t2.push_back(n4);

        x = merge(n2,n1);
        y = merge(n4,n3);

        assert(x == merge(t1[1], t1[0])); // x == t1
        assert(y == merge(t2[1], t2[0])); // y == t2

        t1 = t1 + t2;
        x = x + y;

        if(x != merge(t1[1], t1[0])) {
            return false;
        }

        if( (t1 < t2) != (x < y) && (t1 <= t2) != (x <= y) && (t1 > t2) != (x > y) && (t1 >= t2) != (x >= y) && (t1 == t2) != (x == y) && (t1 != t2) != (x != y) ) {
            cout << "Comparison fail" << endl;
            return false;
        }

        t1.clear();
        t2.clear();
    }

    return true;
}

bool testMult() {
    uintInf_t t1, t2;
    uint64_t x,y;

    srand(0);

    for(int i=0; i<1000*1000; i++) {
        uint32_t n1,n2;
        n1 = UINT32_MAX - rand();
        n2 = UINT32_MAX - rand();

        t1.push_back(n1);
        t2.push_back(n2);

        x = n1;
        y = n2;

        t1 = t1 * t2;
        x = x * y;

        if(x != merge(t1[1], t1[0])) {
            return false;
        }

        t1.clear();
        t2.clear();
    }

    return true;
}
