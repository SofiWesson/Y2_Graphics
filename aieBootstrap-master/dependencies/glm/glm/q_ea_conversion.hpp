#include "geometric.hpp"
#include "trigonometric.hpp"
#include "exponential.hpp"
#include "glm.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

// made by Sofi
namespace glm
{
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> eulerAngles_to_quat(vec<3, T, P> const& v)
	{
        double cy = cos(v.y * 0.5);
        double sy = sin(v.y * 0.5);
        double cp = cos(v.x * 0.5);
        double sp = sin(v.x * 0.5);
        double cr = cos(v.z * 0.5);
        double sr = sin(v.z * 0.5);

        tquat<T, P> q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;

        return q;
	}

    template<typename T, precision P>
    GLM_FUNC_QUALIFIER vec<3, T, P> quat_to_eulerAngles(tquat<T, P> const& q)
    {
        vec<3, T, P> eulerAngles = vec<3, T, P>(0);

        // roll (x-axis rotation)
        double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
        double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
        eulerAngles.z = std::atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        double sinp = 2 * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1)
            eulerAngles.x = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
        else
            eulerAngles.x = std::asin(sinp);

        // yaw (z-axis rotation)
        double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
        double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
        eulerAngles.y = std::atan2(siny_cosp, cosy_cosp);

        return eulerAngles;
    }
}