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

#ifndef SG_FIXED_TRANSFORM_3D_H
#define SG_FIXED_TRANSFORM_3D_H

#include <godot_cpp/classes/ref_counted.hpp>

#include "sg_fixed_vector3.h"
#include "../../internal/sg_fixed_transform_3D_internal.h"

using namespace godot;

class SGFixedTransform3D : public RefCounted {
	GDCLASS(SGFixedTransform3D, RefCounted);

	Ref<SGFixedVector3> x;
	Ref<SGFixedVector3> y;
	Ref<SGFixedVector3> origin;

protected:
	static void _bind_methods();

public:
	_FORCE_INLINE_ Ref<SGFixedVector3> get_x() const { return x; }
	void set_x(const Ref<SGFixedVector3> &p_x);

	_FORCE_INLINE_ Ref<SGFixedVector3> get_y() const { return y; }
	void set_y(const Ref<SGFixedVector3> &p_y);

	_FORCE_INLINE_ Ref<SGFixedVector3> get_origin() const { return origin; }
	void set_origin(const Ref<SGFixedVector3> &p_origin);

	_FORCE_INLINE_ SGFixedTransform3DInternal get_internal() const {
		return SGFixedTransform3DInternal(fixed(x->get_x()), fixed(x->get_y()), fixed(y->get_x()), fixed(y->get_y()), fixed(origin->get_x()), fixed(origin->get_y()));
	}

	_FORCE_INLINE_ void set_internal(const SGFixedTransform3DInternal &p_internal) {
		x->set_x(p_internal[0][0].value);
		x->set_y(p_internal[0][1].value);
		y->set_x(p_internal[1][0].value);
		y->set_y(p_internal[1][1].value);
		origin->set_x(p_internal[2][0].value);
		origin->set_y(p_internal[2][1].value);
	}

	_FORCE_INLINE_ static Ref<SGFixedTransform3D> from_internal(const SGFixedTransform3DInternal &p_internal) {
		return Ref<SGFixedTransform3D>(memnew(SGFixedTransform3D(p_internal)));
	}

	Transform3D to_float() const;
	void from_float(const Transform3D &p_float_transform);

	Ref<SGFixedTransform3D> copy() const;

	Ref<SGFixedTransform3D> inverse() const;
	Ref<SGFixedTransform3D> affine_inverse() const;

	int64_t get_rotation() const;
	Ref<SGFixedTransform3D> rotated(int64_t p_radians) const;

	Ref<SGFixedVector3> get_scale() const;
	Ref<SGFixedTransform3D> scaled(const Ref<SGFixedVector3> &p_scale) const;
	Ref<SGFixedTransform3D> translated(const Ref<SGFixedVector3> &p_offset) const;

	Ref<SGFixedTransform3D> orthonormalized() const;
	bool is_equal_approx(const Ref<SGFixedTransform3D> &p_transform) const;

	Ref<SGFixedTransform3D> mul(const Ref<SGFixedTransform3D> &p_transform) const;

	Ref<SGFixedTransform3D> interpolate_with(const Ref<SGFixedTransform3D> &p_transform, int64_t p_weight) const;

	Ref<SGFixedVector3> basis_xform(const Ref<SGFixedVector3> &p_vec) const;
	Ref<SGFixedVector3> basis_xform_inv(const Ref<SGFixedVector3> &p_vec) const;
	Ref<SGFixedVector3> xform(const Ref<SGFixedVector3> &p_vec) const;
	Ref<SGFixedVector3> xform_inv(const Ref<SGFixedVector3> &p_vec) const;

	SGFixedTransform3D();
	SGFixedTransform3D(const SGFixedTransform3DInternal &p_internal);
};

#endif
