#include "Vector.h"
#include <algorithm>
#include <cmath>

Vector::Vector(const Vector& o) {
    x_ = o.x_;
    y_ = o.y_;
}

Vector::Vector(Vector&& o) {
    x_ = std::move(o.x_);
    y_ = std::move(o.y_);
}

Vector::Vector(const Number& x, const Number& y) {
    x_ = x;
    y_ = y;
}

Vector& Vector::operator=(const Vector& o) {
    x_ = o.x_;
    y_ = o.y_;
    return *this;
}

Vector& Vector::operator=(Vector&& o) {
    x_ = std::move(o.x_);
    y_ = std::move(o.y_);
    return *this;
}

Number Vector::getX() const {
    return x_;
}

Number Vector::getY() const {
    return y_;
}


Number Vector::getPolarAngle() const {
    return createNumber(std::atan2(y_.getDouble(), x_.getDouble()));
}

Number Vector::getPolarRadius() const {
    auto x = x_.getDouble();
    auto y = y_.getDouble();
    return createNumber(std::sqrt(x * x + y * y));
}

Vector Vector::operator+(const Vector& o) const {
    return Vector(x_ + o.x_, y_ + o.y_);
}

Vector Vector::operator-(const Vector& o) const {
    return Vector(x_ - o.x_, y_ - o.y_);
}

bool Vector::operator==(const Vector& oth) const {
    return x_ == oth.x_ && y_ == oth.y_;
}

bool Vector::operator!=(const Vector& oth) const {
    return !(*this == oth);
}

const Vector Vector::NULLV(Number::ZERO, Number::ZERO);
const Vector Vector::IDENTITY(Number::ONE, Number::ONE);
