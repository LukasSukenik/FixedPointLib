#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <cassert>

using namespace std;

unsigned int gcd (unsigned int n1, unsigned int n2) {
    unsigned int tmp;
    while (n2 != 0) {
        tmp = n1;
        n1 = n2;
        n2 = tmp % n2;
    }
    return n1;
}

class uintInf_t : public vector<uint64_t> {
public:
    bool negative = false;          // negative == true -> number < 0

    bool operator== (const uintInf_t& o) {
        assert(o.back() !=0 && this->back() != 0);
        if(o.size() != this->size())
            return false;
        if(this->negative != o.negative) // 0 is reprezented by empty container
            return false;
        for(unsigned int i=0; i<this->size(); ++i) {
            if(this->operator [](i) != o[i])
                return false;
        }
        return true;
    }

    bool operator!= (const uintInf_t& o) {
        return !this->operator ==(o);
    }

    bool operator< (const uintInf_t& o) {
        assert(o.back() !=0 && this->back() != 0);
        if(o.size() > this->size())
            return true;
        if(o.size() < this->size())
            return false;
        if(!this->negative && o.negative) // (+) < (-)
            return false;
        if(this->negative && !o.negative) // (-) < (+)
            return true;
        for(int i=this->size()-1; i>=0; --i) {
            if(this->operator [](i) < o[i])
                return true;
            if(this->operator [](i) > o[i])
                return false;
            // equal continue on lesser numbers
        }
        return false; // they are equal
    }

    bool operator> (const uintInf_t& o) {
        return !this->operator< (o) && !this->operator ==(o);
    }

    bool operator<= (const uintInf_t& o) {
        return this->operator< (o) || this->operator ==(o);
    }

    bool operator>= (const uintInf_t& o) {
        return !this->operator< (o);
    }


    bool operator== (const unsigned int& o) {
        assert(this->back() != 0);
        return !this->operator !=(o);
    }

    bool operator!= (const unsigned int& o) {
        assert(this->back() != 0);
        if(this->empty() || this->size() > 1)
            return true;
        return o != this->operator [](0);
    }

    //uintInf_t& operator= (uintInf_t o) inherited

    uintInf_t operator% (uintInf_t o) {
        return o;
    }

    uintInf_t operator/ (uintInf_t o) { // TODO: Long division algorithm
        return o;
    }

    uintInf_t operator+ (uintInf_t o) {
        // some operations with (+) sign are infact (-)
        if(this->negative != o.negative) {
            if(!this->negative) {   // (+a) + (-b) -> (+a) - (+b)
                o.negative = false;
                return this->operator -(o);
            } else {                // (-a) + (+b) -> (+b) - (+a)
                this->negative = false;
                return o.operator -(*this);
            }
        }
        uintInf_t result;
        result.resize(std::max(o.size(), this->size()));

        for(auto& item : result) {
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

        assert(result.back() != 0);

        return result;
    }

    uintInf_t operator- (uintInf_t o) {
        // some operations with (-) sign are infact (+)
        if(this->negative != o.negative) {
            o.negative = this->negative;
            return this->operator +(o);
        }

        // Problem: We have unsigned integers -> cant go below 0
        uintInf_t *big, *small;
        uintInf_t result; // is big - small
        uint64_t temp = 0;

        if(this->operator >=(o)) {
            big = this;
            small = &o;
            result.negative = this->negative;
        } else {
            big = &o;
            small = this;
            result.negative = !this->negative;
        }

        assert(big->size() >= small->size());
        result = *big;

        for(auto& item : result)
            item += (1ull<<32);
        for(unsigned int i=0; i<small->size(); ++i)
            result[i] -= small->operator [](i);
        for(auto& item : result) {
            item -= temp;
            temp = 0;
            if(item >= (1ull<<32))
                item -= (1ull<<32);
            else
                temp = 1;
        }

        while(result.back() == 0)
            result.pop_back();

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
        for(unsigned int i=1; i<std::max(o.size(), this->size()); i++) {
            result[0] = result[0] + result[i];
        }
        result[0].negative = (this->negative == o.negative) ? false : true;
        assert(result[0].back() != 0);
        return result[0];
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

    uint64_t merge(uint32_t i, uint32_t j) {
        uint64_t result = i;
        result <<= 32;
        return result + j;
    }


};

std::ostream& operator<<(std::ostream& o, const uintInf_t& u) {
    for(auto rIt = u.rbegin(); rIt != u.rend(); ++rIt)
        o << bitset<32>(*rIt);
    return o;
}

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
        // set apropriate denumarator (scale), multiply (TODO largest common multiple)
        o.scale = o.scale * this->scale;
        // multiply the num according to new scale
        o.num = o.num * this->scale;
        this->num = this->num * o.scale;
        // add nums
        o.num = this->num + o.num;
        return o;
    }

    Fixed operator- (Fixed o) {
        // set apropriate denumarator (scale)
        o.scale = o.scale * this->scale;
        // multiply the num according to new scale
        o.num = o.num * this->scale;
        this->num = this->num * o.scale;
        // subtract nums
        o.num = this->num - o.num;
        return o;
    }

    Fixed operator* (Fixed o) {
        o.scale = o.scale * this->scale;
        o.num = o.num * this->num;
        // TODO: Make the fraction so that it doesnt have common divisors
        return o;
    }

    Fixed operator/ (Fixed o) {
        o.scale = o.scale * this->num;
        o.num = o.num * this->scale;
        // TODO: Make the fraction so that it doesnt have common divisors
        return o;
    }

    Fixed operator% (Fixed o) = delete; // works only for integers

    Fixed operator++ (int) { // postfix
        Fixed tmp(*this);
        ++(*this);
        return tmp;
    }
    Fixed operator-- (int ) {
        Fixed tmp(*this);
        --(*this);
        return tmp;
    }

    Fixed& operator++ () { // prefix
        uintInf_t inc = this->scale;
        inc.negative = false;
        this->num = this->num + inc;
        return *this;
    }
    Fixed& operator-- () {
        uintInf_t inc = this->scale;
        inc.negative = true;
        this->num = this->num + inc;
        return *this;
    }

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
    uintInf_t num;   // numerator, only last 32 bits are valid, rest 0
    uintInf_t scale; // denominator

    int valid;
    int ibase; // input base, 2 to 16
    int obase; // outputbase, 2 to 999
};
