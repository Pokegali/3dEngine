//
// Created by remi on 10/01/25.
//

#ifndef RAY_H
#define RAY_H
#include "Vector.h"


class Ray {
public:
	Ray(const Vector& origin, const Vector& direction);

	Vector origin;
	Vector direction;
};



#endif //RAY_H
