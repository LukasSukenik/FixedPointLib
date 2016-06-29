#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <cassert>

const std::string DIGITS = "0123456789ABCDEF";

const uint64_t mask32 = std::numeric_limits<uint32_t>::max();


class IntInf : public std::vector<uint32_t> {
public:
    bool negative = false;          // negative == true -> number < 0

    IntInf() = default;
    IntInf(int32_t n) {
        if(n!=0)
            this->push_back(n);
        if(n < 0)
            negative = true;
    }

    bool operator== (const IntInf & o) const {
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
            if((*this)[i] != o[i])
                return false;
        }
        return true;
    }

    bool operator!= (const IntInf & o) const {
        return !(*this == o);
    }

    bool operator< (const IntInf & o) const {
        if(this->empty() && o.empty())
            return false;

	    if(!this->negative && o.negative) // (+) < (-)
		    return false;
	    if(this->negative && !o.negative) // (-) < (+)
		    return true;
        if(o.size() > this->size())
            return true;
        if(o.size() < this->size())
            return false;
        for(int i=this->size()-1; i>=0; --i) {
            if((*this)[i] < o[i])
                return true;
            if((*this)[i] > o[i])
                return false;
            // equal continue on lesser numbers
        }
        return false; // they are equal
    }

    bool operator> (const IntInf & o) const {
        return o < *this;
    }

    bool operator<= (const IntInf & o) const {
        return !(o < *this);
    }

    bool operator>= (const IntInf & o) const {
        return !(*this < o);
    }

    //uintInf_t& operator= (uintInf_t o) inherited

    IntInf operator >> (const unsigned int shift) {
        IntInf o(*this);
        o >>= shift;
        return o;
    }


    IntInf operator << (const unsigned int shift) {
        IntInf o(*this);
        o <<= shift;
        return o;
    }

    /**
     * @brief operator <<= ( <<=1  equal to *=2)
     * @param shift
     * @return
     */
    IntInf operator <<= (unsigned int shift) {
        if(this->empty())
            return *this;

        unsigned int shift32 = shift/32;
        shift -= (32*shift32);
        if(shift32 > 0) {
            this->resize(this->size() + shift32, 0);
            for(unsigned int i = this->size()-1; i >= shift32; --i) { // i-shift32 >= 0 -> i >= shift32
                (*this)[i] = (*this)[i-shift32];
                (*this)[i-shift32] = 0;
            }
        }

        if(shift > 0) {
            this->push_back(0);

            for(unsigned int i = this->size()-1; i > 0; --i) {
                (*this)[i] += (*this)[i-1] >> (32-shift);
                (*this)[i-1] <<= shift;
            }

            if(this->back() == 0)
                this->pop_back();
        }
        return *this;
    }

    /**
     * @brief operator >>= ( >>=1  equal to /=2)
     * @param shift
     * @return
     */
    IntInf operator >>= (unsigned int shift) {
        if(this->empty())
            return *this;

        unsigned int shift32 = shift/32;
        shift -= (32*shift32);

        if(shift32 > 0) {
            if(this->size() <= shift32) {
                this->clear();
                return *this;
            }

            for(unsigned int i = 0; i < this->size()-shift32; ++i) { // i+shift32 < size()
                (*this)[i] = (*this)[i+shift32];
            }
            this->resize(this->size() - shift32);
        }

        if(shift > 0) {
            (*this)[0] >>= shift;

            if(this->size() > 1) {
                for(unsigned int i = 1; i < this->size(); ++i) {
                    (*this)[i-1] += (*this)[i] << (32-shift);
                    (*this)[i] >>= shift;
                }
            }

            if(this->back() == 0)
                this->pop_back();
        }
        return *this;
    }

    IntInf division3(IntInf dividend, IntInf divisor, IntInf * remainder) {

        bool res = (dividend.negative || divisor.negative) && !(dividend.negative && divisor.negative);
        dividend.negative = false;
        divisor.negative = false;
        IntInf origdiv = divisor;
        IntInf quotient;
        IntInf sum_q;
        while(true) {
            quotient = 1;

            if(dividend.size() > 100)
                exit(1);

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
        sum_q.negative = res;
        return sum_q;
    }


    IntInf operator% (IntInf o) {
        IntInf rem;
        division3(*this,o, &rem);
        return rem;
    }

    IntInf operator/ (IntInf o) {
        assert(!o.empty() && "Division by zero");
        IntInf rem;
        return division3(*this, o, &rem);
    }

    IntInf operator+ (IntInf o) {
        if(this->empty() && o.empty())
            return o;
        // some operations with (+) sign are infact (-)
        if(this->negative != o.negative) {
            if(!this->negative) {   // (+a) + (-b) -> (+a) - (+b)
                o.negative = false;
                return *this - o;
            } else {                // (-a) + (+b) -> (+b) - (+a)
                this->negative = false;
                return o - *this;
            }
        }

        uint64_t temp;
        o.resize(std::max( o.size(), this->size()), 0);
        o.push_back(0);

        for(unsigned int i=0; i < this->size(); ++i) {
            temp = (*this)[i];
            temp += o[i];

            o[i] = ( temp & mask32 );
            o[i+1] += ( temp>>32 ); // o is always 1 bigger than *this
        }

        o.negative = this->negative;

        while(o.back() == 0) // due to intermediate results of operator *, which have 0 on higher indexes
            o.pop_back();
        assert(o.empty() || o.back() != 0);

        return o;
    }

    IntInf operator- (IntInf o) {
        // some operations with (-) sign are infact (+)
        if(this->negative != o.negative) {
            o.negative = this->negative;
            return *this + o;
        }

        // Problem: We have unsigned integers -> cant go below 0
        IntInf *big, *small;
        uint64_t temp = 0;
        bool equal=false;

        if( !(this->lt(o,equal)) ) { // this >= o
            big = this;
            small = &o;
            o.negative = this->negative;
        } else {
            big = &o;
            small = this;
            o.negative = !this->negative;
        }
        if(equal)
            return 0;

        assert(big->size() >= small->size());

        o.resize(big->size(), 0);
        o.push_back(0); // for convenience
        temp = 1ull<<32;
        for(unsigned int i=0; i < this->size(); ++i) {
            temp = ((1ull<<32)-1) + (temp>>32); // here temp>>32 == 1 -> we are OK, temp>>32 == 0 -> underflow occured
            assert(i < big->size());
            temp += static_cast<uint64_t>( (*big)[i] );
            temp -= static_cast<uint64_t>( (*small)[i] );

            o[i] = ( temp & mask32 );
        }

        while(o.back() == 0)
            o.pop_back();

        assert(o.empty() || o.back() != 0);

        return o;
    }

    IntInf operator* (IntInf o)  {
        if(this->empty() || o.empty())
            return IntInf();

        IntInf result;
        uint64_t temp = 0;
        result.resize(o.size() + this->size() +1, 0);

        for(unsigned int sum = 0; sum < result.size()-1; ++sum) {
            for(unsigned int i = 0; i < this->size(); ++i) {
                for(unsigned int j = 0; j < o.size(); ++j) {
                    if(i+j == sum) {
                        temp = (*this)[i];
                        temp *= o[j];
                        for(unsigned int k = sum; k < result.size(); ++k) {
                            temp += result[k];
                            result[k] = ( temp & mask32 );
                            temp >>= 32;
                        }
                    }
                }
            }
        }

        result.negative = this->negative != o.negative;

        while(result.back() == 0)
            result.pop_back();
        assert(result.empty() || result.back() != 0);

        return result;
    }

    uint64_t merge2() const {
        if(this->empty())
            return 0;
        uint64_t result = (this->size() == 2) ? (*this)[1] : 0;
        result <<= 32;
        return result + (*this)[0];
    }

    int64_t mergeSigned() {
        if(this->empty())
            return 0;
        int64_t result = (this->size() == 2) ? (*this)[1] : 0;
        result <<= 32;
        return ( result + (*this)[0] ) * (negative ? -1 : 1);
    }

private:
    /**
     * @brief duplicit operator <, For more effective operator- it is beneficial to know if the numbers are equal
     * @param o
     * @return
     */
    bool lt (const IntInf & o, bool& equal) const {
        if(this->empty() && o.empty())
            return false;
        equal = false;

        if(!this->negative && o.negative) // (+) < (-)
            return false;
        if(this->negative && !o.negative) // (-) < (+)
            return true;
        if(o.size() > this->size())
            return true;
        if(o.size() < this->size())
            return false;
        for(int i=this->size()-1; i>=0; --i) {
            if((*this)[i] < o[i])
                return true;
            if((*this)[i] > o[i])
                return false;
            // equal continue on lesser numbers
        }
        equal = true;
        return false; // they are equal
    }
};

std::ostream& operator<<(std::ostream& o, const IntInf & u) {
    if(u.empty()) {
        o << std::bitset<32>(0);
    }
    for(auto rIt = u.rbegin(); rIt != u.rend(); ++rIt)
        o << std::bitset<32>(*rIt);
    return o;
}

IntInf gcd (IntInf n1, IntInf n2) {
    IntInf tmp;
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

    Fixed(std::string num, uint32_t ibase = 10, uint32_t obase = 10, uint32_t precision = 20) :
	    ibase(ibase), obase(obase), precision(precision) {
	    assert(2 <= ibase && ibase <= 16);
	    assert(2 <= obase && obase <= 999);

	    bool negative = false;
	    if (num[0] == '-') {
		    num = num.substr(1);
		    negative = true;
	    }

        auto slash = num.find_first_of('/');
        if (slash != std::string::npos) {
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
	    std::swap(this->num,   o.num);
	    std::swap(this->scale, o.scale);
	    std::swap(this->precision, o.precision);
	    std::swap(this->ibase, o.ibase);
	    std::swap(this->obase, o.obase);

        return *this;
    }

    Fixed& operator+= (Fixed o) {
        this->num = (this->num * o.scale) + (o.num * this->scale);
        this->scale = o.scale * this->scale;
        this->minimizeFraction();
        return *this;
    }

    Fixed operator+ (Fixed o) {
        Fixed res(*this);
        return res+=o;
    }

    Fixed operator- (Fixed o) {
        Fixed res(*this);
        return res-=o;
    }

    Fixed& operator-= (Fixed o) {
        this->num = (this->num * o.scale) - (o.num * this->scale);
        this->scale = o.scale * this->scale;
        this->minimizeFraction();
        return *this;
    }

    Fixed operator* (Fixed o) {
        Fixed res(*this);
        return res*=o;
    }

    Fixed& operator*= (Fixed o) {
        this->scale = o.scale * this->scale;
        this->num = o.num * this->num;
        this->minimizeFraction();
        return *this;
    }

    Fixed operator/ (Fixed o) {
        Fixed res(*this);
        return res/=o;
    }

    Fixed& operator/= (Fixed o) {
        this->num = this->num * o.scale;
        this->scale = this->scale * o.num;
        this->minimizeFraction();
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
        IntInf inc = this->scale;
        inc.negative = false;
        this->num = this->num + inc;
        return *this;
    }
    Fixed& operator-- () {
        IntInf inc = this->scale;
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

    friend std::ostream& operator<<(std::ostream& o, const Fixed & num) {
	    o << num.toString();
	    return o;
    }

	std::string toString() const {
	    auto numerator = this->num;
	    numerator.negative = false;

	    std::string intPart = integerToString(numerator / this->scale);

	    std::string fractPart = fractionToString(numerator % this->scale);

	    //check if floating part contains only 0
	    return (fractPart.find_first_not_of("0") != std::string::npos) ? intPart + "." + fractPart :
	           (intPart.empty() || intPart == "-") ? "0" : intPart;
    }

	std::string toString64bit() {
        return std::to_string(this->num.merge2()) + " / " + std::to_string(this->scale.merge2());
    }

    void setOBase(uint32_t obase) {
        if(obase >= 2 && obase <= 999)
            this->obase = obase;
    }

	void setPrecision(uint32_t precision) {
		this->precision = precision;
	}

private:
    // https://en.wikipedia.org/wiki/Fixed-point_arithmetic
    //
    IntInf num;   // numerator, only last 32 bits are valid, rest 0
    IntInf scale; // denominator

    uint32_t ibase; // input base, 2 to 16
	uint32_t obase; // outputbase, 2 to 999
	uint32_t precision;

    void minimizeFraction() {
        if(this->scale == 1)
            return;
        IntInf gcdNum = gcd(this->num, this->scale);
        this->scale = this->scale / gcdNum;
        this->num = this->num / gcdNum;
    }

	std::string leadingZeros(unsigned long cipher) const {
		return std::string(digitsCount(this->obase) - digitsCount(cipher), '0') + std::to_string(cipher);
	}

	unsigned long digitsCount(unsigned long number) const {
		unsigned long length = 1;
		while ( number /= 10 ) {
			length++;
		}
		return length;
	}

	void convertInput(std::string &num) {
		//get integral and real part of the number
		auto point = num.find_first_of('.');
		std::string intStr = num.substr(0, point);
		std::string fracStr = (point != std::string::npos) ? num.substr(point+1) : "";

		//convert integral part to decimal number
        IntInf intPart(0);
        IntInf power(1);
		for (unsigned long i = 0; i < intStr.length(); ++i) {
			auto digit = DIGITS.find(intStr[intStr.length()-i-1]);
            intPart = intPart + power * IntInf(digit);
            power = power * IntInf(this->ibase);
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
            tmp.scale = tmp.scale * IntInf(this->ibase);
			*this += tmp;
		}
	}

    std::string integerToString(IntInf integer) const {
	    std::string integral;
		while (integer > 0) {
            auto remainder = (integer % IntInf(this->obase));
			auto cipher = (remainder.size() != 0) ? remainder.back() : 0;

			if (this->obase <= 16) {
				integral = DIGITS[cipher] + integral;
			}
			else {
				integral = " " + leadingZeros(cipher) + integral;
			}

            integer = integer / IntInf(this->obase);
		}

		return ((this->num.negative) ? "-" : "") + integral;
	}

    std::string fractionToString(IntInf fraction) const {
	    std::string fractPart;
		uint32_t i = 0;
		while (fraction != 0 && i != this->precision) {
            fraction = fraction * IntInf(this->obase);
            IntInf division = fraction / this->scale;
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
