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

#ifndef SG_PHYSICS_3D_SERVER_H
#define SG_PHYSICS_3D_SERVER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

#include "../math/sg_fixed_transform_3D.h"

class SGFixedNode3D;
class SGCollisionObject3D;
class SGPhysics3DServer;

class SGShape3DInternal;
class SGCollisionObject3DInternal;
class SGWorld3DInternal;

class SGAreaCollision3D : public RefCounted {
	GDCLASS(SGAreaCollision3D, RefCounted);

	SGCollisionObject3D *collider;
	RID collider_rid;
	SGFixedNode3D *shape;
	RID shape_rid;

protected:
	static void _bind_methods();

public:
	SGCollisionObject3D *get_collider() const;
	RID get_collider_rid() const;
	SGFixedNode3D *get_shape() const;
	RID get_shape_rid() const;

	SGAreaCollision3D(SGCollisionObject3D *p_object, RID p_object_rid, SGFixedNode3D *p_shape, RID p_shape_rid);
	SGAreaCollision3D();
};

class SGKinematicCollision3D : public RefCounted {
	GDCLASS(SGKinematicCollision3D, RefCounted);

	SGCollisionObject3D *collider;
	RID collider_rid;
	Ref<SGFixedVector2> normal;
	Ref<SGFixedVector2> remainder;

protected:
	static void _bind_methods();

public:
	Object *get_collider() const;
	RID get_collider_rid() const;
	Ref<SGFixedVector2> get_normal() const;
	Ref<SGFixedVector2> get_remainder() const;

	SGKinematicCollision3D(SGCollisionObject3D *p_collider, RID p_collider_rid, const Ref<SGFixedVector2> &p_normal, const Ref<SGFixedVector2> &p_remainder);
	SGKinematicCollision3D();
};

class SGRayCastCollision3D : public RefCounted {
	GDCLASS(SGRayCastCollision3D, RefCounted);

	SGCollisionObject3D *collider;
	RID collider_rid;
	Ref<SGFixedVector2> point;
	Ref<SGFixedVector2> normal;

protected:
	static void _bind_methods();

public:

	Object *get_collider() const;
	RID get_collider_rid() const;
	Ref<SGFixedVector2> get_point() const;
	Ref<SGFixedVector2> get_normal() const;

	SGRayCastCollision3D(SGCollisionObject3D *p_collider, RID p_collider_rid, const Ref<SGFixedVector2> &p_point, const Ref<SGFixedVector2> &p_normal);
	SGRayCastCollision3D();
};

class SGPhysics3DServer : public Object {
	GDCLASS(SGPhysics3DServer, Object);

	static SGPhysics3DServer *singleton;

	mutable RID_PtrOwner<SGShape3DInternal> shape_owner;
	mutable RID_PtrOwner<SGCollisionObject3DInternal> object_owner;
	mutable RID_PtrOwner<SGWorld3DInternal> world_owner;

	RID default_world;

protected:
	static void _bind_methods();

public:
	static SGPhysics3DServer *get_singleton();

	enum ShapeType {
		SHAPE_UNKNOWN = -1,
		SHAPE_RECTANGLE = 0,
		SHAPE_CIRCLE,
		SHAPE_POLYGON,
		SHAPE_CAPSULE,
	};

	enum CollisionObjectType {
		OBJECT_UNKNOWN = -1,
		OBJECT_AREA = 1,
		OBJECT_BODY = 2,
		OBJECT_BOTH = (OBJECT_AREA | OBJECT_BODY),
	};

	enum BodyType {
		BODY_UNKNOWN = -1,
		BODY_STATIC = 0,
		BODY_CHARACTER = 1,
	};

	RID shape_create(ShapeType p_shape_type);
	ShapeType shape_get_type(RID p_shape) const;
	void shape_set_data(RID p_shape, const Variant &p_data);
	// @todo Extra method used to work around godot-cpp issue.
	// @see https://github.com/godotengine/godot-cpp/issues/1039
	void shape_set_data_ptr(RID p_shape, Node *p_node);
	Variant shape_get_data(RID p_shape) const;
	void shape_set_transform(RID p_shape, const Ref<SGFixedTransform3D> &p_transform);
	Ref<SGFixedTransform3D> shape_get_transform(RID p_shape) const;

	void rectangle_set_extents(RID p_shape, const Ref<SGFixedVector2> p_extents);
	Ref<SGFixedVector2> rectangle_get_extents(RID p_shape) const;

	void circle_set_radius(RID p_shape, int64_t p_radius);
	int64_t circle_get_radius(RID p_shape) const;

	void capsule_set_radius(RID p_shape, int64_t p_radius);
	int64_t capsule_get_radius(RID p_shape) const;
	void capsule_set_height(RID p_shape, int64_t p_height);
	int64_t capsule_get_height(RID p_shape) const;

	void polygon_set_points(RID p_shape, const Array &p_points);
	Array polygon_get_points(RID p_shape) const;

	RID collision_object_create(CollisionObjectType p_object_type, BodyType p_body_type = BODY_UNKNOWN);
	CollisionObjectType collision_object_get_type(RID p_object);
	void collision_object_set_data(RID p_object, const Variant &p_data);
	// @todo Extra method used to work around godot-cpp issue.
	// @see https://github.com/godotengine/godot-cpp/issues/1039
	void collision_object_set_data_ptr(RID p_object, Node *p_node);
	Variant collision_object_get_data(RID p_object) const;
	void collision_object_set_transform(RID p_object, const Ref<SGFixedTransform3D> &p_transform);
	Ref<SGFixedTransform3D> collision_object_get_transform(RID p_object) const;
	void collision_object_add_shape(RID p_object, RID p_shape);
	void collision_object_remove_shape(RID p_object, RID p_shape);
	void collision_object_set_collision_layer(RID p_object, uint32_t p_collision_layer);
	uint32_t collision_object_get_collision_layer(RID p_object) const;
	void collision_object_set_collision_mask(RID p_object, uint32_t p_collision_mask);
	uint32_t collision_object_get_collision_mask(RID p_object) const;
	void collision_object_set_monitorable(RID p_object, bool p_monitorable);
	bool collision_object_get_monitorable(RID p_object) const;

	Array area_get_overlapping_areas(RID p_area) const;
	Array area_get_overlapping_bodies(RID p_area) const;
	Array area_get_overlapping_area_collisions(RID p_area) const;
	Array area_get_overlapping_body_collisions(RID p_area) const;
	int area_get_overlapping_area_count(RID p_area) const;
	int area_get_overlapping_body_count(RID p_area) const;

	BodyType body_get_type(RID p_body) const;
	void body_set_safe_margin(RID p_body, int p_safe_margin);
	int body_get_safe_margin(RID p_body) const;
	bool body_unstuck(RID p_body, int p_max_attempts) const;
	Ref<SGKinematicCollision3D> body_move_and_collide(RID p_body, const Ref<SGFixedVector2> &p_linear_velocity) const;

	RID world_create();
	RID get_default_world();
	void world_add_collision_object(RID p_world, RID p_object);
	void world_remove_collision_object(RID p_world, RID p_object);

	Ref<SGRayCastCollision3D> world_cast_ray(RID p_world, const Ref<SGFixedVector2> &p_start, const Ref<SGFixedVector2> &p_cast_to, uint32_t p_collision_mask, Array p_exceptions = Array(), bool p_collide_with_areas = false, bool p_collide_with_bodies = true);

	void free_rid(RID p_rid);

	// "Cheat" methods so that C++ can access the underlying internal objects.
	SGShape3DInternal *shape_get_internal(RID p_shape);
	SGCollisionObject3DInternal *collision_object_get_internal(RID p_object);
	SGWorld3DInternal *world_get_internal(RID p_world);
	SGWorld3DInternal *get_default_world_internal();

	SGPhysics3DServer();
	~SGPhysics3DServer();

};

VARIANT_ENUM_CAST(SGPhysics3DServer::ShapeType);
VARIANT_ENUM_CAST(SGPhysics3DServer::CollisionObjectType);
VARIANT_ENUM_CAST(SGPhysics3DServer::BodyType);

#endif
