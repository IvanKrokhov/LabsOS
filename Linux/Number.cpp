#include "Number.h"
#include <algorithm>
#include <stdexcept>

Number::Number(double val) {
    num_ = val;
}

Number::Number(const Number& other) {
    num_ = other.num_;
}

Number::Number(Number&& other) {
    num_ = std::move(other.num_);
}

Number& Number::operator=(const Number& other) {
    num_ = other.num_;
    return *this;
}

Number& Number::operator=(Number&& other) {
    num_ = std::move(other.num_);
    return *this;
}

Number Number::operator+(const Number& other) const {
    return Number(num_ + other.num_);
}
Number Number::operator-(const Number& other) const {
    return Number(num_ - other.num_);
}

Number Number::operator*(const Number& other) const {
    return Number(num_ * other.num_);
}
Number Number::operator/(const Number&other) const {
    if (other.num_ == 0) {
        throw std::invalid_argument("You can not devide by zero!");
    }
    return Number(num_ / other.num_);
}

bool Number::operator==(const Number& other) const {
    return num_ == other.num_;
}

bool Number::operator!=(const Number& other) const {
    return !(*this == other);
}

const Number Number::ZERO(0.);
const Number Number::ONE(1.);

double Number::getDouble() const {
    return num_;
}

Number createNumber(double num) {
    return Number(num);
}
