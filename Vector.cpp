//
// Created by remi on 10/01/25.
//

#include "Vector.h"
#include <cmath>

static double square(double x) { return x * x; }


Vector::Vector(): coord{0, 0, 0} {}
Vector::Vector(double x, double y, double z): coord{x, y, z} {}

std::array<double, 3> Vector::getCoordinates() const { return coord; }

double& Vector::operator[](uint32_t i) { return coord[i]; }
double Vector::operator[](uint32_t i) const { return coord[i]; }

Vector& Vector::operator+=(const Vector& v) {
	*this = *this + v;
	return *this;
}

Vector& Vector::operator/=(double x) {
	*this = *this / x;
	return *this;
}

double Vector::norm2() const {
	return square(coord[0]) + square(coord[1]) + square(coord[2]);
}

double Vector::dot(const Vector& a) const {
	return a[0] * coord[0] + a[1] * coord[1] + a[2] * coord[2];
}

Vector Vector::cross(const Vector& a) const {
	return {
		coord[1] * a[2] - coord[2] * a[1],
		coord[2] * a[0] - coord[0] * a[2],
		coord[0] * a[1] - coord[1] * a[0]
	};
}

Vector Vector::operator*(const Vector& a) const {
	return {coord[0] * a[0], coord[1] * a[1], coord[2] * a[2]};
}

Vector Vector::operator+(const Vector& a) const {
	return {coord[0] + a[0], coord[1] + a[1], coord[2] + a[2]};
}

Vector Vector::operator-(const Vector& a) const {
	return {coord[0] - a[0], coord[1] - a[1], coord[2] - a[2]};
}

Vector Vector::operator-() const {
	return *this * -1.;
}

Vector Vector::operator*(double b) const {
	return {coord[0] * b, coord[1] * b, coord[2] * b};
}

Vector Vector::operator/(double b) const {
	return {coord[0] / b, coord[1] / b, coord[2] / b};
}

Vector Vector::normalized() const {
	double norm = std::sqrt(norm2());
	return {coord[0] / norm, coord[1] / norm, coord[2] / norm};
}

Vector operator*(double a, const Vector& b) {
	return {a * b[0], a * b[1], a * b[2]};
}
