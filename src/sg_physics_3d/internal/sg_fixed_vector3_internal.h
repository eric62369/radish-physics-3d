/*************************************************************************/
/* Copyright (c) 2021-2022 David Snopek                                  */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef SG_FIXED_VECTOR3_INTERNAL_H
#define SG_FIXED_VECTOR3_INTERNAL_H

#include "sg_fixed_number_internal.h"

struct SGFixedVector3Internal {
	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {
		fixed x;
		fixed width;
	};
	union {
		fixed y;
		fixed height;
	};
	union {
		fixed z;
		fixed depth;
	};

	static const fixed FIXED_UNIT_EPSILON;
	static const SGFixedVector3Internal ZERO;

	_FORCE_INLINE_ fixed &operator[](int p_idx) {
		if (p_idx == 2) {
			return z;
		}
		else if (p_idx == 1) {
			return y;
		}
		else {
			return x;
		}
	}
	_FORCE_INLINE_ const fixed &operator[](int p_idx) const {
		if (p_idx == 2) {
			return z;
		}
		else if (p_idx == 1) {
			return y;
		}
		else {
			return x;
		}
	}

	_FORCE_INLINE_ SGFixedVector3Internal operator+(const SGFixedVector3Internal &p_v) const {
		return SGFixedVector3Internal(x + p_v.x, y + p_v.y, z + p_v.z);
	}
	_FORCE_INLINE_ void operator+=(const SGFixedVector3Internal &p_v) {
		x += p_v.x;
		y += p_v.y;
		z += p_v.z;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator-(const SGFixedVector3Internal &p_v) const {
		return SGFixedVector3Internal(x - p_v.x, y - p_v.y, z - p_v.z);
	}
	_FORCE_INLINE_ void operator-=(const SGFixedVector3Internal &p_v) {
		x -= p_v.x;
		y -= p_v.y;
		z -= p_v.z;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator*(const SGFixedVector3Internal &p_v) const {
		return SGFixedVector3Internal(x * p_v.x, y * p_v.y, z * p_v.z);
	}
	_FORCE_INLINE_ void operator*=(const SGFixedVector3Internal &p_v) {
		x *= p_v.x;
		y *= p_v.y;
		z *= p_v.z;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator/(const SGFixedVector3Internal &p_v) const {
		return SGFixedVector3Internal(x / p_v.x, y / p_v.y, z / p_v.z);
	}
	_FORCE_INLINE_ void operator/=(const SGFixedVector3Internal &p_v) {
		x /= p_v.x;
		y /= p_v.y;
		z /= p_v.z;
	}

	_FORCE_INLINE_ SGFixedVector3Internal operator+(const fixed &p_v) const {
		return SGFixedVector3Internal(x + p_v, y + p_v, z + p_v);
	}
	_FORCE_INLINE_ void operator+=(const fixed &p_v) {
		x += p_v;
		y += p_v;
		z += p_v;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator-(const fixed &p_v) const {
		return SGFixedVector3Internal(x - p_v, y - p_v, z - p_v);
	}
	_FORCE_INLINE_ void operator-=(const fixed &p_v) {
		x -= p_v;
		y -= p_v;
		z -= p_v;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator*(const fixed &p_v) const {
		return SGFixedVector3Internal(x * p_v, y * p_v, z * p_v);
	}
	_FORCE_INLINE_ void operator*=(const fixed &p_v) {
		x *= p_v;
		y *= p_v;
		z *= p_v;
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator/(const fixed &p_v) const {
		return SGFixedVector3Internal(x / p_v, y / p_v, z / p_v);
	}
	_FORCE_INLINE_ void operator/=(const fixed &p_v) {
		x /= p_v;
		y /= p_v;
		z /= p_v;
	}

	bool operator==(const SGFixedVector3Internal &p_v) const;
	bool operator!=(const SGFixedVector3Internal &p_v) const;

	fixed angle() const;

	void set_rotation(fixed p_radians) {
		x = p_radians.cos();
		// y = p_radians.sin(); // TODO: set rotations and operators above
		z = p_radians.cos(); // TODO: fix set rotation for around y axis only
	}

	_FORCE_INLINE_ SGFixedVector3Internal abs() const {
		return SGFixedVector3Internal(x.abs(), y.abs(), z.abs());
	}
	_FORCE_INLINE_ SGFixedVector3Internal operator-() const {
		return SGFixedVector3Internal(-x, -y, -z);
	}

	SGFixedVector3Internal rotated(fixed p_rotation) const;

	void normalize();
	SGFixedVector3Internal normalized() const;
	bool is_normalized() const;

	SGFixedVector3Internal safe_scale(const SGFixedVector3Internal &p_other) const;
	SGFixedVector3Internal safe_scale(fixed p_scale) const;

	fixed length() const;
	fixed length_squared() const;

	fixed distance_to(const SGFixedVector3Internal &p_other) const;
	fixed distance_squared_to(const SGFixedVector3Internal &p_other) const;
	fixed angle_to(const SGFixedVector3Internal &p_other) const;
	fixed angle_to_point(const SGFixedVector3Internal &p_other) const;
	_FORCE_INLINE_ SGFixedVector3Internal direction_to(const SGFixedVector3Internal &p_to) const;

	fixed dot(const SGFixedVector3Internal &p_other) const;
	fixed cross(const SGFixedVector3Internal &p_other) const;
	//fixed posmod(const fixed p_mod) const;
	//fixed posmodv(const SGFixedVector3Internal &p_modv) const;
	//SGFixedVector3Internal project(const SGFixedVector3Internal &p_to) const;

	// _FORCE_INLINE_ SGFixedVector3Internal tangent() const {
	// 	return SGFixedVector3Internal(y, -x);
	// }

	_FORCE_INLINE_ static SGFixedVector3Internal linear_interpolate(const SGFixedVector3Internal &p_a, const SGFixedVector3Internal &p_b, fixed p_weight);

	SGFixedVector3Internal slide(const SGFixedVector3Internal &p_normal) const;
	SGFixedVector3Internal bounce(const SGFixedVector3Internal &p_normal) const;
	SGFixedVector3Internal reflect(const SGFixedVector3Internal &p_normal) const;

	bool is_equal_approx(const SGFixedVector3Internal &p_v) const;

	_FORCE_INLINE_ SGFixedVector3Internal(fixed p_x, fixed p_y, fixed p_z)
		: x(p_x), y(p_y), z(p_z) {}
	_FORCE_INLINE_ SGFixedVector3Internal()
		: x(fixed::ZERO), y(fixed::ZERO), z(fixed::ZERO) {}

	SGFixedVector3Internal cubic_interpolate(const SGFixedVector3Internal& p_b, const SGFixedVector3Internal& p_pre_a, const SGFixedVector3Internal& p_post_b, fixed p_weight) const;

	static SGFixedVector3Internal get_closest_point_to_segment_3D(const SGFixedVector3Internal &p_point, const SGFixedVector3Internal *p_segment);
};

SGFixedVector3Internal SGFixedVector3Internal::direction_to(const SGFixedVector3Internal &p_to) const {
	SGFixedVector3Internal ret(p_to.x - x, p_to.y - y, p_to.z - z);
	ret.normalize();
	return ret;
}

SGFixedVector3Internal SGFixedVector3Internal::linear_interpolate(const SGFixedVector3Internal &p_a, const SGFixedVector3Internal &p_b, fixed p_weight) {
	SGFixedVector3Internal res = p_a;
	res.x += (p_weight * (p_b.x - p_a.x));
	res.y += (p_weight * (p_b.y - p_a.y));
	res.z += (p_weight * (p_b.z - p_a.z));
	return res;
}

#endif
