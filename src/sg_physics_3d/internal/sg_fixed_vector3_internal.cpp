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

#include "sg_fixed_vector3_internal.h"

const fixed SGFixedVector3Internal::FIXED_UNIT_EPSILON = fixed(65);

const SGFixedVector3Internal SGFixedVector3Internal::ZERO = SGFixedVector3Internal(fixed::ZERO, fixed::ZERO, fixed::ZERO);

bool SGFixedVector3Internal::operator==(const SGFixedVector3Internal &p_v) const {
	return x == p_v.x && y == p_v.y && z == p_v.z;
}

bool SGFixedVector3Internal::operator!=(const SGFixedVector3Internal &p_v) const {
	return x != p_v.x || y != p_v.y || z != p_v.z;
}

fixed SGFixedVector3Internal::angle() const {
	return y.atan2(x);
}

SGFixedVector3Internal SGFixedVector3Internal::rotated(fixed p_rotation) const {
	SGFixedVector3Internal v;
	v.set_rotation(angle() + p_rotation);
	v *= length();
	return v;
}

void SGFixedVector3Internal::normalize() {
	// For values less than 2048 we get really imprecise results. Since only
	// direction matters, we can increase the vector's magnitude.
	fixed x_abs = x.abs();
	fixed y_abs = y.abs();
	fixed z_abs = z.abs();
	if ((x.value != 0 && x_abs.value < 2048) || (y.value != 0 && y_abs.value < 2048) || (z.value != 0 && z_abs.value < 2048)) {
		// Watch out for values that will overflow even 64 bits.
		// 1482910 = sqrt(MAX_SIGNED_64BIT_NUMBER) / 2048
		if (x_abs.value >= 1482910) {
			x = x.value > 0 ? fixed::ONE : fixed::NEG_ONE;
			y = fixed::ZERO;
			z = fixed::ZERO;
		}
		else if (y_abs.value >= 1482910) {
			x = fixed::ZERO;
			y = y.value > 0 ? fixed::ONE : fixed::NEG_ONE;
			z = fixed::ZERO;
		}
		else if (z_abs.value >= 1482910) {
			x = fixed::ZERO;
			y = fixed::ZERO;
			z = z.value > 0 ? fixed::ONE : fixed::NEG_ONE;
		}
		else {
			// Multiply X and Y by 2048.
			fixed x_big = fixed(x.value << 11);
			fixed y_big = fixed(y.value << 11);
			fixed z_big = fixed(z.value << 11);
			fixed l = SGFixedVector3Internal(x_big, y_big, z_big).length();
			if (l != fixed::ZERO) {
				x = x_big / l;
				y = y_big / l;
				z = z_big / l;
			}
		}
	}
	else {
		fixed l = length();
		if (l != fixed::ZERO) {
			x /= l;
			y /= l;
			z /= l;
		}
	}
}

SGFixedVector3Internal SGFixedVector3Internal::normalized() const {
	SGFixedVector3Internal v = *this;
	v.normalize();
	return v;
}

bool SGFixedVector3Internal::is_normalized() const {
	return fixed::is_equal_approx(length_squared(), fixed::ONE, SGFixedVector3Internal::FIXED_UNIT_EPSILON);
}

SGFixedVector3Internal SGFixedVector3Internal::safe_scale(const SGFixedVector3Internal &p_other) const {
	SGFixedVector3Internal ret = *this * p_other;

	// Don't allow scaling all the way down to zero.
	if (ret.x == fixed::ZERO && x != fixed::ZERO) {
		ret.x.value = x > fixed::ZERO ? 1 : -1;
	}
	if (ret.y == fixed::ZERO && y != fixed::ZERO) {
		ret.y.value = y > fixed::ZERO ? 1 : -1;
	}
	if (ret.z == fixed::ZERO && z != fixed::ZERO) {
		ret.z.value = z > fixed::ZERO ? 1 : -1;
	}

	return ret;
}

SGFixedVector3Internal SGFixedVector3Internal::safe_scale(fixed p_scale) const {
	return safe_scale(SGFixedVector3Internal(p_scale, p_scale, p_scale));
}

fixed SGFixedVector3Internal::length() const {
	// By directly using 64-bit integers we can avoid a left shift, since
	// multiplying two fixed point numbers effectively shifts them left.
	int64_t length_squared = x.value * x.value + y.value * y.value + z.value * z.value;
	if (length_squared == 0) {
		return fixed::ZERO;
	}
	fixed length = fixed(sg_sqrt_64(length_squared));
	if (length == fixed::ZERO) {
		// If the vector was non-zero, then we must return some length.
		return fixed(1);
	}
	return length;
}

fixed SGFixedVector3Internal::length_squared() const {
	fixed ret = x * x + y * y + z * z;
	// Squaring a fixed-point number smaller than 15 will be 0, which means
	// that it's possible for ret to equal 0.
	if (ret == fixed::ZERO && (x != fixed::ZERO || y != fixed::ZERO || z != fixed::ZERO)) {
		// If the vector was non-zero, then we must return some length.
		return fixed(1);
	}
	return ret;
}

fixed SGFixedVector3Internal::distance_to(const SGFixedVector3Internal &p_other) const {
	SGFixedVector3Internal vec = p_other - *this;
	return vec.length();
}

fixed SGFixedVector3Internal::distance_squared_to(const SGFixedVector3Internal &p_other) const {
	SGFixedVector3Internal vec = p_other - *this;
	return vec.length_squared();
}

fixed SGFixedVector3Internal::angle_to(const SGFixedVector3Internal &p_other) const {
	return cross(p_other).atan2(dot(p_other));
}

fixed SGFixedVector3Internal::angle_to_point(const SGFixedVector3Internal &p_other) const {
	return (y - p_other.y).atan2(x - p_other.x);
}

fixed SGFixedVector3Internal::dot(const SGFixedVector3Internal &p_other) const {
	return x * p_other.x + y * p_other.y + z * p_other.z;
}

fixed SGFixedVector3Internal::cross(const SGFixedVector3Internal &p_other) const {
	// c.x = (a.y * b.z) - (a.z * b.y)
	// c.y = (a.z * b.x) - (a.x * b.z)
	// c.z = (a.x * b.y) - (a.y * b.x)
	return x * p_other.y - y * p_other.x; // TODO: https://docs.godotengine.org/en/latest/tutorials/math/vector_math.html#cross-product
}

/*
fixed SGFixedVector3Internal::posmod(const fixed p_mod) const {

}

fixed SGFixedVector3Internal::posmodv(const SGFixedVector3Internal &p_modv) const {

}

SGFixedVector3Internal SGFixedVector3Internal::project(const SGFixedVector3Internal &p_to) const {
	return p_to * (dot(p_to) / p_to.length_squared());
}
*/

SGFixedVector3Internal SGFixedVector3Internal::slide(const SGFixedVector3Internal &p_normal) const {
#ifdef MATH_CHECKS
	ERR_FAIL_COND_V_MSG(!p_normal.is_normalized(), SGFixedVector3Internal(), "The normal SGFixedVector3Internal must be normalized.");
#endif
	return *this - p_normal * this->dot(p_normal);
}

SGFixedVector3Internal SGFixedVector3Internal::bounce(const SGFixedVector3Internal &p_normal) const {
	return -reflect(p_normal);
}

SGFixedVector3Internal SGFixedVector3Internal::reflect(const SGFixedVector3Internal &p_normal) const {
#ifdef MATH_CHECKS
	ERR_FAIL_COND_V_MSG(!p_normal.is_normalized(), SGFixedVector3Internal(), "The normal SGFixedVector3Internal must be normalized.");
#endif
	return p_normal * fixed::TWO * this->dot(p_normal) - *this;
}

bool SGFixedVector3Internal::is_equal_approx(const SGFixedVector3Internal &p_v) const {
	return fixed::is_equal_approx(x, p_v.x) && fixed::is_equal_approx(y, p_v.y);
}

SGFixedVector3Internal SGFixedVector3Internal::cubic_interpolate(const SGFixedVector3Internal& p_b, const SGFixedVector3Internal& p_pre_a, const SGFixedVector3Internal& p_post_b, fixed p_weight) const {
	SGFixedVector3Internal p0 = p_pre_a;
	SGFixedVector3Internal p1 = *this;
	SGFixedVector3Internal p2 = p_b;
	SGFixedVector3Internal p3 = p_post_b;

	fixed t = p_weight;
	fixed t2 = t * t;
	fixed t3 = t2 * t;

	// from original floating point compute:
	//out = 0.5 *
	//	((p1 * 2.0) +
	//		(-p0 + p2) * t +
	//		(2.0 * p0 - 5.0 * p1 + 4 * p2 - p3) * t2 +
	//		(-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3);
	SGFixedVector3Internal out = ((p1 * fixed(131072)) +
			(-p0 + p2) * t +
			((p0 * fixed(131072)) - p1 * fixed(327680) + (p2 * fixed(262144)) - p3) * t2 +
			(-p0 + p1 * fixed(196608) - p2 * fixed(196608) + p3) * t3) * fixed(32768);
	return out;
}

SGFixedVector3Internal SGFixedVector3Internal::get_closest_point_to_segment_3D(const SGFixedVector3Internal &p_point, const SGFixedVector3Internal *p_segment) {
	SGFixedVector3Internal p = p_point - p_segment[0];
	SGFixedVector3Internal n = p_segment[1] - p_segment[0];
	fixed l2 = n.length();
	if (l2 == fixed::ZERO) {
		return p_segment[0]; // Both points are the same, just give any.
	}

	fixed d = (n / l2).dot(p / l2);

	if (d <= fixed::ZERO) {
		return p_segment[0]; // Before first point.
	} else if (d >= fixed::ONE) {
		return p_segment[1]; // After first point.
	} else {
		return p_segment[0] + n * d; // Inside.
	}
}