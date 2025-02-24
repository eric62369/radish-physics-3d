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

#ifndef SG_FIXED_TRANSFORM_3D_INTERNAL_H
#define SG_FIXED_TRANSFORM_3D_INTERNAL_H

#include "sg_fixed_vector3_internal.h"

struct SGFixedTransform3DInternal {
	SGFixedVector3Internal elements[3];

	_FORCE_INLINE_ fixed tdotx(const SGFixedVector3Internal &v) const { return elements[0][0] * v.x + elements[1][0] * v.y; }
	_FORCE_INLINE_ fixed tdoty(const SGFixedVector3Internal &v) const { return elements[0][1] * v.x + elements[1][1] * v.y; }

	const SGFixedVector3Internal &operator[](int p_idx) const { return elements[p_idx]; }
	SGFixedVector3Internal &operator[](int p_idx) { return elements[p_idx]; }

	_FORCE_INLINE_ SGFixedVector3Internal get_axis(int p_axis) const {
		ERR_FAIL_INDEX_V(p_axis, 3, SGFixedVector3Internal());
		return elements[p_axis];
	}
	_FORCE_INLINE_ void set_axis(int p_axis, const SGFixedVector3Internal &p_vec) {
		ERR_FAIL_INDEX(p_axis, 3);
		elements[p_axis] = p_vec;
	}

	void invert();
	SGFixedTransform3DInternal inverse() const;

	void affine_invert();
	SGFixedTransform3DInternal affine_inverse() const;

	void set_rotation(fixed p_rot);
	fixed get_rotation() const;
	_FORCE_INLINE_ void set_rotation_and_scale(fixed p_rot, const SGFixedVector3Internal &p_scale);
	void rotate(fixed p_phi);

	void scale(const SGFixedVector3Internal &p_scale);
	void scale_basis(const SGFixedVector3Internal &p_scale);
	void translate(fixed p_tx, fixed p_ty);
	void translate(const SGFixedVector3Internal &p_translation);

	fixed basis_determinant() const;

	SGFixedVector3Internal get_scale() const;
	void set_scale(const SGFixedVector3Internal &p_scale);

	_FORCE_INLINE_ const SGFixedVector3Internal &get_origin() const { return elements[2]; }
	_FORCE_INLINE_ void set_origin(const SGFixedVector3Internal &p_origin) { elements[2] = p_origin; }

	SGFixedTransform3DInternal scaled(const SGFixedVector3Internal &p_scale) const;
	SGFixedTransform3DInternal basis_scaled(const SGFixedVector3Internal &p_scale) const;
	SGFixedTransform3DInternal translated(const SGFixedVector3Internal &p_offset) const;
	SGFixedTransform3DInternal rotated(fixed p_phi) const;

	SGFixedTransform3DInternal untranslated() const;

	void orthonormalize();
	SGFixedTransform3DInternal orthonormalized() const;
	bool is_equal_approx(const SGFixedTransform3DInternal &p_transform) const;

	bool operator==(const SGFixedTransform3DInternal &p_transform) const;
	bool operator!=(const SGFixedTransform3DInternal &p_transform) const;

	void operator*=(const SGFixedTransform3DInternal &p_transform);
	SGFixedTransform3DInternal operator*(const SGFixedTransform3DInternal &p_transform) const;

	SGFixedTransform3DInternal interpolate_with(const SGFixedTransform3DInternal &p_transform, fixed p_c) const;

	_FORCE_INLINE_ SGFixedVector3Internal basis_xform(const SGFixedVector3Internal &p_vec) const;
	_FORCE_INLINE_ SGFixedVector3Internal basis_xform_inv(const SGFixedVector3Internal &p_vec) const;
	_FORCE_INLINE_ SGFixedVector3Internal xform(const SGFixedVector3Internal &p_vec) const;
	_FORCE_INLINE_ SGFixedVector3Internal xform_inv(const SGFixedVector3Internal &p_vec) const;
	//_FORCE_INLINE_ SGFixedRect3Internal xform(const SGFixedRect3Internal &p_rect) const;
	//_FORCE_INLINE_ SGFixedRect3Internal xform_inv(const SGFixedRect3Internal &p_rect) const;

	SGFixedTransform3DInternal(fixed xx, fixed xy, fixed xz, fixed yx, fixed yy, fixed yz, fixed zx, fixed zy, fixed zz, fixed ox, fixed oy, fixed oz) {
		elements[0][0] = xx;
		elements[0][1] = xy;
		elements[0][2] = xz;
		elements[1][0] = yx;
		elements[1][1] = yy;
		elements[1][2] = yz;
		elements[2][0] = zx;
		elements[2][1] = zy;
		elements[2][2] = zz;
		elements[3][0] = ox;
		elements[3][1] = oy;
		elements[3][2] = oz;
	}

	SGFixedTransform3DInternal(fixed p_rot, const SGFixedVector3Internal &p_pos);
	SGFixedTransform3DInternal() {
		elements[0][0] = fixed::ONE;
		elements[1][1] = fixed::ONE;
		elements[2][2] = fixed::ONE;
	}
	/*
	SGFixedTransform3DInternal(const SGFixedTransform3DInternal& p_other) {
		memcpy(&elements, &p_other.elements, sizeof(elements));
	}
	*/
};

SGFixedVector3Internal SGFixedTransform3DInternal::basis_xform(const SGFixedVector3Internal &p_vec) const {
	return SGFixedVector3Internal(tdotx(p_vec), tdoty(p_vec));
}

SGFixedVector3Internal SGFixedTransform3DInternal::basis_xform_inv(const SGFixedVector3Internal &p_vec) const {
	return SGFixedVector3Internal(elements[0].dot(p_vec), elements[1].dot(p_vec));
}

SGFixedVector3Internal SGFixedTransform3DInternal::xform(const SGFixedVector3Internal &p_vec) const {
	return SGFixedVector3Internal(tdotx(p_vec), tdoty(p_vec)) + elements[2];
}

SGFixedVector3Internal SGFixedTransform3DInternal::xform_inv(const SGFixedVector3Internal &p_vec) const {
	SGFixedVector3Internal v = p_vec - elements[2];
	return SGFixedVector3Internal(elements[0].dot(v), elements[1].dot(v));
}

void SGFixedTransform3DInternal::set_rotation_and_scale(fixed p_rot, const SGFixedVector3Internal &p_scale) {
	elements[0][0] = p_rot.cos() * p_scale.x;
	elements[1][1] = p_rot.cos() * p_scale.y;
	elements[1][0] = -p_rot.sin() * p_scale.y;
	elements[0][1] = p_rot.sin() * p_scale.x;
}

#endif
