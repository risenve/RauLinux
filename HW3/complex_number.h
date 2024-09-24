#ifndef COMPLEX_NUMBER_H
#define COMPLEX_NUMBER_H

class ComplexNumber {
public:
    ComplexNumber(double number  = 0.0, double i = 0.0);
    
    ComplexNumber operator+(const ComplexNumber& value) const;
    ComplexNumber operator-(const ComplexNumber& value) const;
    ComplexNumber operator*(double value) const;
    
    double abs() const;
    
    void print() const;
    
private:
    double number;
    double i;
};

#endif // COMPLEX_NUMBER_H
