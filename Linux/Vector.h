#ifndef LINUX_VECTOR_H
#define LINUX_VECTOR_H

#pragma once

#include "Number.h"

class Vector {
private:
    Number x_;
    Number y_;

public:
    Vector() = default;
    ~Vector() = default;
    Vector(const Vector&);
    Vector(Vector&&);
    Vector(const Number&, const Number&);
    Vector& operator=(const Vector&);
    Vector& operator=(Vector&&);

    Number getX() const;
    Number getY() const;

    Number getPolarAngle() const;
    Number getPolarRadius() const;

    Vector operator+(const Vector&) const;
    Vector operator-(const Vector&) const;

    bool operator==(const Vector&) const;
    bool operator!=(const Vector&) const;

    static const Vector NULLV;
    static const Vector IDENTITY;
};


#endif
