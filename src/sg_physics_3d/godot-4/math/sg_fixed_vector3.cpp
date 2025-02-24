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

#include "sg_fixed_vector3.h"

void SGFixedVector3::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_x"), &SGFixedVector3::get_x);
	ClassDB::bind_method(D_METHOD("set_x", "x"), &SGFixedVector3::set_x);
	ClassDB::bind_method(D_METHOD("get_y"), &SGFixedVector3::get_y);
	ClassDB::bind_method(D_METHOD("set_y", "y"), &SGFixedVector3::set_y);
	ClassDB::bind_method(D_METHOD("get_z"), &SGFixedVector3::get_z);
	ClassDB::bind_method(D_METHOD("set_z", "z"), &SGFixedVector3::set_z);

	ClassDB::bind_method(D_METHOD("clear"), &SGFixedVector3::clear);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "x", PROPERTY_HINT_NONE), "set_x", "get_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "y", PROPERTY_HINT_NONE), "set_y", "get_y");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "z", PROPERTY_HINT_NONE), "set_z", "get_z");

	ClassDB::bind_method(D_METHOD("add", "value"), &SGFixedVector3::add);
	ClassDB::bind_method(D_METHOD("iadd", "value"), &SGFixedVector3::iadd);
	ClassDB::bind_method(D_METHOD("sub", "value"), &SGFixedVector3::sub);
	ClassDB::bind_method(D_METHOD("isub", "value"), &SGFixedVector3::isub);
	ClassDB::bind_method(D_METHOD("mul", "value"), &SGFixedVector3::mul);
	ClassDB::bind_method(D_METHOD("imul", "value"), &SGFixedVector3::imul);
	ClassDB::bind_method(D_METHOD("div", "value"), &SGFixedVector3::div);
	ClassDB::bind_method(D_METHOD("idiv", "value"), &SGFixedVector3::idiv);

	ClassDB::bind_method(D_METHOD("copy"), &SGFixedVector3::copy);
	ClassDB::bind_method(D_METHOD("abs"), &SGFixedVector3::abs);
	ClassDB::bind_method(D_METHOD("normalized"), &SGFixedVector3::normalized);
	ClassDB::bind_method(D_METHOD("is_normalized"), &SGFixedVector3::is_normalized);
	ClassDB::bind_method(D_METHOD("length"), &SGFixedVector3::length);
	ClassDB::bind_method(D_METHOD("length_squared"), &SGFixedVector3::length_squared);

	ClassDB::bind_method(D_METHOD("distance_to", "other_vector"), &SGFixedVector3::distance_to);
	ClassDB::bind_method(D_METHOD("distance_squared_to", "other_vector"), &SGFixedVector3::distance_squared_to);
	ClassDB::bind_method(D_METHOD("angle_to", "other_vector"), &SGFixedVector3::angle_to);
	ClassDB::bind_method(D_METHOD("angle_to_point", "other_vector"), &SGFixedVector3::angle_to_point);
	ClassDB::bind_method(D_METHOD("direction_to", "other_vector"), &SGFixedVector3::direction_to);

	ClassDB::bind_method(D_METHOD("rotate", "radians"), &SGFixedVector3::rotate);
	ClassDB::bind_method(D_METHOD("rotated", "radians"), &SGFixedVector3::rotated);
	ClassDB::bind_method(D_METHOD("angle"), &SGFixedVector3::angle);

	ClassDB::bind_method(D_METHOD("dot", "other_vector"), &SGFixedVector3::dot);
	ClassDB::bind_method(D_METHOD("cross", "other_vector"), &SGFixedVector3::cross);

	ClassDB::bind_method(D_METHOD("linear_interpolate", "other_vector", "weight"), &SGFixedVector3::linear_interpolate);
	ClassDB::bind_method(D_METHOD("cubic_interpolate", "other_vector", "pre_a", "post_b", "weight"), &SGFixedVector3::cubic_interpolate);

	ClassDB::bind_method(D_METHOD("slide", "normal"), &SGFixedVector3::slide);
	ClassDB::bind_method(D_METHOD("bounce", "normal"), &SGFixedVector3::bounce);
	ClassDB::bind_method(D_METHOD("reflect", "normal"), &SGFixedVector3::reflect);

	ClassDB::bind_method(D_METHOD("is_equal_approx", "other_vector"), &SGFixedVector3::is_equal_approx);

	ClassDB::bind_method(D_METHOD("from_float", "float_vector"), &SGFixedVector3::from_float);
	ClassDB::bind_method(D_METHOD("to_float"), &SGFixedVector3::to_float);
}

Ref<SGFixedVector3> SGFixedVector3::add(const Variant &p_other) const {
	if (p_other.get_type() == Variant::INT) {
		return SGFixedVector3::from_internal(value + fixed(p_other));
	}
	
	Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
	ERR_FAIL_COND_V_MSG(!other_vector.is_valid(), Variant(), "Invalid type.");

	return SGFixedVector3::from_internal(value + other_vector->get_internal());
}

void SGFixedVector3::iadd(const Variant &p_other) {
	if (p_other.get_type() == Variant::INT) {
		value += fixed(p_other);
	}
	else {
		Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
		ERR_FAIL_COND_MSG(!other_vector.is_valid(), "Invalid type.");

		value += other_vector->get_internal();
	}
	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

Ref<SGFixedVector3> SGFixedVector3::sub(const Variant &p_other) const {
	if (p_other.get_type() == Variant::INT) {
		return SGFixedVector3::from_internal(value - fixed(p_other));
	}
	
	Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
	ERR_FAIL_COND_V_MSG(!other_vector.is_valid(), Variant(), "Invalid type.");

	return SGFixedVector3::from_internal(value - other_vector->get_internal());
}

void SGFixedVector3::isub(const Variant &p_other) {
	if (p_other.get_type() == Variant::INT) {
		value -= fixed(p_other);
	}
	else {
		Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
		ERR_FAIL_COND_MSG(!other_vector.is_valid(), "Invalid type.");

		value -= other_vector->get_internal();
	}
	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

Ref<SGFixedVector3> SGFixedVector3::mul(const Variant &p_other) const {
	if (p_other.get_type() == Variant::INT) {
		return SGFixedVector3::from_internal(value * fixed(p_other));
	}
	
	Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
	ERR_FAIL_COND_V_MSG(!other_vector.is_valid(), Variant(), "Invalid type.");

	return SGFixedVector3::from_internal(value * other_vector->get_internal());
}

void SGFixedVector3::imul(const Variant &p_other) {
	if (p_other.get_type() == Variant::INT) {
		value *= fixed(p_other);
	}
	else {
		Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
		ERR_FAIL_COND_MSG(!other_vector.is_valid(), "Invalid type.");

		value *= other_vector->get_internal();
	}
	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

Ref<SGFixedVector3> SGFixedVector3::div(const Variant &p_other) const {
	if (p_other.get_type() == Variant::INT) {
		return SGFixedVector3::from_internal(value / fixed(p_other));
	}
	
	Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
	ERR_FAIL_COND_V_MSG(!other_vector.is_valid(), Variant(), "Invalid type.");

	return SGFixedVector3::from_internal(value / other_vector->get_internal());
}

void SGFixedVector3::idiv(const Variant &p_other) {
	if (p_other.get_type() == Variant::INT) {
		value /= fixed(p_other);
	}
	else {
		Ref<SGFixedVector3> other_vector = Object::cast_to<SGFixedVector3>(p_other);
		ERR_FAIL_COND_MSG(!other_vector.is_valid(), "Invalid type.");

		value /= other_vector->get_internal();
	}
	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

Ref<SGFixedVector3> SGFixedVector3::copy() const {
	return SGFixedVector3::from_internal(value);
}

Ref<SGFixedVector3> SGFixedVector3::abs() const {
	return SGFixedVector3::from_internal(value.abs());
}

Ref<SGFixedVector3> SGFixedVector3::normalized() const {
	return SGFixedVector3::from_internal(value.normalized());
}

bool SGFixedVector3::is_normalized() const {
	return value.is_normalized();
}

int64_t SGFixedVector3::length() const {
	return value.length().value;
}

int64_t SGFixedVector3::length_squared() const {
	return value.length_squared().value;
}

int64_t SGFixedVector3::distance_to(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.distance_to(p_other->get_internal()).value;
}

int64_t SGFixedVector3::distance_squared_to(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.distance_squared_to(p_other->get_internal()).value;
}

int64_t SGFixedVector3::angle_to(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.angle_to(p_other->get_internal()).value;
}

int64_t SGFixedVector3::angle_to_point(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.angle_to_point(p_other->get_internal()).value;
}

Ref<SGFixedVector3> SGFixedVector3::direction_to(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), Ref<SGFixedVector3>());
	return SGFixedVector3::from_internal(value.direction_to(p_other->get_internal()));
}

void SGFixedVector3::rotate(int64_t p_rotation) {
	value = value.rotated(fixed(p_rotation));

	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

int64_t SGFixedVector3::angle() const {
	return value.angle().value;
}

int64_t SGFixedVector3::dot(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.dot(p_other->get_internal()).value;
}

int64_t SGFixedVector3::cross(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), 0);
	return value.cross(p_other->get_internal()).value;
}

Ref<SGFixedVector3> SGFixedVector3::rotated(int64_t p_rotation) const {
	return SGFixedVector3::from_internal(value.rotated(fixed(p_rotation)));
}

Ref<SGFixedVector3> SGFixedVector3::linear_interpolate(const Ref<SGFixedVector3> &p_to, int64_t weight) const {
	ERR_FAIL_COND_V(!p_to.is_valid(), Ref<SGFixedVector3>());
	return SGFixedVector3::from_internal(SGFixedVector3Internal::linear_interpolate(value, p_to->get_internal(), fixed(weight)));
}

Ref<SGFixedVector3> SGFixedVector3::slide(const Ref<SGFixedVector3> &p_normal) const {
	ERR_FAIL_COND_V(!p_normal.is_valid(), Ref<SGFixedVector3>());
	Ref<SGFixedVector3> v(memnew(SGFixedVector3));
	v->value = value.slide(p_normal->value);
	return v;
}

Ref<SGFixedVector3> SGFixedVector3::bounce(const Ref<SGFixedVector3> &p_normal) const {
	ERR_FAIL_COND_V(!p_normal.is_valid(), Ref<SGFixedVector3>());
	Ref<SGFixedVector3> v(memnew(SGFixedVector3));
	v->value = value.bounce(p_normal->value);
	return v;
}

Ref<SGFixedVector3> SGFixedVector3::reflect(const Ref<SGFixedVector3> &p_normal) const {
	ERR_FAIL_COND_V(!p_normal.is_valid(), Ref<SGFixedVector3>());
	Ref<SGFixedVector3> v(memnew(SGFixedVector3));
	v->value = value.reflect(p_normal->value);
	return v;
}

bool SGFixedVector3::is_equal_approx(const Ref<SGFixedVector3> &p_other) const {
	ERR_FAIL_COND_V(!p_other.is_valid(), false);
	return value.is_equal_approx(p_other->get_internal());
}

void SGFixedVector3::from_float(Vector3 p_float_vector) {
	value.x = fixed::from_float(p_float_vector.x);
	value.y = fixed::from_float(p_float_vector.y);
	value.z = fixed::from_float(p_float_vector.z);

	if (watcher) {
		watcher->fixed_vector3_changed(this);
	}
}

Vector3 SGFixedVector3::to_float() const {
	return Vector3(value.x.to_float(), value.y.to_float(), value.z.to_float());
}

Ref<SGFixedVector3> SGFixedVector3::cubic_interpolate(const Ref<SGFixedVector3>& p_b, const Ref<SGFixedVector3>& p_pre_a, const Ref<SGFixedVector3>& p_post_b, int64_t p_weight) const {
	return SGFixedVector3::from_internal(value.cubic_interpolate(p_b->get_internal(), p_pre_a->get_internal(), p_post_b->get_internal(), fixed(p_weight)));
}

