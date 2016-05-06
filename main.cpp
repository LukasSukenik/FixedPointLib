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

uint64_t merge(uint32_t i, uint32_t j) {
    uint64_t result = i;
    result <<= 32;
    return result + j;
}

int main()
{
    cout << "inf int addition and comparison, only 64bit and positive: " << std::boolalpha << test() << endl;
    cout << "inf int multiplication, only 64bit and positive: " << std::boolalpha << testMult() << endl;
    cout << "inf int subtraction, only 64bit and positive: " << std::boolalpha << testMinus() << endl;

    uintInf_t a;
    a.push_back(2);
    a.push_back(1);

    return 0;
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
