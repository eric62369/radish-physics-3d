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

#include "sg_fixed_node_3D.h"

#include <core/engine.h>

void SGFixedNode3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_fixed_transform"), &SGFixedNode3D::get_fixed_transform);
	ClassDB::bind_method(D_METHOD("set_fixed_transform", "fixed_transform"), &SGFixedNode3D::set_fixed_transform);

	ClassDB::bind_method(D_METHOD("get_fixed_position"), &SGFixedNode3D::get_fixed_position);
	ClassDB::bind_method(D_METHOD("set_fixed_position", "fixed_position"), &SGFixedNode3D::set_fixed_position);
	ClassDB::bind_method(D_METHOD("_get_fixed_position_x"), &SGFixedNode3D::_get_fixed_position_x);
	ClassDB::bind_method(D_METHOD("_set_fixed_position_x", "x"), &SGFixedNode3D::_set_fixed_position_x);
	ClassDB::bind_method(D_METHOD("_get_fixed_position_y"), &SGFixedNode3D::_get_fixed_position_y);
	ClassDB::bind_method(D_METHOD("_set_fixed_position_y", "y"), &SGFixedNode3D::_set_fixed_position_y);

	ClassDB::bind_method(D_METHOD("get_fixed_scale"), &SGFixedNode3D::get_fixed_scale);
	ClassDB::bind_method(D_METHOD("set_fixed_scale", "fixed_scale"), &SGFixedNode3D::set_fixed_scale);
	ClassDB::bind_method(D_METHOD("_get_fixed_scale_x"), &SGFixedNode3D::_get_fixed_scale_x);
	ClassDB::bind_method(D_METHOD("_set_fixed_scale_x", "x"), &SGFixedNode3D::_set_fixed_scale_x);
	ClassDB::bind_method(D_METHOD("_get_fixed_scale_y"), &SGFixedNode3D::_get_fixed_scale_y);
	ClassDB::bind_method(D_METHOD("_set_fixed_scale_y", "y"), &SGFixedNode3D::_set_fixed_scale_y);

	ClassDB::bind_method(D_METHOD("get_fixed_rotation"), &SGFixedNode3D::get_fixed_rotation);
	ClassDB::bind_method(D_METHOD("set_fixed_rotation", "fixed_scale"), &SGFixedNode3D::set_fixed_rotation);

	// For editor and storage.
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_position_x"), "_set_fixed_position_x", "_get_fixed_position_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_position_y"), "_set_fixed_position_y", "_get_fixed_position_y");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_scale_x"), "_set_fixed_scale_x", "_get_fixed_scale_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_scale_y"), "_set_fixed_scale_y", "_get_fixed_scale_y");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_rotation"), "set_fixed_rotation", "get_fixed_rotation");

	// For code only.
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "fixed_transform", PROPERTY_HINT_NONE, "", 0), "set_fixed_transform", "get_fixed_transform");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "fixed_position", PROPERTY_HINT_NONE, "", 0), "set_fixed_position", "get_fixed_position");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "fixed_scale", PROPERTY_HINT_NONE, "", 0), "set_fixed_scale", "get_fixed_scale");

	ClassDB::bind_method(D_METHOD("get_global_fixed_transform"), &SGFixedNode3D::get_global_fixed_transform);
	ClassDB::bind_method(D_METHOD("set_global_fixed_transform", "fixed_transform"), &SGFixedNode3D::set_global_fixed_transform);

	ClassDB::bind_method(D_METHOD("get_global_fixed_position"), &SGFixedNode3D::get_global_fixed_position);
	ClassDB::bind_method(D_METHOD("set_global_fixed_position", "fixed_position"), &SGFixedNode3D::set_global_fixed_position);

	ClassDB::bind_method(D_METHOD("get_global_fixed_rotation"), &SGFixedNode3D::get_global_fixed_rotation);
	ClassDB::bind_method(D_METHOD("set_global_fixed_rotation", "fixed_rotation"), &SGFixedNode3D::set_global_fixed_rotation);

	ClassDB::bind_method(D_METHOD("update_float_transform"), &SGFixedNode3D::update_float_transform);
}

void SGFixedNode3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSFORM_CHANGED:
			update_float_transform();
			break;

		case NOTIFICATION_ENTER_TREE:
			get_global_transform();
			break;
	}
}

#ifdef TOOLS_ENABLED
void SGFixedNode3D::_changed_callback(Object *p_changed, const char *p_prop) {
	if (!updating_transform) {
		if (strcmp(p_prop, "position") == 0) {
			Vector2 position = get_position();
			fixed_transform->get_origin()->set_internal(SGFixedVector2Internal(fixed::from_float(position.x), fixed::from_float(position.y)));
			set_fixed_position(fixed_transform->get_origin());
		}
		else if (strcmp(p_prop, "scale") == 0) {
			fixed_scale->from_float(get_scale());
			set_fixed_scale(fixed_scale);
		}
		else if (strcmp(p_prop, "rotation") == 0) {
			set_fixed_rotation(fixed::from_float(get_rotation()).value);
		}
		else if (strcmp(p_prop, "transform") == 0) {
			//fixed_transform->from_float(get_transform());
			//set_fixed_transform(fixed_transform);
		}
	}
}
#endif

void SGFixedNode3D::_update_fixed_transform_rotation_and_scale() {
	SGFixedTransform3DInternal new_xform;
	new_xform.set_rotation_and_scale(fixed(fixed_rotation), fixed_scale->get_internal());
	fixed_transform->get_x()->set_internal(new_xform[0]);
	fixed_transform->get_y()->set_internal(new_xform[1]);
	transform_changed();
}

SGFixedTransform3DInternal SGFixedNode3D::get_global_fixed_transform_internal() const {
	SGFixedNode3D *fixed_parent = Object::cast_to<SGFixedNode3D>(get_parent());
	if (fixed_parent) {
		return fixed_parent->get_global_fixed_transform_internal() * fixed_transform->get_internal();
	}
	return fixed_transform->get_internal();
}

void SGFixedNode3D::update_fixed_transform_internal(const SGFixedTransform3DInternal &p_transform) {
	fixed_transform->set_internal(p_transform);
	fixed_scale->set_internal(p_transform.get_scale());
	fixed_rotation = p_transform.get_rotation().value;
	transform_changed();
	_change_notify("fixed_position_x");
	_change_notify("fixed_position_y");
	_change_notify("fixed_scale_x");
	_change_notify("fixed_scale_y");
	_change_notify("fixed_rotation");
}

void SGFixedNode3D::update_global_fixed_transform_internal(const SGFixedTransform3DInternal &p_global_transform) {
	SGFixedNode3D *fixed_parent = Object::cast_to<SGFixedNode3D>(get_parent());
	if (fixed_parent) {
		update_fixed_transform_internal(fixed_parent->get_global_fixed_transform_internal().affine_inverse() * p_global_transform);
	}
	else {
		update_fixed_transform_internal(p_global_transform);
	}
}

int64_t SGFixedNode3D::_get_fixed_position_x() const {
	return fixed_transform->get_origin()->get_x();
}

void SGFixedNode3D::_set_fixed_position_x(int64_t p_x) {
	fixed_transform->get_origin()->set_x(p_x);
	_change_notify("fixed_position_x");
}

int64_t SGFixedNode3D::_get_fixed_position_y() const {
	return fixed_transform->get_origin()->get_y();
}

void SGFixedNode3D::_set_fixed_position_y(int64_t p_y) {
	fixed_transform->get_origin()->set_y(p_y);
	_change_notify("fixed_position_y");
}

int64_t SGFixedNode3D::_get_fixed_scale_x() const {
	return fixed_scale->get_x();
}

void SGFixedNode3D::_set_fixed_scale_x(int64_t p_scale_x) {
	fixed_scale->set_x(p_scale_x);
	_update_fixed_transform_rotation_and_scale();
	_change_notify("fixed_scale_x");
}

int64_t SGFixedNode3D::_get_fixed_scale_y() const {
	return fixed_scale->get_y();
}

void SGFixedNode3D::_set_fixed_scale_y(int64_t p_scale_y) {
	fixed_scale->set_y(p_scale_y);
	_update_fixed_transform_rotation_and_scale();
	_change_notify("fixed_scale_y");
}

void SGFixedNode3D::set_fixed_transform(const Ref<SGFixedTransform3D> &p_fixed_transform) {
	ERR_FAIL_COND(!p_fixed_transform.is_valid());
	update_fixed_transform_internal(p_fixed_transform->get_internal());
}

Ref<SGFixedTransform3D> SGFixedNode3D::get_fixed_transform() const {
	return fixed_transform;
}

void SGFixedNode3D::set_fixed_position(const Ref<SGFixedVector2> &p_fixed_position) {
	ERR_FAIL_COND(!p_fixed_position.is_valid());

	fixed_transform->get_origin()->set_internal(p_fixed_position->get_internal());
	transform_changed();
	_change_notify("fixed_position_x");
	_change_notify("fixed_position_y");

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		updating_transform = true;
		set_position(fixed_transform->get_origin()->to_float());
		updating_transform = false;
	}
#endif
}

Ref<SGFixedVector2> SGFixedNode3D::get_fixed_position() const {
	return fixed_transform->get_origin();
}

void SGFixedNode3D::set_fixed_scale(const Ref<SGFixedVector2> &p_fixed_scale) {
	ERR_FAIL_COND(!p_fixed_scale.is_valid());

	fixed_scale->set_internal(p_fixed_scale->get_internal());
	_update_fixed_transform_rotation_and_scale();
	_change_notify("fixed_scale_x");
	_change_notify("fixed_scale_y");

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		updating_transform = true;
		set_scale(fixed_scale->to_float());
		updating_transform = false;
	}
#endif
}

Ref<SGFixedVector2> SGFixedNode3D::get_fixed_scale() const {
	return fixed_scale;
}

void SGFixedNode3D::set_fixed_rotation(int64_t p_fixed_rotation) {
	fixed_rotation = p_fixed_rotation;
	_update_fixed_transform_rotation_and_scale();
	_change_notify("fixed_rotation");

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		updating_transform = true;
		set_rotation(fixed(fixed_rotation).to_float());
		updating_transform = false;
	}
#endif
}

int64_t SGFixedNode3D::get_fixed_rotation() const {
	return fixed_rotation;
}

void SGFixedNode3D::set_global_fixed_transform(const Ref<SGFixedTransform3D> &p_global_transform) {
	ERR_FAIL_COND(!p_global_transform.is_valid());
	update_global_fixed_transform_internal(p_global_transform->get_internal());
}

Ref<SGFixedTransform3D> SGFixedNode3D::get_global_fixed_transform() const {
	return Ref<SGFixedTransform3D>(memnew(SGFixedTransform3D(get_global_fixed_transform_internal())));
}

void SGFixedNode3D::set_global_fixed_position(const Ref<SGFixedVector2> &p_fixed_position) {
	ERR_FAIL_COND(!p_fixed_position.is_valid());
	set_global_fixed_position_internal(p_fixed_position->get_internal());
}

Ref<SGFixedVector2> SGFixedNode3D::get_global_fixed_position() {
	return SGFixedVector2::from_internal(get_global_fixed_transform_internal().get_origin());
}

void SGFixedNode3D::set_fixed_position_internal(const SGFixedVector2Internal &p_fixed_position) {
	fixed_transform->get_origin()->set_internal(p_fixed_position);
	transform_changed();
}

void SGFixedNode3D::set_global_fixed_position_internal(const SGFixedVector2Internal &p_fixed_position) {
	SGFixedNode3D *fixed_parent = Object::cast_to<SGFixedNode3D>(get_parent());
	if (fixed_parent) {
		fixed_transform->get_origin()->set_internal(fixed_parent->get_global_fixed_transform_internal().affine_inverse().xform(p_fixed_position));
	}
	else {
		fixed_transform->get_origin()->set_internal(p_fixed_position);
	}
	transform_changed();
}


void SGFixedNode3D::set_global_fixed_rotation(int64_t p_fixed_rotation) {
	SGFixedNode3D *fixed_parent = Object::cast_to<SGFixedNode3D>(get_parent());
	if (fixed_parent) {
		fixed parent_rotation = fixed_parent->get_global_fixed_transform_internal().get_rotation();
		set_fixed_rotation(p_fixed_rotation - parent_rotation.value);
	}
	else {
		set_fixed_rotation(p_fixed_rotation);
	}
}

int64_t SGFixedNode3D::get_global_fixed_rotation() const {
	return get_global_fixed_transform_internal().get_rotation().value;
}

void SGFixedNode3D::update_float_transform() {
	if (fixed_xform_dirty) {
#ifdef TOOLS_ENABLED
		updating_transform = true;
#endif

		Transform3D float_xform;
		float_xform.set_rotation_and_scale(fixed(fixed_rotation).to_float(), fixed_scale->to_float());
		float_xform[2] = fixed_transform->get_origin()->to_float();
		set_transform(float_xform);

#ifdef TOOLS_ENABLED
		updating_transform = false;;
#endif

		fixed_xform_dirty = false;
	}
	get_global_transform();
}

void SGFixedNode3D::fixed_vector2_changed(SGFixedVector2 *p_vector) {
	if (p_vector == fixed_transform->get_origin().ptr()) {
		transform_changed();
	}
	else if (p_vector == fixed_scale.ptr()) {
		set_fixed_scale(fixed_scale);
	}
}

void SGFixedNode3D::transform_changed() {
	fixed_xform_dirty = true;
	_notify_transform();
}

SGFixedNode3D::SGFixedNode3D() {
	fixed_transform = Ref<SGFixedTransform3D>(memnew(SGFixedTransform3D));
	fixed_transform->get_origin()->set_watcher(this);

	fixed_scale = Ref<SGFixedVector2>(memnew(SGFixedVector2(SGFixedVector2Internal(fixed::ONE, fixed::ONE))));
	fixed_scale->set_watcher(this);

	fixed_rotation = 0;

	fixed_xform_dirty = false;

	set_notify_transform(true);

#ifdef TOOLS_ENABLED
	updating_transform = false;

	if (Engine::get_singleton()->is_editor_hint()) {
		add_change_receptor(this);
	}
#endif
}

SGFixedNode3D::~SGFixedNode3D() {
	fixed_transform->get_origin()->set_watcher(nullptr);
	fixed_scale->set_watcher(nullptr);
}
