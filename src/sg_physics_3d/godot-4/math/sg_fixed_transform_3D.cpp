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

#include "sg_fixed_transform_3D.h"

void SGFixedTransform3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_x"), &SGFixedTransform3D::get_x);
	ClassDB::bind_method(D_METHOD("set_x", "x"), &SGFixedTransform3D::set_x);
	ClassDB::bind_method(D_METHOD("get_y"), &SGFixedTransform3D::get_y);
	ClassDB::bind_method(D_METHOD("set_y", "y"), &SGFixedTransform3D::set_y);
	ClassDB::bind_method(D_METHOD("get_z"), &SGFixedTransform3D::get_z);
	ClassDB::bind_method(D_METHOD("set_z", "z"), &SGFixedTransform3D::set_z);
	ClassDB::bind_method(D_METHOD("get_origin"), &SGFixedTransform3D::get_origin);
	ClassDB::bind_method(D_METHOD("set_origin", "origin"), &SGFixedTransform3D::set_origin);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "x", PROPERTY_HINT_NONE, "", 0), "set_x", "get_x");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "y", PROPERTY_HINT_NONE, "", 0), "set_y", "get_y");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "z", PROPERTY_HINT_NONE, "", 0), "set_z", "get_z");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "origin", PROPERTY_HINT_NONE, "", 0), "set_origin", "get_origin");

	ClassDB::bind_method(D_METHOD("inverse"), &SGFixedTransform3D::inverse);
	ClassDB::bind_method(D_METHOD("affine_inverse"), &SGFixedTransform3D::affine_inverse);
	ClassDB::bind_method(D_METHOD("get_rotation"), &SGFixedTransform3D::get_rotation);
	ClassDB::bind_method(D_METHOD("rotated", "radians"), &SGFixedTransform3D::rotated);
	ClassDB::bind_method(D_METHOD("get_scale"), &SGFixedTransform3D::get_scale);
	ClassDB::bind_method(D_METHOD("scaled", "scale"), &SGFixedTransform3D::scaled);
	ClassDB::bind_method(D_METHOD("translated", "offset"), &SGFixedTransform3D::translated);
	ClassDB::bind_method(D_METHOD("orthonormalized"), &SGFixedTransform3D::orthonormalized);
	ClassDB::bind_method(D_METHOD("is_equal_approx", "transform"), &SGFixedTransform3D::is_equal_approx);
	ClassDB::bind_method(D_METHOD("mul", "transform"), &SGFixedTransform3D::mul);
	ClassDB::bind_method(D_METHOD("interpolate_with", "transform"), &SGFixedTransform3D::interpolate_with);
	ClassDB::bind_method(D_METHOD("basis_xform", "vector"), &SGFixedTransform3D::basis_xform);
	ClassDB::bind_method(D_METHOD("basis_xform_inv", "vector"), &SGFixedTransform3D::basis_xform_inv);
	ClassDB::bind_method(D_METHOD("xform", "vector"), &SGFixedTransform3D::xform);
	ClassDB::bind_method(D_METHOD("xform_inv", "vector"), &SGFixedTransform3D::xform_inv);

	ClassDB::bind_method(D_METHOD("copy"), &SGFixedTransform3D::copy);
}

Transform3D SGFixedTransform3D::to_float() const {
	Vector3 x_float = x->to_float();
	Vector3 y_float = y->to_float();
	Vector3 z_float = z->to_float();
	Vector3 origin_float = origin->to_float();
	return Transform3D(x_float.x, x_float.y, x_float.z, y_float.x, y_float.y, y_float.z, z_float.x, z_float.y, z_float.z, origin_float.x, origin_float.y, origin_float.z);
}

void SGFixedTransform3D::from_float(const Transform3D &p_float_transform) {
	// TODO: someone remind me how 3D transforms are different than columns in 2D / how to get the vectors from transform 
	x->from_float(p_float_transform.get_basis().get_column(0));
	y->from_float(p_float_transform.get_basis().get_column(1));
	z->from_float(p_float_transform.get_basis().get_column(2));
	origin->from_float(p_float_transform.origin);
}

Ref<SGFixedTransform3D> SGFixedTransform3D::copy() const {
	Ref<SGFixedTransform3D> ret(memnew(SGFixedTransform3D));
	ret->x->set_internal(x->get_internal());
	ret->y->set_internal(y->get_internal());
	ret->z->set_internal(z->get_internal());
	ret->origin->set_internal(origin->get_internal());
	return ret;
}

void SGFixedTransform3D::set_x(const Ref<SGFixedVector3> &p_x) {
	ERR_FAIL_COND(!p_x.is_valid());
	x->set_internal(p_x->get_internal());
}

void SGFixedTransform3D::set_y(const Ref<SGFixedVector3> &p_y) {
	ERR_FAIL_COND(!p_y.is_valid());
	y->set_internal(p_y->get_internal());
}

void SGFixedTransform3D::set_z(const Ref<SGFixedVector3> &p_z) {
	ERR_FAIL_COND(!p_z.is_valid());
	z->set_internal(p_z->get_internal());
}

void SGFixedTransform3D::set_origin(const Ref<SGFixedVector3> &p_origin) {
	ERR_FAIL_COND(!p_origin.is_valid());
	origin->set_internal(p_origin->get_internal());
}

Ref<SGFixedTransform3D> SGFixedTransform3D::inverse() const {
	SGFixedTransform3DInternal internal = get_internal();
	internal.invert();
	return SGFixedTransform3D::from_internal(internal);
}

Ref<SGFixedTransform3D> SGFixedTransform3D::affine_inverse() const {
	SGFixedTransform3DInternal internal = get_internal();
	internal.affine_invert();
	return SGFixedTransform3D::from_internal(internal);
}

int64_t SGFixedTransform3D::get_rotation() const {
	SGFixedTransform3DInternal internal = get_internal();
	return internal.get_rotation().value;
}

Ref<SGFixedTransform3D> SGFixedTransform3D::rotated(int64_t p_radians) const {
	SGFixedTransform3DInternal internal = get_internal();
	internal.rotate(fixed(p_radians));
	return SGFixedTransform3D::from_internal(internal);
}

Ref<SGFixedVector3> SGFixedTransform3D::get_scale() const {
	SGFixedTransform3DInternal internal = get_internal();
	return SGFixedVector3::from_internal(internal.get_scale());
}

Ref<SGFixedTransform3D> SGFixedTransform3D::scaled(const Ref<SGFixedVector3> &p_scale) const {
	ERR_FAIL_COND_V(!p_scale.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedTransform3DInternal internal = get_internal();
	internal.scale(p_scale->get_internal());
	return SGFixedTransform3D::from_internal(internal);
}

Ref<SGFixedTransform3D> SGFixedTransform3D::translated(const Ref<SGFixedVector3> &p_offset) const {
	ERR_FAIL_COND_V(!p_offset.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedTransform3DInternal internal = get_internal();
	internal.translate(p_offset->get_internal());
	return SGFixedTransform3D::from_internal(internal);
}

Ref<SGFixedTransform3D> SGFixedTransform3D::orthonormalized() const {
	SGFixedTransform3DInternal internal = get_internal();
	internal.orthonormalize();
	return SGFixedTransform3D::from_internal(internal);
}

bool SGFixedTransform3D::is_equal_approx(const Ref<SGFixedTransform3D> &p_transform) const {
	ERR_FAIL_COND_V(!p_transform.is_valid(), false);
	SGFixedTransform3DInternal internal = get_internal();
	return internal.is_equal_approx(p_transform->get_internal());
}

Ref<SGFixedTransform3D> SGFixedTransform3D::mul(const Ref<SGFixedTransform3D> &p_transform) const {
	ERR_FAIL_COND_V(!p_transform.is_valid(), Ref<SGFixedTransform3D>());
	return SGFixedTransform3D::from_internal(get_internal() * p_transform->get_internal());
}

Ref<SGFixedTransform3D> SGFixedTransform3D::interpolate_with(const Ref<SGFixedTransform3D> &p_transform, int64_t p_weight) const {
	ERR_FAIL_COND_V(!p_transform.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedTransform3DInternal internal = get_internal();
	internal.interpolate_with(p_transform->get_internal(), fixed(p_weight));
	return SGFixedTransform3D::from_internal(internal);
}

Ref<SGFixedVector3> SGFixedTransform3D::basis_xform(const Ref<SGFixedVector3> &p_vec) const {
	ERR_FAIL_COND_V(!p_vec.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedVector3Internal internal = p_vec->get_internal();
	internal = get_internal().basis_xform(internal);
	return SGFixedVector3::from_internal(internal);
}

Ref<SGFixedVector3> SGFixedTransform3D::basis_xform_inv(const Ref<SGFixedVector3> &p_vec) const {
	ERR_FAIL_COND_V(!p_vec.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedVector3Internal internal = p_vec->get_internal();
	internal = get_internal().basis_xform_inv(internal);
	return SGFixedVector3::from_internal(internal);
}

Ref<SGFixedVector3> SGFixedTransform3D::xform(const Ref<SGFixedVector3> &p_vec) const {
	ERR_FAIL_COND_V(!p_vec.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedVector3Internal internal = p_vec->get_internal();
	internal = get_internal().xform(internal);
	return SGFixedVector3::from_internal(internal);
}

Ref<SGFixedVector3> SGFixedTransform3D::xform_inv(const Ref<SGFixedVector3> &p_vec) const {
	ERR_FAIL_COND_V(!p_vec.is_valid(), Ref<SGFixedTransform3D>());
	SGFixedVector3Internal internal = p_vec->get_internal();
	internal = get_internal().xform_inv(internal);
	return SGFixedVector3::from_internal(internal);
}

SGFixedTransform3D::SGFixedTransform3D() :
	x(memnew(SGFixedVector3(SGFixedVector3Internal(fixed::ONE, fixed::ZERO, fixed::ZERO)))),
	y(memnew(SGFixedVector3(SGFixedVector3Internal(fixed::ZERO, fixed::ONE, fixed::ZERO)))),
	z(memnew(SGFixedVector3(SGFixedVector3Internal(fixed::ZERO, fixed::ZERO, fixed::ONE)))),
	origin(memnew(SGFixedVector3()))
{
}

SGFixedTransform3D::SGFixedTransform3D(const SGFixedTransform3DInternal &p_internal) :
	x(memnew(SGFixedVector3(p_internal.elements[0]))),
	y(memnew(SGFixedVector3(p_internal.elements[1]))),
	z(memnew(SGFixedVector3(p_internal.elements[2]))),
	origin(memnew(SGFixedVector3(p_internal.elements[3])))
{
}
