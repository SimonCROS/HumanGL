//
// Created by loumarti on 1/28/25.
//

#ifndef QUAT_H
#define QUAT_H

#include "ft_glm.h"
#include <cmath>

namespace ft_glm
{
    struct quat
    {
        float x, y, z, w;
        quat() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        quat(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

        explicit quat(const float angle, const vec3& axis) {
            const float halfAngle = angle * 0.5f;
            const float s = std::sin(halfAngle);
            w = std::cos(halfAngle);
            x = axis.x * s;
            y = axis.y * s;
            z = axis.z * s;
        }

    	explicit quat(const float pitch, const float yaw, const float roll) {
        	const vec3 eulerAngle(pitch * 0.5f, yaw * 0.5f, roll * 0.5f);
        	const vec3 c(std::cos(eulerAngle.x), std::cos(eulerAngle.y), std::cos(eulerAngle.z));
        	const vec3 s(std::sin(eulerAngle.x), std::sin(eulerAngle.y), std::sin(eulerAngle.z));

        	this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        	this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        	this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        	this->z = c.x * c.y * s.z - s.x * s.y * c.z;
        }

    };

	inline auto operator<<(std::ostream& os, const quat& q) -> std::ostream&  {
		os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
		return os;
	}
}



#endif //QUAT_H
