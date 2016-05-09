#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <cassert>

using namespace std;

const string DIGITS = "0123456789ABCDEF";

class uintInf_t : public vector<uint64_t> {
public:
    bool negative = false;          // negative == true -> number < 0

    uintInf_t() = default;
    uintInf_t(uint32_t n) {
        if(n!=0)
            this->push_back(n);
    }

    bool operator== (const uintInf_t& o) {
        if(this->empty() && o.empty())
            return true;
        if(this->empty() || o.empty())
            return false;

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
        if(this->empty() && o.empty())
            return false;

        //assert(o.back() !=0 && this->back() != 0);
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

    //uintInf_t& operator= (uintInf_t o) inherited

    uintInf_t operator% (uintInf_t o) {
        uintInf_t rem;
        division3(*this,o, &rem);
        return rem;
    }

    uintInf_t operator <<= (const unsigned int shift) {
        assert(shift <= 32);
        for(auto& item : *this)
            item <<= shift;
        propagateUP();
        return *this;
    }

    uintInf_t operator >>= (const unsigned int shift) {
        assert(shift <= 32);
        uint64_t t1 = 0;
        uint64_t t2 = 0;

        for(auto rIt = this->rbegin(); rIt != this->rend(); ++rIt) {
            t1 = (*rIt << (64-shift)) >> 32;
            *rIt >>= shift;
            *rIt += t2;
            t2 = t1;
        }

        while(this->back() == 0)
            this->pop_back();
        assert(this->back() != 0);
        return *this;
    }

    uintInf_t division3(uintInf_t dividend, uintInf_t divisor, uintInf_t * remainder) {

        uintInf_t origdiv = divisor;
        uintInf_t quotient;
        uintInf_t sum_q;
        while(true) {
            quotient = 1;

            if (dividend == divisor) {
                *remainder = 0;
                sum_q = sum_q + 1;
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

            sum_q = sum_q + quotient;
            dividend = dividend - divisor;
            divisor = origdiv;
        }


        assert(sum_q.empty() || sum_q.back() != 0);
        return sum_q;
    }

    uintInf_t operator/ (uintInf_t o) { // TODO: Long division algorithm
        assert(!o.empty() && "Division by zero");
        uintInf_t rem;
        return division3(*this, o, &rem);
    }

    uintInf_t operator+ (uintInf_t o) {
        if(this->empty() && o.empty())
            return o;
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
	    std::vector<uintInf_t> result;
	    result.resize(std::max(o.size(), this->size()));
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
        //assert(result[0].back() != 0);                               //TODO SIGSEGV
        return result[0];
    }

    uint64_t merge2() const {
        if(this->empty())
            return 0;
        uint64_t result = (this->size() == 2) ? this->operator [](1) : 0;
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
    while (n2 != 0) {
        tmp = n1;
        n1 = n2;
        n2 = tmp % n2;
    }
    return n1;
}

class Fixed {
public:
	Fixed() : num(0), scale(1), ibase(10), obase(10), precision(20) {}

    Fixed(string num, uint32_t ibase = 10, uint32_t obase = 10, uint32_t precision = 20) :
	    ibase(ibase), obase(obase), precision(precision) {
	    assert(2 <= ibase && ibase <= 16);
	    assert(2 <= obase && obase <= 999);

	    bool negative = false;
	    if (num[0] == '-') {
		    num = num.substr(1);
		    negative = true;
	    }

	    auto slash = num.find_first_of('/');
	    if (slash != string::npos) {
		    Fixed f1(num.substr(0, slash), this->ibase, this->obase, this->precision);
		    Fixed f2(num.substr(slash+1), this->ibase, this->obase, this->precision);
		    this->num = f1.num * f2.scale;
		    this->scale = f1.scale * f2.num ;
	    }
	    else if (num.empty()) {
		    this->num = 0;
		    this->scale = 1;
	    }
	    else {
		    convertInput(num);
	    }
	    minimizeFraction();
	    this->num.negative = negative;
    }

    Fixed& operator= (Fixed o) {
        swap(this->num,   o.num);
        swap(this->scale, o.scale);
        swap(this->precision, o.precision);
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
        Fixed res;
	    res.num = this->num * o.scale;
	    res.scale = this->scale * o.num;
        res.minimizeFraction();  //TODO 1/20 returns "empty"/20, gcd=1, maybe some problem with 1/gcd?
        return res;
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

    friend std::ostream& operator<<(std::ostream& o, Fixed & num) {
	    o << num.toString();
	    return o;
    }

    string toString() {
	    auto numerator = this->num;
	    numerator.negative = false;

	    std::string intPart = integerToString(numerator / this->scale);

	    std::string fractPart = fractionToString(numerator % this->scale);

	    //check if floating part contains only 0
	    return (fractPart.find_first_not_of("0") != string::npos) ? intPart + "." + fractPart :
	           (intPart.empty() || intPart == "-") ? "0" : intPart;
    }

    string toString64bit() {
        return std::to_string(this->num.merge2()) + " / " + std::to_string(this->scale.merge2());
    }

    void setOBase(uint32_t obase) {
        if(obase >= 2 && obase <= 999)
            this->obase = obase;
    }

	void setPrecision(uint32_t precision) {
		this->precision = precision;
	}

//private:
    // https://en.wikipedia.org/wiki/Fixed-point_arithmetic
    //
    uintInf_t num;   // numerator, only last 32 bits are valid, rest 0
    uintInf_t scale; // denominator

    uint32_t ibase; // input base, 2 to 16
	uint32_t obase; // outputbase, 2 to 999
	uint32_t precision;

    void minimizeFraction() {
        uintInf_t gcdNum = gcd(this->num, this->scale);
        this->scale = this->scale / gcdNum;
        this->num = this->num / gcdNum;
    }

	std::string leadingZeros(unsigned long cipher) {
		return string(digitsCount(this->obase) - digitsCount(cipher), '0') + std::to_string(cipher);
	}

	unsigned long digitsCount(unsigned long number) {
		unsigned long length = 1;
		while ( number /= 10 ) {
			length++;
		}
		return length;
	}

	void convertInput(string &num) {
		//get integral and real part of the number
		auto point = num.find_first_of('.');
		string intStr = num.substr(0, point);
		string fracStr = (point != string::npos) ? num.substr(point+1) : "";

		//convert integral part to decimal number
		uintInf_t intPart(0);
		uintInf_t power(1);
		for (unsigned long i = 0; i < intStr.length(); ++i) {
			auto digit = DIGITS.find(intStr[intStr.length()-i-1]);
			intPart = intPart + power * uintInf_t(digit);
			power = power * uintInf_t(this->ibase);
		}
		this->num = intPart;
		this->scale = 1;

		if (fracStr.empty()) {
			return;
		}

		//convert fractional part to decimal number
		Fixed tmp("0", this->ibase, this->obase, this->precision);
		for (unsigned long i = 0; i < fracStr.length(); ++i) {
			auto digit = DIGITS.find(fracStr[i]);
			tmp.num = digit;
			tmp.scale = tmp.scale * uintInf_t(this->ibase);
			*this += tmp;
		}
	}

	std::string integerToString(uintInf_t integer) {
		string integral;
		while (integer > 0) {
			auto remainder = (integer % uintInf_t(this->obase));
			auto cipher = (remainder.size() != 0) ? remainder.back() : 0; //TODO

			if (this->obase <= 16) {
				integral = DIGITS[cipher] + integral;
			}
			else {
				integral = " " + leadingZeros(cipher) + integral;
			}

			integer = integer / uintInf_t(this->obase);
		}

		return ((this->num.negative) ? "-" : "") + integral;
	}

	std::string fractionToString(uintInf_t fraction) {
		string fractPart;
		uint32_t i = 0;
		while (fraction != 0 && i != this->precision) {
			fraction = fraction * uintInf_t(this->obase);
			uintInf_t division = fraction / this->scale;
			auto cipher = (division.size() != 0) ? division.back() : 0;
			fraction = fraction % this->scale;

			if (this->obase <= 16) {
				fractPart += DIGITS[cipher];
			}
			else {
				fractPart += leadingZeros(cipher) + " ";
			}

			i++;
		}
		return fractPart;
	}
};
