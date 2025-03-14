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

#include "sg_fixed_transform_3D_internal.h"

void SGFixedTransform3DInternal::invert() {
	SWAP(elements[0][1], elements[1][0]);
	SWAP(elements[0][2], elements[2][0]);
	SWAP(elements[1][2], elements[2][1]);
	elements[3] = basis_xform(-elements[3]);
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::inverse() const {
	SGFixedTransform3DInternal inv = *this;
	inv.invert();
	return inv;
}

void SGFixedTransform3DInternal::affine_invert() {
	fixed det = basis_determinant();
#ifdef MATH_CHECKS
	ERR_FAIL_COND(det == fixed::ZERO);
#endif
	SWAP(elements[0][0], elements[2][2]);
	SWAP(elements[0][1], elements[2][1]);
	SWAP(elements[1][0], elements[1][2]);
	elements[0] /= SGFixedVector3Internal(det, -det, -det); // TODO: determinant 3D vector math please 
	elements[1] /= SGFixedVector3Internal(-det, det, -det);
	elements[2] /= SGFixedVector3Internal(-det, -det, det);

	elements[3] = basis_xform(-elements[3]);
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::affine_inverse() const {
	SGFixedTransform3DInternal inv = *this;
	inv.affine_invert();
	return inv;
}

void SGFixedTransform3DInternal::rotate(fixed p_phi) {
	SGFixedVector3Internal scale = get_scale();
	*this = SGFixedTransform3DInternal(p_phi, SGFixedVector3Internal()) * (*this);
	set_scale(scale);
}

fixed SGFixedTransform3DInternal::get_rotation() const {
	return elements[0].z.atan2(elements[0].x);
}

void SGFixedTransform3DInternal::set_rotation(fixed p_rot) {
	SGFixedVector3Internal scale = get_scale();
	fixed cr = p_rot.cos();
	fixed sr = p_rot.sin();
	elements[0][0] = cr;
	elements[0][2] = sr;
	elements[1][1] = fixed::ONE;
	elements[2][0] = -sr;
	elements[2][2] = cr;
	set_scale(scale);
}

SGFixedTransform3DInternal::SGFixedTransform3DInternal(fixed p_rot, const SGFixedVector3Internal &p_pos) {
	fixed cr = p_rot.cos();
	fixed sr = p_rot.sin();
	elements[0][0] = cr;
	elements[0][2] = sr;
	elements[1][1] = fixed::ONE;
	elements[2][0] = -sr;
	elements[2][2] = cr;
	elements[3] = p_pos;
}

SGFixedVector3Internal SGFixedTransform3DInternal::get_scale() const {
	fixed det_sign = FIXED_SGN(basis_determinant());
	return SGFixedVector3Internal(elements[0].length(), det_sign * elements[1].length(), det_sign * elements[2].length()); // TODO: determinant vector math
}

void SGFixedTransform3DInternal::set_scale(const SGFixedVector3Internal &p_scale) {
	elements[0].normalize();
	elements[1].normalize();
	elements[2].normalize();

	// If scale is very nearly 1, then we just trust normalize() to do its magic.
	if (!fixed::is_equal_approx(p_scale.x, fixed::ONE, SGFixedVector3Internal::FIXED_UNIT_EPSILON)) {
		elements[0] = elements[0].safe_scale(p_scale.x);
	}
	if (!fixed::is_equal_approx(p_scale.y, fixed::ONE, SGFixedVector3Internal::FIXED_UNIT_EPSILON)) {
		elements[1] = elements[1].safe_scale(p_scale.y);
	}
	if (!fixed::is_equal_approx(p_scale.y, fixed::ONE, SGFixedVector3Internal::FIXED_UNIT_EPSILON)) {
		elements[2] = elements[2].safe_scale(p_scale.z);
	}
}

void SGFixedTransform3DInternal::scale(const SGFixedVector3Internal &p_scale) {
	scale_basis(p_scale);
	elements[3] *= p_scale;
}

void SGFixedTransform3DInternal::scale_basis(const SGFixedVector3Internal &p_scale) {
	elements[0] = elements[0].safe_scale(p_scale);
	elements[1] = elements[1].safe_scale(p_scale);
	elements[2] = elements[2].safe_scale(p_scale);
}

void SGFixedTransform3DInternal::translate(fixed p_tx, fixed p_ty, fixed p_tz) {
	translate(SGFixedVector3Internal(p_tx, p_ty, p_tz));
}

void SGFixedTransform3DInternal::translate(const SGFixedVector3Internal &p_translation) {
	elements[3] += basis_xform(p_translation);
}

void SGFixedTransform3DInternal::orthonormalize() {
	// Gram-Schmidt Process
	SGFixedVector3Internal x = elements[0];
	SGFixedVector3Internal y = elements[1];
	SGFixedVector3Internal z = elements[2];

	x.normalized();
	y = (y - x * (x.dot(y)));
	y.normalize();
	z = (z - x * (x.dot(z)) - y * (y.dot(z)));
	z.normalize();

	elements[0] = x;
	elements[1] = y;
	elements[2] = z;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::orthonormalized() const {
	SGFixedTransform3DInternal t = *this;
	t.orthonormalize();
	return t;
}

bool SGFixedTransform3DInternal::is_equal_approx(const SGFixedTransform3DInternal &p_transform) const {
	return elements[0].is_equal_approx(p_transform.elements[0]) && elements[1].is_equal_approx(p_transform.elements[1]) && elements[2].is_equal_approx(p_transform.elements[2]);
}

bool SGFixedTransform3DInternal::operator==(const SGFixedTransform3DInternal &p_transform) const {
	for (int i = 0; i < 4; i++) {
		if (elements[i] != p_transform.elements[i]) {
			return false;
		}
	}
	return true;
}

bool SGFixedTransform3DInternal::operator!=(const SGFixedTransform3DInternal &p_transform) const {
	for (int i = 0; i < 4; i++) {
		if (elements[i] != p_transform.elements[i]) {
			return true;
		}
	}
	return false;
}

void SGFixedTransform3DInternal::operator*=(const SGFixedTransform3DInternal &p_transform) {
	elements[3] = xform(p_transform.elements[3]);

	fixed x0, x1, x2, y0, y1, y2, z0, z1, z2;

	x0 = tdotx(p_transform.elements[0]);
	x1 = tdoty(p_transform.elements[0]);
	x2 = tdotz(p_transform.elements[0]);
	y0 = tdotx(p_transform.elements[1]);
	y1 = tdoty(p_transform.elements[1]);
	y2 = tdotz(p_transform.elements[1]);
	z0 = tdotx(p_transform.elements[2]);
	z1 = tdoty(p_transform.elements[2]);
	z2 = tdotz(p_transform.elements[2]);

	elements[0][0] = x0;
	elements[0][1] = x1;
	elements[0][2] = x2;
	elements[1][0] = y0;
	elements[1][1] = y1;
	elements[1][2] = y2;
	elements[2][0] = z0;
	elements[2][1] = z1;
	elements[2][2] = z2;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::operator*(const SGFixedTransform3DInternal &p_transform) const {
	SGFixedTransform3DInternal t = *this;
	t *= p_transform;
	return t;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::scaled(const SGFixedVector3Internal &p_scale) const {
	SGFixedTransform3DInternal t = *this;
	t.scale(p_scale);
	return t;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::basis_scaled(const SGFixedVector3Internal &p_scale) const {
	SGFixedTransform3DInternal t = *this;
	t.scale_basis(p_scale);
	return t;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::translated(const SGFixedVector3Internal &p_offset) const {
	SGFixedTransform3DInternal t = *this;
	t.translate(p_offset);
	return t;
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::rotated(fixed p_phi) const {
	SGFixedTransform3DInternal t = *this;
	t.rotate(p_phi);
	return t;
}

fixed SGFixedTransform3DInternal::basis_determinant() const {
	 // 3D determinant: https://en.wikipedia.org/wiki/Determinant
	return (elements[0].x * elements[1].y * elements[2].z) + (elements[1].x * elements[2].y * elements[0].z) + (elements[2].x * elements[0].y * elements[1].z) - (elements[2].x * elements[1].y * elements[0].z) - (elements[1].x * elements[0].y * elements[2].z) - (elements[0].x * elements[2].y * elements[1].z);
}

SGFixedTransform3DInternal SGFixedTransform3DInternal::interpolate_with(const SGFixedTransform3DInternal &p_transform, fixed p_c) const {
	SGFixedVector3Internal p1 = get_origin();
	SGFixedVector3Internal p2 = p_transform.get_origin();

	fixed r1 = get_rotation();
	fixed r2 = p_transform.get_rotation();

	SGFixedVector3Internal s1 = get_scale();
	SGFixedVector3Internal s2 = p_transform.get_scale();

	SGFixedVector3Internal v1(r1.cos(), r1.sin(), r1.sin()); // TODO: whats the z axis in this case??
	SGFixedVector3Internal v2(r2.cos(), r2.sin(), r1.sin());

	fixed dot = v1.dot(v2);
	dot = CLAMP(dot, fixed::NEG_ONE, fixed::ONE);

	SGFixedVector3Internal v;

	// 65500 = ~0.9995
	if (dot > fixed(65500)) {
		v = SGFixedVector3Internal::linear_interpolate(v1, v2, p_c).normalized();
	}
	else {
		fixed angle = p_c * dot.acos();
		SGFixedVector3Internal v3 = (v2 - v1 * dot).normalized();
		v = v1 * angle.cos() + v3 * angle.sin();
	}

	SGFixedTransform3DInternal res(v.y.atan2(v.x), SGFixedVector3Internal::linear_interpolate(p1, p2, p_c));
	res.scale_basis(SGFixedVector3Internal::linear_interpolate(s1, s2, p_c));
	return res;
}
