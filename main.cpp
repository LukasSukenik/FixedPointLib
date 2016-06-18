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

bool testInfIntComplex();
bool testInfIntComplex64();

uint64_t merge(uint32_t i, uint32_t j) {
    uint64_t result = i;
    result <<= 32;
    return result + j;
}

int64_t merge2(uint32_t i, uint32_t j, bool negative) {
    int64_t result = i;
    result <<= 32;
    return (result + j) * (negative ? -1 : 1);
}

int main()
{
    Fixed a("101111000110000101001110",2,10); // 12345678
	cout << a << endl;

	Fixed b("1.375",10,5); //1.14141414...
	b.setPrecision(0);
	cout << b << endl; //1

	Fixed c("1010101.10111001",2,20); // 04 05.14 09 01 05
	cout << c << endl;

	Fixed d("-A5.2D90",16,10); //-165.177978515625
	d++;
	cout << d << endl; //-164.177978515625

	Fixed e("-1/20"); //-0.05
    e.setPrecision(1);
    cout << "should be -0.05 is " <<  e << endl; //0

	Fixed f("1.1/0.1100011",2); //cca 1.92307692308
	f--;
	cout << f << endl; //cca 0.92307692308

	Fixed g("-1");
	Fixed h("20");
	Fixed tmp = g/h; //-1.05
	tmp.setPrecision(2);
    cout << g << "/" << h << "=" << tmp << endl;


    cout << "inf int addition and comparison, only 64bit and positive: " << std::boolalpha << test() << endl;
    cout << "inf int multiplication, only 64bit: " << std::boolalpha << testMult() << endl;
    cout << "inf int subtraction, only 64bit and positive: " << std::boolalpha << testMinus() << endl;
    cout << "Easy Fixed test, only 64bit and positive: " << std::boolalpha << testFixed1() << endl;
    cout << "shift operators test : " << std::boolalpha << shiftOperatorsTest() << endl;
    cout << "division algo test : " << std::boolalpha << divisionAlgoTest() << endl;

    cout << "complex +- 64bit infinite integer test : " << std::boolalpha << testInfIntComplex64() << endl;
    cout << "complex infinite integer test : " << std::boolalpha << testInfIntComplex() << endl;

    return 0;
}

bool testInfIntComplex64() {
    intInf t1, t2, t3;
    int64_t x,y,z;

    srand(0);

    for(int i=0; i<1000*1000; i++) {
        uint32_t n1,n2,n3,n4,n5,n6;
        n1 = ( rand() ) % (1<<31);
        n2 = ( rand() ) % (1<<31);
        n3 = ( rand() ) % (1<<31);
        n4 = ( rand() ) % (1<<31);
        n5 = ( rand() ) % (1<<31);
        n6 = ( rand() ) % (1<<31);

        t1.push_back(n1);
        t1.push_back(n2);

        t2.push_back(n3);
        t2.push_back(n4);

        t3.push_back(n5);
        t3.push_back(n6);

        t1.negative = rand()%2 == 0;
        t2.negative = rand()%2 == 0;
        t3.negative = rand()%2 == 0;

        x = merge2(n2,n1, t1.negative);
        y = merge2(n4,n3, t2.negative);
        z = merge2(n6,n5, t3.negative);

        assert(x == merge2(t1[1], t1[0], t1.negative)); // x == t1
        assert(y == merge2(t2[1], t2[0], t2.negative)); // y == t2
        assert(z == merge2(t3[1], t3[0], t3.negative)); // z == t3

        t1 = t1 + t2;
        x  = x  + y; // t1 == x, t2 == y

        t2 = t2 - t3;
        y  = y  - z; // t3 == z

        if(x != merge2(t1[1], t1[0], t1.negative) ) {
            cout << x << " " << merge2(t1[1], t1[0], t1.negative) << endl;
            return false;
        }

        if(y != merge2(t2[1], t2[0], t2.negative) ) {
            cout << y << " " << merge2(t2[1], t2[0], t2.negative) << endl;
            return false;
        }

        if( (t1 < t2) != (x < y) && (t1 <= t2) != (x <= y) && (t1 > t2) != (x > y) && (t1 >= t2) != (x >= y) && (t1 == t2) != (x == y) && (t1 != t2) != (x != y) ) {
            cout << "Comparison fail" << endl;
            return false;
        }

        t1.clear();
        t2.clear();
        t3.clear();
    }

    return true;
}

bool testInfIntComplex() {
    intInf t1, t2, t3;

    srand(0);

    for(int i=0; i<100; i++) {
        uint32_t n1,n2,n3,n4,n5,n6;
        n1 = ( rand() ) % (1<<31);
        n2 = ( rand() ) % (1<<31);
        n3 = ( rand() ) % (1<<31);
        n4 = ( rand() ) % (1<<31);
        n5 = ( rand() ) % (1<<31);
        n6 = ( rand() ) % (1<<31);

        t1.push_back(n1);
        t1.push_back(n2);

        t2.push_back(n3);
        t2.push_back(n4);

        t3.push_back(n5);
        t3.push_back(n6);

        t1.negative = rand()%2 == 0;
        t2.negative = rand()%2 == 0;
        t3.negative = rand()%2 == 0;

        t1 = ((t1 * t2) + t1 +t2 *t3);
        t2 = t1/t2 *t1*t2*t3/t2*t2 +t1*t3/t2;

        t1.clear();
        t2.clear();
        t3.clear();
    }

    return true;
}

bool shiftOperatorsTest() {
    srand(654654);
    intInf n;
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

    intInf n;
    intInf d;
    intInf rem;
    uint64_t correct = 0;
    intInf res;
    const uint64_t steps = 1000*10;

    for(uint64_t i=0; i<steps; ++i) {
        n = rand();
        n = n * (2<<20);
        d = rand();
        //d = d * (2<<20);
        rem = 0;
        //res = division(n,d,d,&rem);
        //res = n.division3(n,d,&rem);
        res = n / d;

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
    Fixed num("1/4");
    //num.num.push_back(1);
    //num.scale.push_back(4);

    Fixed num2("1/20");
    //num2.num.push_back(1);
    //num2.scale.push_back(20);

    Fixed num3("0");
    num3 = num + num2;

    cout << num.toString64bit() + " + " + num2.toString64bit() + " = " + (num3).toString64bit() << endl;

    return true;
}

bool testMinus() {
    srand(0);
    intInf t1,t2;
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
    intInf t1, t2;
    uint64_t x,y;

    srand(0);

    for(int i=0; i<1000*100; i++) {
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
    intInf t1, t2;
    int64_t x,y;

    srand(0);

    for(int i=0; i<1000*100; i++) {
        int32_t n1,n2;
        n1 = ( rand() ) % (1<<31);
        n2 = ( rand() ) % (1<<31);

        t1.push_back(n1);
        t2.push_back(n2);

        t1.negative = rand()%2 == 0;
        t2.negative = rand()%2 == 0;

        x = t1.negative ? n1*-1 : n1;
        y = t2.negative ? n2*-1 : n2;

        assert(x == merge2(0, t1[0], t1.negative)); // x == t1
        assert(y == merge2(0, t2[0], t2.negative)); // y == t2

        t1 = t1 * t2;
        x = x * y;

        if(x != merge2(t1[1], t1[0], t1.negative )) {
            return false;
        }

        t1.clear();
        t2.clear();
    }

    return true;
}
