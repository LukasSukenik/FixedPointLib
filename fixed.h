#pragma once

class Fixed {
public:

    Fixed& operator=(Fixed o);

    Fixed operator+(Fixed o);
    Fixed operator-(Fixed o);

    Fixed operator+();
    Fixed operator-();

    Fixed operator++(int); // postfix
    Fixed operator--(int );

    Fixed& operator++(); // prefix
    Fixed& operator--();
};

//komentar
