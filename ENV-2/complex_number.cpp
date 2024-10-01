#include "complex_number.h"
#include <iostream>
#include <cmath>

ComplexNumber::ComplexNumber(double number, double i) : number(number), i(i) {}

ComplexNumber ComplexNumber::operator+(const ComplexNumber& value) const {
    return ComplexNumber(number + value.number, i + value.i);
}

ComplexNumber ComplexNumber::operator-(const ComplexNumber& value) const {
    return ComplexNumber(number - value.number, i - value.i);
}

ComplexNumber ComplexNumber::operator*(double value) const {
    return ComplexNumber(number * value, i * value);
}

double ComplexNumber::abs() const {
    return std::sqrt(number * number + i * i);
}

void ComplexNumber::print() const {
    std::cout << "the numer is " << number << " + " << i << "i" << std::endl;
}
