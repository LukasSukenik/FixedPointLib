#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Fixed {
public:
    Fixed(int valid, int ibase, int obase, string num) : valid(valid), ibase(ibase), obase(obase) {
        // convert num to internal type based on ibase
    }

    Fixed& operator= (Fixed o);

    Fixed operator+ (Fixed o);
    Fixed operator- (Fixed o);

    Fixed operator* (Fixed o);
    Fixed operator/ (Fixed o);

    Fixed operator% (Fixed o);

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

    Fixed& operator%= (Fixed o);

    std::ostream& operator<<(std::ostream& o);
    string toString();

    void setOBase(int obase) {
        if(obase >= 2 && obase <= 999)
            this->obase = obase;
    }

private:
    // https://en.wikipedia.org/wiki/Fixed-point_arithmetic
    vector<long long int> num; // numerator
    long long int scale;       // denominator

    int valid;
    int ibase; // input base, 2 to 16
    int obase; // outputbase, 2 to 999
};
