//
// Created by remi on 10/01/25.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <cstdint>


class Vector {
public:
	Vector();
	Vector(double x, double y, double z);

	[[nodiscard]] std::array<double, 3> getCoordinates() const;

	double& operator[](uint32_t i);
	double operator[](uint32_t i) const;

	Vector& operator+=(const Vector& v);
	Vector& operator/=(double);

	void rotate(double angleRad, uint32_t axis);

	[[nodiscard]] double norm2() const;
	[[nodiscard]] double dot(const Vector&) const;
	[[nodiscard]] Vector cross(const Vector&) const;
	Vector operator*(const Vector&) const;

	Vector operator+(const Vector&) const;
	Vector operator-(const Vector&) const;
	Vector operator-() const;
	Vector operator*(double) const;
	Vector operator/(double) const;

	[[nodiscard]] Vector normalized() const;

private:
	std::array<double, 3> coord;
};

Vector operator*(double, const Vector&);

#define vec111 Vector(1, 1, 1)

#endif //VECTOR_H
