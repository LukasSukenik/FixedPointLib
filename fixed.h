#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;

class uintInf_t : vector<uint64_t> {
public:

    bool operator== (const int& o) {
        return !this->operator !=(o);
    }

    bool operator!= (const int& o) {
        return false;
    }

    uintInf_t& operator= (uintInf_t o) {
        return o;
    }

    uintInf_t operator% (uintInf_t o) {
        return o;
    }

    uintInf_t operator+ (uintInf_t o);
    uintInf_t operator- (uintInf_t o);

    uintInf_t operator* (uintInf_t o)  {
        return o;
    }

    uintInf_t operator/ (uintInf_t o) {
        return o;
    }


};

uintInf_t gcd ( uintInf_t a, uintInf_t b );
uintInf_t lcm ( uintInf_t a, uintInf_t b );

class Fixed {
public:
    Fixed(int valid, int ibase, int obase, string num) : valid(valid), ibase(ibase), obase(obase) {
        // convert num to internal type based on ibase
    }

    Fixed& operator= (Fixed o) {
        swap(this->negative, o.negative);
        swap(this->num,   o.num);
        swap(this->scale, o.scale);
        swap(this->valid, o.valid);
        swap(this->ibase, o.ibase);
        swap(this->obase, o.obase);

        return *this;
    }

    Fixed operator+ (Fixed o) {

        // unify scale -> Least common multiple

    }

    Fixed operator- (Fixed o);

    Fixed operator* (Fixed o);
    Fixed operator/ (Fixed o);

    Fixed operator% (Fixed o) = delete; // works only for integers

    Fixed operator++ (int); // postfix
    Fixed operator-- (int );

    Fixed& operator++ (); // prefix
    Fixed& operator-- ();

    bool operator== (const Fixed& o);
    bool operator!= (const Fixed& o);

    bool operator< (const Fixed& o);
    bool operator> (const Fixed& o);

    bool operator<= (const Fixed& o);
    bool operator>= (const Fixed& o);

    Fixed& operator+= (Fixed o);
    Fixed& operator-= (Fixed o);

    Fixed& operator*= (Fixed o);
    Fixed& operator/= (Fixed o);

    Fixed& operator%= (Fixed o) = delete; // works only for integers

    std::ostream& operator<<(std::ostream& o);
    string toString();

    void setOBase(int obase) {
        if(obase >= 2 && obase <= 999)
            this->obase = obase;
    }

private:
    // https://en.wikipedia.org/wiki/Fixed-point_arithmetic
    //
    //  negative numbers -> negative scale
    //
    //  neccessary for easy operations implementation
    //
    bool negative;          // negative == true -> number < 0
    uintInf_t num;   // numerator, only last 32 bits are valid, rest 0
    uintInf_t scale; // denominator


    int valid;
    int ibase; // input base, 2 to 16
    int obase; // outputbase, 2 to 999
};

/*

Copyright (c) 2011, Louis-Philippe Lessard
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* Find the greatest common divisor of 2 numbers
* See http://en.wikipedia.org/wiki/Greatest_common_divisor
*
* @param[in] a First number
* @param[in] b Second number
* @return greatest common divisor
*/
uintInf_t gcd ( uintInf_t a, uintInf_t b )
{
    uintInf_t c;
    while ( a != 0 )
    {
        c = a;
        a = b%a;
        b = c;
    }
    return b;
}

/**
* Find the least common multiple of 2 numbers
* See http://en.wikipedia.org/wiki/Least_common_multiple
*
* @param[in] a First number
* @param[in] b Second number
* @return least common multiple
*/
uintInf_t lcm(uintInf_t a, uintInf_t b)
{
    return (b / gcd(a, b) ) * a;
}
