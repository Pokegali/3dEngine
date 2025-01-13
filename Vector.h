//
// Created by remi on 10/01/25.
//

#ifndef VECTOR_H
#define VECTOR_H


class Vector {
public:
	Vector();
	Vector(double x, double y, double z);

	double& operator[](int i);
	double operator[](int i) const;

	Vector& operator+=(const Vector& v);

	[[nodiscard]] double norm2() const;
	[[nodiscard]] double dot(const Vector& a) const;

	Vector operator+(const Vector&) const;
	Vector operator-(const Vector&) const;
	Vector operator*(double) const;

	Vector normalized();

private:
	double coord[3];
};

Vector operator*(double, const Vector&);


#endif //VECTOR_H
