#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <bitset>

using namespace std;

class uintInf_t : public vector<uint64_t> {
public:
    bool negative = false;          // negative == true -> number < 0

    bool operator== (const int& o) {
        return !this->operator !=(o);
    }

    bool operator!= (const int& o) {
        if(this->empty() || o < 0 || this->size() > 1)
            return true;
        return o != this->operator [](0);
    }

    //uintInf_t& operator= (uintInf_t o) inherited

    uintInf_t operator% (uintInf_t o) {
        return o;
    }

    uintInf_t operator+ (uintInf_t o) { // work only for all positive or all negative numbers
        if(this->negative != o.negative) {
            // TODO: use operator -
        }
        uintInf_t result;
        result.resize(std::max(o.size(), this->size()));

        for(auto item : result) {
            item = 0;
        }
        for(unsigned int i=0; i<o.size(); i++) {
            result[i] += o[i];
        }
        for(unsigned int i=0; i<this->size(); i++) {
            result[i] += this->operator [](i);
        }
        result.propagateUP();
        result.negative = this->negative;
        return result;
    }

    uintInf_t operator- (uintInf_t o) { //
        if(this->negative != o.negative) {
            // TODO: use operator +
        }

        uintInf_t *big, *small;
        uintInf_t result;

        // this has more numbers -> bigger, || ( a && this is only evaluated is a == true)
        if(this->size() > o.size() || (this->size() == o.size() && this->back() > o.back())) {
            big = this;
            small = &o;
        } else {
            big = &o;
            small = this;
        }

        result = *big;
        result.bitShiftUp();

        for(unsigned int i=0; i<small->size(); i++) {
            result[i] = ((*small)[i] << 32);
        }

        result.propagateDown();
        return result;
    }

    uintInf_t operator* (uintInf_t o)  {
        uintInf_t result[ std::max(o.size(), this->size()) ];
        for(auto& item : result) {
            item.resize( o.size() * this->size() );
        }

        int batch = 0;
        for(unsigned int i=0; i < ( o.size() * this->size() ) ; ++i) {
            result[batch][i] = 0;
            for(unsigned int j=0; j<o.size(); ++j) {
                for(unsigned int k=0; k<this->size(); ++k) {
                    if(i == (j+k)) {
                        result[batch][i] += o[j] * this->operator [](k);
                        ++batch;
                    }
                }
            }
            batch = 0;
        }
        for(auto& item : result) {
            item.propagateUP();
        }
        for(int i=1; i<std::max(o.size(), this->size()); i++) {
            result[0] = result[0] + result[i];
        }
        result[0].negative = this->negative;
        return result[0];
    }

    uintInf_t operator/ (uintInf_t o) {
        return o;
    }

    void print() {
        for(auto item : *this) {
            cout << item << " ";
        }
        cout << endl;
    }

private:
    /**
     * @brief propagate set the vector elements so that first 32 bits are 0
     */
    void propagateUP() {
        uint64_t temp = 0;
        for(auto& item : *this) {
            item += temp;
            temp = item;
            temp >>= 32;
            item <<= 32;
            item >>= 32;
        }
        if(temp != 0)
            this->push_back(temp);
    }

    // TODO: finish
    void propagateDown() {
        uint64_t temp = 0;
        for(auto& item : *this) {
            if(item < UINT32_MAX) {
                item += UINT32_MAX;
            }
        }
    }

    void bitShiftUp() {
        for(auto& item : *this) {
            item <<= 32;
        }
    }

    void bitShiftDown() {
        for(auto& item : *this) {
            item >>= 32;
        }
    }
};

class Fixed {
public:
    Fixed(int valid, int ibase, int obase, string num) : valid(valid), ibase(ibase), obase(obase) {
        // convert num to internal type based on ibase
    }

    Fixed& operator= (Fixed o) {
        swap(this->num,   o.num);
        swap(this->scale, o.scale);
        swap(this->valid, o.valid);
        swap(this->ibase, o.ibase);
        swap(this->obase, o.obase);

        return *this;
    }

    Fixed operator+ (Fixed o) {
        // set apropriate denumarator (scale), multiply (best is solution is largest common multiple)

        // multiply the num according to new scale

        // add nums
        o.num = this->num + o.num;
        return o;
    }

    Fixed operator- (Fixed o) {
        // set apropriate denumarator (scale)

        // multiply the num according to new scale

        // subtract nums
        o.num = this->num - o.num;
        return o;
    }

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

    uintInf_t num;   // numerator, only last 32 bits are valid, rest 0
    uintInf_t scale; // denominator


    int valid;
    int ibase; // input base, 2 to 16
    int obase; // outputbase, 2 to 999
};
