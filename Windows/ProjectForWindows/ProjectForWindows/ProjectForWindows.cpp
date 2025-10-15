#include <iostream>

#include "../VectorLib/Vector.h"
#include "../NumberLib/Number.h"

int main()
{
    std::cout << "NUMBERS\n";
    Number a(1.0), b(2.0);
    std::cout << a.getDouble() << "\n";
    Number c(b);
    c = a;
    Number d(c);
    d = std::move(b);
    std::cout << d.getDouble() << "\t" << b.getDouble() << "\n";
    std::cout << (c + d).getDouble() << "\t" << (c - d).getDouble() << "\n";
    if (Number::ZERO == b) {
        std::cout << "b is clear";
    }
    if (Number::ONE != a) {
        std::cout << "a is not one";
    }
    std::cout << "VECTORS!\n";
    Vector v1(Number(1.0), Number(2.0));
    Vector v2(Number(3.0), Number(4.0));

    std::cout << "v1:\t" << v1.getX().getDouble() << "\t " << v1.getY().getDouble() << "\n";
    std::cout << "v2:\t" << v2.getX().getDouble() << "\t " << v2.getY().getDouble() << "\n";

    Vector v3(v1);
    v3 = v2;
    Vector v4(v3);
    v4 = std::move(v1);

    std::cout << "v4:\t" << v4.getX().getDouble() << "\t" << v4.getY().getDouble() << "\n";
    std::cout << "v1:\t" << v1.getX().getDouble() << "\t" << v1.getY().getDouble() << "\n";

    Vector sum = v3 + v4;
    Vector diff = v3 - v4;

    std::cout << "v3 + v4:\t" << sum.getX().getDouble() << "\t" << sum.getY().getDouble() << "\n";
    std::cout << "v3 - v4:\t" << diff.getX().getDouble() << "\t" << diff.getY().getDouble() << "\n";

    std::cout << "v3 radius: " << v3.getPolarRadius().getDouble() << "\n";
    std::cout << "v3 angle: " << v3.getPolarAngle().getDouble() << "\n";
    std::cout << "v4 radius: " << v4.getPolarRadius().getDouble() << "\n";
    std::cout << "v4 angle: " << v4.getPolarAngle().getDouble() << "\n";

    if (Vector::NULLV == v1) {
        std::cout << "v1 is NULL\n";
    }
    if (Vector::IDENTITY != v2) {
        std::cout << "v2 is not IDENTITY\n";
    }

    if (v3 == v4) {
        std::cout << "v3 == v4\n";
    }
    if (v3 != v2) {
        std::cout << "v3 != v2\n";
    }
}