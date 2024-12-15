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

#include "sg_ray_cast_3D.h"

#include <core/engine.h>
#include "sg_collision_object_3D.h"

#include "../../servers/sg_physics_3D_server.h"
#include "../../../internal/sg_world_3D_internal.h"
#include "../../../internal/sg_bodies_3D_internal.h"

void SGRayCast3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_cast_to"), &SGRayCast3D::get_cast_to);
	ClassDB::bind_method(D_METHOD("set_cast_to", "cast_to"), &SGRayCast3D::set_cast_to);
	ClassDB::bind_method(D_METHOD("_get_cast_to_x"), &SGRayCast3D::_get_cast_to_x);
	ClassDB::bind_method(D_METHOD("_set_cast_to_x", "x"), &SGRayCast3D::_set_cast_to_x);
	ClassDB::bind_method(D_METHOD("_get_cast_to_y"), &SGRayCast3D::_get_cast_to_y);
	ClassDB::bind_method(D_METHOD("_set_cast_to_y", "y"), &SGRayCast3D::_set_cast_to_y);

	// For editor and storage.
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cast_to_x"), "_set_cast_to_x", "_get_cast_to_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cast_to_y"), "_set_cast_to_y", "_get_cast_to_y");

	// For code only.
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cast_to", PROPERTY_HINT_NONE, "", 0), "set_cast_to", "get_cast_to");

	ClassDB::bind_method(D_METHOD("get_collision_mask"), &SGRayCast3D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &SGRayCast3D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &SGRayCast3D::set_collision_mask_bit);

	ClassDB::bind_method(D_METHOD("set_collide_with_areas", "collide_with_areas"), &SGRayCast3D::set_collide_with_areas);
	ClassDB::bind_method(D_METHOD("get_collide_with_areas"), &SGRayCast3D::get_collide_with_areas);
	ClassDB::bind_method(D_METHOD("set_collide_with_bodies", "collide_with_bodies"), &SGRayCast3D::set_collide_with_bodies);
	ClassDB::bind_method(D_METHOD("get_collide_with_bodies"), &SGRayCast3D::get_collide_with_bodies);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_areas"), "set_collide_with_areas", "get_collide_with_areas");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_bodies"), "set_collide_with_bodies", "get_collide_with_bodies");

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");

	ClassDB::bind_method(D_METHOD("update_raycast_collision"), &SGRayCast3D::update_raycast_collision);
	ClassDB::bind_method(D_METHOD("is_colliding"), &SGRayCast3D::is_colliding);
	ClassDB::bind_method(D_METHOD("get_collider"), &SGRayCast3D::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_rid"), &SGRayCast3D::get_collider_rid);
	ClassDB::bind_method(D_METHOD("get_collision_point"), &SGRayCast3D::get_collision_point);
	ClassDB::bind_method(D_METHOD("get_collision_normal"), &SGRayCast3D::get_collision_normal);

	ClassDB::bind_method(D_METHOD("add_exception", "object"), &SGRayCast3D::add_exception);
	ClassDB::bind_method(D_METHOD("remove_exception", "object"), &SGRayCast3D::remove_exception);
	ClassDB::bind_method(D_METHOD("get_exceptions"), &SGRayCast3D::get_exceptions);
	ClassDB::bind_method(D_METHOD("set_exceptions", "exceptions"), &SGRayCast3D::set_exceptions);
	ClassDB::bind_method(D_METHOD("clear_exceptions"), &SGRayCast3D::clear_exceptions);
}

void SGRayCast3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (!Engine::get_singleton()->is_editor_hint() && !get_tree()->is_debugging_collisions_hint())
				break;
			Transform3D xf;
			Vector2 cast_to_float = cast_to->to_float();
			xf.rotate(cast_to_float.angle());
			xf.translate(Vector2(cast_to_float.length(), 0));

			// Draw an arrow indicating where the RayCast is pointing to
			Color draw_col = get_tree()->get_debug_collisions_color();
			draw_line(Vector2(), cast_to_float, draw_col, 2, true);
			Vector<Vector2> pts;
			float tsize = 8;
			pts.push_back(xf.xform(Vector2(tsize, 0)));
			pts.push_back(xf.xform(Vector2(0, Math_SQRT12 * tsize)));
			pts.push_back(xf.xform(Vector2(0, -Math_SQRT12 * tsize)));
			Vector<Color> cols;
			for (int i = 0; i < 3; i++)
				cols.push_back(draw_col);

			draw_primitive(pts, cols, Vector<Vector2>());

		} break;
	}
}

Ref<SGFixedVector2> SGRayCast3D::get_cast_to() const {
	return cast_to;
}

void SGRayCast3D::set_cast_to(const Ref<SGFixedVector2> &p_cast_to) {
	ERR_FAIL_COND(!p_cast_to.is_valid());
	cast_to->set_internal(p_cast_to->get_internal());
	update();
	_change_notify("cast_to");
}

int64_t SGRayCast3D::_get_cast_to_x() const {
	return cast_to->get_x();
}

void SGRayCast3D::_set_cast_to_x(int64_t p_x) {
	cast_to->set_x(p_x);
	update();
	_change_notify("cast_to_x");
}

int64_t SGRayCast3D::_get_cast_to_y() const {
	return cast_to->get_y();
}

void SGRayCast3D::_set_cast_to_y(int64_t p_y) {
	cast_to->set_y(p_y);
	update();
	_change_notify("cast_to_y");
}

uint32_t SGRayCast3D::get_collision_mask() const {
	return collision_mask;
}
void SGRayCast3D::set_collision_mask(uint32_t p_collision_mask) {
	collision_mask = p_collision_mask;
	_change_notify("collision_mask");
}

void SGRayCast3D::set_collision_mask_bit(int p_bit, bool p_value) {
	uint32_t m = collision_mask;
	if (p_value) {
		m |= (1 << p_bit);
	}
	else {
		m &= ~(1 << p_bit);
	}
	set_collision_mask(m);
}

void SGRayCast3D::set_collide_with_areas(bool p_collide_with_areas){
	collide_with_areas = p_collide_with_areas;
}

bool SGRayCast3D::get_collide_with_areas() {
	return collide_with_areas;
}

void SGRayCast3D::set_collide_with_bodies(bool p_collide_with_bodies) {
	collide_with_bodies = p_collide_with_bodies;
}

bool SGRayCast3D::get_collide_with_bodies() {
	return collide_with_bodies;
}

void SGRayCast3D::update_raycast_collision() {
	SGWorld3DInternal::RayCastInfo info;

	Ref<SGFixedTransform3D> t = get_global_fixed_transform();
	Ref<SGFixedVector2> start = t->get_origin()->copy();
	t->get_origin()->clear();

	Ref<SGRayCastCollision3D> collision = SGPhysics3DServer::get_singleton()->world_cast_ray(world_rid, start, t->xform(cast_to), collision_mask, exceptions, collide_with_areas, collide_with_bodies);
	if (collision.is_valid()) {
		colliding = true;
		Object *collider_obj = collision->get_collider();
		collider = collider_obj ? collider_obj->get_instance_id() : 0;
		collider_rid = collision->get_collider_rid();
		collision_point = collision->get_point();
		collision_normal = collision->get_normal();
	} else {
		colliding = false;
		collider = 0;
		collider_rid = RID();
		collision_point->clear();
		collision_normal->clear();
	}
}

bool SGRayCast3D::is_colliding() const {
	return colliding;
}

Object *SGRayCast3D::get_collider() const {
	if (!collider) {
		return nullptr;
	}
	return ObjectDB::get_instance(collider);
}

RID SGRayCast3D::get_collider_rid() const {
	return collider_rid;
}

Ref<SGFixedVector2> SGRayCast3D::get_collision_point() const {
	return collision_point;
}

Ref<SGFixedVector2> SGRayCast3D::get_collision_normal() const {
	return collision_normal;
}

void SGRayCast3D::add_exception(const Object *p_object) {
	const SGCollisionObject3D *collision_object = Object::cast_to<SGCollisionObject3D>(p_object);
	if (collision_object) {
		exceptions.push_back(SGPhysics3DServer::get_singleton()->collision_object_get_internal(collision_object->get_rid()));
	}
}

void SGRayCast3D::remove_exception(const Object *p_object) {
	const SGCollisionObject3D *collision_object = Object::cast_to<SGCollisionObject3D>(p_object);
	if (p_object) {
		exceptions.erase(SGPhysics3DServer::get_singleton()->collision_object_get_internal(collision_object->get_rid()));
	}
}

Array SGRayCast3D::get_exceptions() const {
	return exceptions;
}

void SGRayCast3D::set_exceptions(const Array &p_exceptions) {
	exceptions.clear();
	for (int i = 0; i < p_exceptions.size(); i++) {
		add_exception(p_exceptions[i]);
	}
}

void SGRayCast3D::clear_exceptions() {
	exceptions.clear();
}

void SGRayCast3D::set_world(RID p_world) {
	world_rid = p_world;
}

SGRayCast3D::SGRayCast3D() {
	// Start casting to (0, 50) like Godot's RayCast3D.
	cast_to = Ref<SGFixedVector2>(memnew(SGFixedVector2(SGFixedVector2Internal(fixed::ZERO, fixed(3276800)))));
	collision_mask = 1;

	colliding = false;
	collider = 0;
	collision_point = Ref<SGFixedVector2>(memnew(SGFixedVector2));
	collision_normal = Ref<SGFixedVector2>(memnew(SGFixedVector2));

	world_rid = SGPhysics3DServer::get_singleton()->get_default_world();
}

SGRayCast3D::~SGRayCast3D() {
}
