#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <cassert>

using namespace std;

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
        assert(!o.empty() && "Division by zero");
        if(o.size() == 1 && o.back()==1)
            return *this;
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

    uint64_t merge2() {
        uint64_t result = this->operator [](1);
        result <<= 32;
        return result + this->operator [](0);
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
};

std::ostream& operator<<(std::ostream& o, const uintInf_t& u) {
    for(auto rIt = u.rbegin(); rIt != u.rend(); ++rIt)
        o << bitset<32>(*rIt);
    return o;
}

uintInf_t gcd (uintInf_t n1, uintInf_t n2) {
    uintInf_t tmp;
    n1.negative = false;
    n2.negative = false;
    /*while (n2 != 0) {
        tmp = n1;
        n1 = n2;
        n2 = tmp % n2;
    }
    return n1;*/
    tmp.push_back(1);
    return tmp;
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
        o.num = (this->num * o.scale) + (o.num * this->scale);
        o.scale = o.scale * this->scale;
        o.minimizeFraction();
        return o;
    }

    Fixed& operator+= (Fixed o) {
        *this = this->operator +(o);
        return *this;
    }

    Fixed operator- (Fixed o) {
        o.num = (this->num * o.scale) - (o.num * this->scale);
        o.scale = o.scale * this->scale;
        o.minimizeFraction();
        return o;
    }

    Fixed& operator-= (Fixed o) {
        *this = this->operator -(o);
        return *this;
    }

    Fixed operator* (Fixed o) {
        o.scale = o.scale * this->scale;
        o.num = o.num * this->num;
        o.minimizeFraction();
        return o;
    }

    Fixed& operator*= (Fixed o) {
        *this = this->operator *(o);
        return *this;
    }

    Fixed operator/ (Fixed o) {
        o.scale = o.scale * this->num;
        o.num = o.num * this->scale;
        o.minimizeFraction();
        return o;
    }

    Fixed& operator/= (Fixed o) {
        *this = this->operator /(o);
        return *this;
    }

    Fixed operator% (Fixed o) = delete; // works only for integers
    Fixed& operator%= (Fixed o) = delete; // works only for integers

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

    bool operator== (const Fixed& o) {
        return ((this->num * o.scale) == (this->scale * o.num));
    }

    bool operator!= (const Fixed& o) {
        return ((this->num * o.scale) != (this->scale * o.num));
    }

    bool operator< (const Fixed& o) {
        return ((this->num * o.scale) < (this->scale * o.num));
    }

    bool operator> (const Fixed& o) {
        return ((this->num * o.scale) > (this->scale * o.num));
    }

    bool operator<= (const Fixed& o) {
        return ((this->num * o.scale) <= (this->scale * o.num));
    }

    bool operator>= (const Fixed& o) {
        return ((this->num * o.scale) >= (this->scale * o.num));
    }

    std::ostream& operator<<(std::ostream& o);
    string toString();

    string toString64bit() {
        return std::to_string(this->num.merge2()) + " / " + std::to_string(this->scale.merge2());
    }

    void setOBase(int obase) {
        if(obase >= 2 && obase <= 999)
            this->obase = obase;
    }

//private:
    // https://en.wikipedia.org/wiki/Fixed-point_arithmetic
    //
    uintInf_t num;   // numerator, only last 32 bits are valid, rest 0
    uintInf_t scale; // denominator

    int valid;
    int ibase; // input base, 2 to 16
    int obase; // outputbase, 2 to 999

    void minimizeFraction() {
        uintInf_t gcdNum = gcd(this->num, this->scale);
        this->scale = this->scale / gcdNum;
        this->num = this->num / gcdNum;
    }
};
