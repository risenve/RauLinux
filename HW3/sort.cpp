#include "sort.h"
#include <algorithm>

bool compareByAbs(const ComplexNumber& a, const ComplexNumber& b) {
    return a.abs() < b.abs();
}

void sortComplexNumbers(std::vector<ComplexNumber>& numbers) {
    std::sort(numbers.begin(), numbers.end(), compareByAbs);
}
