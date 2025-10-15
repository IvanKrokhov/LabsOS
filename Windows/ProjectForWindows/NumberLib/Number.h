#pragma once

class Number {
private:
	double num_ = 0;
public:
	Number() = default;
	~Number() = default;
	explicit Number(double);
	Number(const Number&);
	Number(Number&&);

	Number& operator=(const Number&);
	Number& operator=(Number&&);

	Number operator+(const Number&) const;
	Number operator-(const Number&) const;
	Number operator*(const Number&) const;
	Number operator/(const Number&) const;

	bool operator==(const Number&) const;
	bool operator !=(const Number&) const;

	static const Number ZERO;
	static const Number ONE;

	double getDouble() const;
};

Number createNumber(double);
