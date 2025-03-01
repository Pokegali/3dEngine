//
// Created by remi on 10/01/25.
//

#include "Vector.h"

#include <cmath>
#include <stdexcept>

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

void Vector::rotate(double angleRad, uint32_t axis) {
	double cos = std::cos(angleRad);
	double sin = std::sin(angleRad);
	auto [vx, vy, vz] = this->coord;
	switch (axis) {
		case 0:
			coord[1] = vy * cos + vz * -sin;
			coord[2] = vy * sin + vz * cos;
			break;
		case 1:
			coord[0] = vx * cos + vz * sin;
			coord[2] = vx * -sin + vz * cos;
			break;
		case 2:
			coord[0] = vx * cos + vy * -sin;
			coord[2] = vx * sin + vy * cos;
			break;
		default:
			throw std::runtime_error("Axis must be 0 (x), 1 (y), or 2 (z).");
	}
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
