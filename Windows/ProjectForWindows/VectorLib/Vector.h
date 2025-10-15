#pragma once

#include "../NumberLib/Number.h"

#ifdef VECTORLIB_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

class VECTOR_API Vector {
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
