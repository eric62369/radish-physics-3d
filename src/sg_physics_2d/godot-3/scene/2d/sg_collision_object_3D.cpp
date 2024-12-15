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

#include "sg_collision_object_3D.h"

#include <core/engine.h>

#include "sg_collision_shape_3D.h"
#include "sg_collision_polygon_3D.h"
#include "../../servers/sg_physics_3D_server.h"

#include "../../../internal/sg_bodies_3D_internal.h"

void SGCollisionObject3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("sync_to_physics_engine"), &SGCollisionObject3D::sync_to_physics_engine);
	ClassDB::bind_method(D_METHOD("get_rid"), &SGCollisionObject3D::get_rid);

	ClassDB::bind_method(D_METHOD("get_collision_layer"), &SGCollisionObject3D::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "collision_layer"), &SGCollisionObject3D::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &SGCollisionObject3D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &SGCollisionObject3D::set_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_layer_bit", "bit", "value"), &SGCollisionObject3D::set_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("get_collision_layer_bit"), &SGCollisionObject3D::get_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &SGCollisionObject3D::set_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_mask_bit"), &SGCollisionObject3D::get_collision_mask_bit);

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");
}

void SGCollisionObject3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			SGPhysics3DServer::get_singleton()->world_add_collision_object(world_rid, rid);
			sync_to_physics_engine();
			break;

		case NOTIFICATION_READY:
			sync_to_physics_engine();
			break;

		case NOTIFICATION_EXIT_CANVAS:
			SGPhysics3DServer::get_singleton()->world_remove_collision_object(world_rid, rid);
			break;
	}
}

String SGCollisionObject3D::get_configuration_warning() const {
	String warning = SGFixedNode3D::get_configuration_warning();

	bool has_shape_child = false;
	for (int i = 0; i < get_child_count(); i++) {
		if (Object::cast_to<SGCollisionShape3D>(get_child(i))) {
			has_shape_child = true;
			break;
		}
		if (Object::cast_to<SGCollisionPolygon3D>(get_child(i))) {
			has_shape_child = true;
			break;
		}
	}
	if (!has_shape_child) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("This node needs at least one SGCollisionShape3D as a child.");
	}

	return warning;
}

void SGCollisionObject3D::sync_from_physics_engine() {
	SGFixedTransform3DInternal physics_transform = SGPhysics3DServer::get_singleton()->collision_object_get_internal(rid)->get_transform();

	SGFixedNode3D *fixed_parent = Object::cast_to<SGFixedNode3D>(get_parent());
	if (!fixed_parent) {
		update_fixed_transform_internal(physics_transform);
		return;
	}

	SGFixedTransform3DInternal parent_transform = fixed_parent->get_global_fixed_transform_internal();
	update_fixed_transform_internal(parent_transform.affine_inverse() * physics_transform);
}

void SGCollisionObject3D::sync_to_physics_engine() const {
	for (int i = 0; i < get_child_count(); i++) {
		SGCollisionShape3D *shape = Object::cast_to<SGCollisionShape3D>(get_child(i));
		if (shape) {
			shape->sync_to_physics_engine();
		}
		else {
			SGCollisionPolygon3D *polygon = Object::cast_to<SGCollisionPolygon3D>(get_child(i));
			if (polygon) {
				polygon->sync_to_physics_engine();
			}
		}
	}

	// Update the body last, because then the shape info will be all setup to
	// be used for updating the body's broadphase element.
	SGPhysics3DServer::get_singleton()->collision_object_set_transform(rid, get_global_fixed_transform());
}

void SGCollisionObject3D::set_world(RID p_world) {
	if (world_rid != p_world) {
		if (is_inside_tree()) {
			SGPhysics3DServer::get_singleton()->world_remove_collision_object(world_rid, rid);
		}

		world_rid = p_world;

		if (is_inside_tree()) {
			SGPhysics3DServer::get_singleton()->world_add_collision_object(world_rid, rid);
		}
	}
}

uint32_t SGCollisionObject3D::get_collision_layer() const {
	return collision_layer;
}

void SGCollisionObject3D::set_collision_layer(uint32_t p_collision_layer) {
	collision_layer = p_collision_layer;
	SGPhysics3DServer::get_singleton()->collision_object_set_collision_layer(rid, collision_layer);
	_change_notify("collision_layer");
}

uint32_t SGCollisionObject3D::get_collision_mask() const {
	return collision_mask;
}
void SGCollisionObject3D::set_collision_mask(uint32_t p_collision_mask) {
	collision_mask = p_collision_mask;
	SGPhysics3DServer::get_singleton()->collision_object_set_collision_mask(rid, collision_mask);
	_change_notify("collision_mask");
}

void SGCollisionObject3D::set_collision_layer_bit(int p_bit, bool p_value) {
	uint32_t l = collision_layer;
	if (p_value) {
		l |= (1 << p_bit);
	}
	else {
		l &= ~(1 << p_bit);
	}
	set_collision_layer(l);
}

bool SGCollisionObject3D::get_collision_layer_bit(int p_bit) const {
	return get_collision_layer() & (1 << p_bit);
}

void SGCollisionObject3D::set_collision_mask_bit(int p_bit, bool p_value) {
	uint32_t m = collision_mask;
	if (p_value) {
		m |= (1 << p_bit);
	}
	else {
		m &= ~(1 << p_bit);
	}
	set_collision_mask(m);
}

bool SGCollisionObject3D::get_collision_mask_bit(int p_bit) const {
	return get_collision_mask() & (1 << p_bit);
}

SGCollisionObject3D::SGCollisionObject3D(RID p_rid) {
	SGPhysics3DServer *physics_server = SGPhysics3DServer::get_singleton();

	rid = p_rid;
	physics_server->collision_object_set_data(rid, this);
	world_rid = physics_server->get_default_world();

	collision_layer = 1;
	collision_mask = 1;
}

SGCollisionObject3D::~SGCollisionObject3D() {
	if (rid.is_valid()) {
		SGPhysics3DServer::get_singleton()->free_rid(rid);
	}
}
