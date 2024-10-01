#include <iostream>
#include <vector>
#include "complex_number.h"
#include "sort.h"

int main() {
    std::vector<ComplexNumber> numbers = {
        ComplexNumber(1, 4),
        ComplexNumber(112, 13),
        ComplexNumber(0, 2),
        ComplexNumber(58, 172)
    };

    std::cout << "before sort:" << std::endl;
    for (const auto& n : numbers) {
        n.print();
    }

    sortComplexNumbers(numbers);

    std::cout << "\nafter sort:" << std::endl;
    for (const auto& n : numbers) {
        n.print();
    }

    return 0;
}
