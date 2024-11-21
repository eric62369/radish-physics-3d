
#include "sg_physics_3d_server.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/method_bind.hpp>

#include "../scene/3d/sg_collision_object_3d.h"
#include "../../internal/sg_shapes_3d_internal.h"
#include "../../internal/sg_bodies_3d_internal.h"
#include "../../internal/sg_world_3d_internal.h"

struct SGInternalData {
	enum DataType {
		TYPE_NULL,
		TYPE_STRING,
		TYPE_OBJECT,
	};

	RID rid;
	// @todo This should really just hold a Variant, we we need to break this up for a godot-cpp bug.
	// @see https://github.com/godotengine/godot-cpp/issues/1039
	DataType type = DataType::TYPE_NULL;
	Object *object = nullptr;
	String string;

	SGInternalData(RID p_rid) : rid(p_rid) {}

	void set_data(Object *p_object) {
		if (p_object != nullptr) {
			type = DataType::TYPE_OBJECT;
			object = p_object;
		} else {
			type = DataType::TYPE_NULL;
			object = nullptr;
		}
		string = "";
	}

	void set_data(String p_string) {
		type = DataType::TYPE_STRING;
		string = p_string;
		object = nullptr;
	}

	void set_data_from_variant(const Variant &p_variant) {
		if (p_variant.get_type() == Variant::OBJECT) {
			set_data((Object *)p_variant);
		}
		else if (p_variant.get_type() == Variant::STRING) {
			set_data((String)p_variant);
		} else {
			set_data(nullptr);
		}
	}

	bool is_greater_than(const SGInternalData &p_other) {
		DataType a_t = type;
		DataType b_t = p_other.type;
		if (a_t == DataType::TYPE_NULL || b_t == DataType::TYPE_NULL) {
			WARN_PRINT("Can't sort physics objects deterministically if data is null");
		}
		if (a_t == b_t) {
			if (a_t == DataType::TYPE_OBJECT) {
				// These have to be Node's because we check when setting the data.
				Node *a = Object::cast_to<Node>(object);
				Node *b = Object::cast_to<Node>(p_other.object);
				return a->is_greater_than(b);
			}
			if (a_t == DataType::TYPE_STRING) {
				return string.casecmp_to(p_other.string) == 1;
			}
			return true;
		}

		if (a_t == DataType::TYPE_OBJECT) {
			// Push objects to the start of the list.
			return false;
		}
		if (a_t == DataType::TYPE_STRING) {
			// Put strings after objects.
			return b_t == DataType::TYPE_OBJECT ? true : false;
		}
		if (a_t == DataType::TYPE_NULL) {
			// Put null after strings.
			return true;
		}

		// This should never be reached, but if anything else gets in there,
		// then push it to the end of the list.
		return true;
	}

	Object *get_object() {
		if (type == DataType::TYPE_OBJECT) {
			return object;
		}
		return nullptr;
	}

	Variant get_data() {
		if (type == DataType::TYPE_OBJECT) {
			return Variant(object);
		}
		else if (type == DataType::TYPE_STRING) {
			return Variant(string);
		}
		return Variant();
	}
};

SGPhysics3dServer *SGPhysics3dServer::singleton = nullptr;

static bool sg_compare_collision_objects(const SGCollisionObject3dInternal* p_a, const SGCollisionObject3dInternal *p_b) {
	SGInternalData *a = (SGInternalData *)p_a->get_data();
	SGInternalData *b = (SGInternalData *)p_b->get_data();
	return b->is_greater_than(*a);
}

struct SGCollisionObjectComparator {
	bool operator()(const SGCollisionObject3dInternal *p_a, const SGCollisionObject3dInternal *p_b) const {
		return sg_compare_collision_objects(p_a, p_b);
	}
};

struct SGAreaCollision3dComparator {
	bool operator()(const SGAreaCollision3d *p_a, const SGAreaCollision3d *p_b) const {
		SGPhysics3dServer *physics_server = SGPhysics3dServer::get_singleton();

		SGCollisionObject3d *a = Object::cast_to<SGCollisionObject3d>((Object *)p_a->get_collider());
		SGCollisionObject3d *b = Object::cast_to<SGCollisionObject3d>((Object *)p_b->get_collider());

		return sg_compare_collision_objects(physics_server->collision_object_get_internal(a->get_rid()), physics_server->collision_object_get_internal(b->get_rid()));
	}
};

void SGAreaCollision3d::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_collider"), &SGAreaCollision3d::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_rid"), &SGAreaCollision3d::get_collider_rid);
	ClassDB::bind_method(D_METHOD("get_shape"), &SGAreaCollision3d::get_shape);
	ClassDB::bind_method(D_METHOD("get_shape_rid"), &SGAreaCollision3d::get_shape_rid);


	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider"), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider_rid"), "", "get_collider_rid");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape"), "", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape_rid"), "", "get_shape_rid");
}

SGCollisionObject3d *SGAreaCollision3d::get_collider() const {
	return collider;
}

SGFixedNode3d *SGAreaCollision3d::get_shape() const {
	return shape;
}

RID SGAreaCollision3d::get_collider_rid() const {
	return collider_rid;
}

RID SGAreaCollision3d::get_shape_rid() const {
	return shape_rid;
}

SGAreaCollision3d::SGAreaCollision3d(SGCollisionObject3d *p_object, RID p_object_rid, SGFixedNode3d *p_shape, RID p_shape_rid) {
	collider = p_object;
	collider_rid = p_object_rid;
	shape = p_shape;
	shape_rid = p_shape_rid;
}

SGAreaCollision3d::SGAreaCollision3d() {
	collider = nullptr;
	shape = nullptr;
}

void SGKinematicCollision3d::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_collider"), &SGKinematicCollision3d::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_rid"), &SGKinematicCollision3d::get_collider_rid);
	ClassDB::bind_method(D_METHOD("get_normal"), &SGKinematicCollision3d::get_normal);
	ClassDB::bind_method(D_METHOD("get_remainder"), &SGKinematicCollision3d::get_remainder);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider", PROPERTY_HINT_NONE, "", 0), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider_rid", PROPERTY_HINT_NONE, "", 0), "", "get_collider_rid");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal", PROPERTY_HINT_NONE, "", 0), "", "get_normal");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "remainder", PROPERTY_HINT_NONE, "", 0), "", "get_remainder");
}

Object *SGKinematicCollision3d::get_collider() const {
	return collider;
}

RID SGKinematicCollision3d::get_collider_rid() const {
	return collider_rid;
}

Ref<SGFixedVector2> SGKinematicCollision3d::get_normal() const {
	return normal;
}

Ref<SGFixedVector2> SGKinematicCollision3d::get_remainder() const {
	return remainder;
}

SGKinematicCollision3d::SGKinematicCollision3d(SGCollisionObject3d *p_collider, RID p_collider_rid, const Ref<SGFixedVector2> &p_normal, const Ref<SGFixedVector2> &p_remainder) {
	collider = p_collider;
	collider_rid = p_collider_rid;
	normal = p_normal;
	remainder = p_remainder;
}

SGKinematicCollision3d::SGKinematicCollision3d() {
	collider = nullptr;
}

void SGRayCastCollision3d::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_collider"), &SGRayCastCollision3d::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_rid"), &SGRayCastCollision3d::get_collider_rid);
	ClassDB::bind_method(D_METHOD("get_point"), &SGRayCastCollision3d::get_point);
	ClassDB::bind_method(D_METHOD("get_normal"), &SGRayCastCollision3d::get_normal);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider", PROPERTY_HINT_NONE, "", 0), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider_rid", PROPERTY_HINT_NONE, "", 0), "", "get_collider_rid");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "point", PROPERTY_HINT_NONE, "", 0), "", "get_point");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal", PROPERTY_HINT_NONE, "", 0), "", "get_normal");
}

Object *SGRayCastCollision3d::get_collider() const {
	return collider;
}

RID SGRayCastCollision3d::get_collider_rid() const {
	return collider_rid;
}

Ref<SGFixedVector2> SGRayCastCollision3d::get_point() const {
	return point;
}

Ref<SGFixedVector2> SGRayCastCollision3d::get_normal() const {
	return normal;
}

SGRayCastCollision3d::SGRayCastCollision3d(SGCollisionObject3d *p_collider, RID p_collider_rid, const Ref<SGFixedVector2> &p_point, const Ref<SGFixedVector2> &p_normal) {
	collider = p_collider;
	collider_rid = p_collider_rid;
	point = p_point;
	normal = p_normal;
}

SGRayCastCollision3d::SGRayCastCollision3d() {
	collider = nullptr;
}

void SGPhysics3dServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("shape_create", "shape_type"), &SGPhysics3dServer::shape_create);
	ClassDB::bind_method(D_METHOD("shape_get_type", "shape"), &SGPhysics3dServer::shape_get_type);
	ClassDB::bind_method(D_METHOD("shape_set_data", "shape", "data"), &SGPhysics3dServer::shape_set_data);
	ClassDB::bind_method(D_METHOD("shape_get_data", "shape"), &SGPhysics3dServer::shape_get_data);
	ClassDB::bind_method(D_METHOD("shape_set_transform", "shape", "transform"), &SGPhysics3dServer::shape_set_transform);
	ClassDB::bind_method(D_METHOD("shape_get_transform", "shape"), &SGPhysics3dServer::shape_get_transform);

	ClassDB::bind_method(D_METHOD("rectangle_set_extents", "shape", "extents"), &SGPhysics3dServer::rectangle_set_extents);
	ClassDB::bind_method(D_METHOD("rectangle_get_extents", "shape"), &SGPhysics3dServer::rectangle_get_extents);

	ClassDB::bind_method(D_METHOD("circle_set_radius", "shape", "radius"), &SGPhysics3dServer::circle_set_radius);
	ClassDB::bind_method(D_METHOD("circle_get_radius", "shape"), &SGPhysics3dServer::circle_get_radius);

	ClassDB::bind_method(D_METHOD("polygon_set_points", "shape", "points"), &SGPhysics3dServer::polygon_set_points);
	ClassDB::bind_method(D_METHOD("polygon_get_points", "shape"), &SGPhysics3dServer::polygon_get_points);

	ClassDB::bind_method(D_METHOD("collision_object_create", "object_type", "body_type"), &SGPhysics3dServer::collision_object_create, DEFVAL(BODY_UNKNOWN));
	ClassDB::bind_method(D_METHOD("collision_object_get_type", "object"), &SGPhysics3dServer::collision_object_get_type);
	ClassDB::bind_method(D_METHOD("collision_object_set_data", "object", "data"), &SGPhysics3dServer::collision_object_set_data);
	ClassDB::bind_method(D_METHOD("collision_object_get_data", "object"), &SGPhysics3dServer::collision_object_get_data);
	ClassDB::bind_method(D_METHOD("collision_object_set_transform", "object", "transform"), &SGPhysics3dServer::collision_object_set_transform);
	ClassDB::bind_method(D_METHOD("collision_object_get_transform", "object"), &SGPhysics3dServer::collision_object_get_transform);
	ClassDB::bind_method(D_METHOD("collision_object_add_shape", "object", "shape"), &SGPhysics3dServer::collision_object_add_shape);
	ClassDB::bind_method(D_METHOD("collision_object_remove_shape", "object", "shape"), &SGPhysics3dServer::collision_object_remove_shape);
	ClassDB::bind_method(D_METHOD("collision_object_set_collision_layer", "object", "layer"), &SGPhysics3dServer::collision_object_set_collision_layer);
	ClassDB::bind_method(D_METHOD("collision_object_get_collision_layer", "object"), &SGPhysics3dServer::collision_object_get_collision_layer);
	ClassDB::bind_method(D_METHOD("collision_object_set_collision_mask", "object", "mask"), &SGPhysics3dServer::collision_object_set_collision_mask);
	ClassDB::bind_method(D_METHOD("collision_object_get_collision_mask", "object"), &SGPhysics3dServer::collision_object_get_collision_mask);
	ClassDB::bind_method(D_METHOD("collision_object_set_monitorable", "object", "monitorable"), &SGPhysics3dServer::collision_object_set_monitorable);
	ClassDB::bind_method(D_METHOD("collision_object_get_monitorable", "object"), &SGPhysics3dServer::collision_object_get_monitorable);

	ClassDB::bind_method(D_METHOD("area_get_overlapping_areas", "area"), &SGPhysics3dServer::area_get_overlapping_areas);
	ClassDB::bind_method(D_METHOD("area_get_overlapping_bodies", "area"), &SGPhysics3dServer::area_get_overlapping_bodies);
	ClassDB::bind_method(D_METHOD("area_get_overlapping_area_collisions", "area"), &SGPhysics3dServer::area_get_overlapping_area_collisions);
	ClassDB::bind_method(D_METHOD("area_get_overlapping_body_collisions", "area"), &SGPhysics3dServer::area_get_overlapping_body_collisions);
	ClassDB::bind_method(D_METHOD("area_get_overlapping_area_count", "area"), &SGPhysics3dServer::area_get_overlapping_area_count);
	ClassDB::bind_method(D_METHOD("area_get_overlapping_body_count", "area"), &SGPhysics3dServer::area_get_overlapping_body_count);

	ClassDB::bind_method(D_METHOD("body_get_type", "body"), &SGPhysics3dServer::body_get_type);
	ClassDB::bind_method(D_METHOD("body_set_safe_margin", "body", "safe_margin"), &SGPhysics3dServer::body_set_safe_margin);
	ClassDB::bind_method(D_METHOD("body_get_safe_margin", "body"), &SGPhysics3dServer::body_get_safe_margin);
	ClassDB::bind_method(D_METHOD("body_unstuck", "body", "max_attempts"), &SGPhysics3dServer::body_unstuck);
	ClassDB::bind_method(D_METHOD("body_move_and_collide", "body", "linear_velocity"), &SGPhysics3dServer::body_move_and_collide);

	ClassDB::bind_method(D_METHOD("world_create"), &SGPhysics3dServer::world_create);
	ClassDB::bind_method(D_METHOD("get_default_world"), &SGPhysics3dServer::get_default_world);
	ClassDB::bind_method(D_METHOD("world_add_collision_object", "world", "object"), &SGPhysics3dServer::world_add_collision_object);
	ClassDB::bind_method(D_METHOD("world_remove_collision_object", "world", "object"), &SGPhysics3dServer::world_remove_collision_object);

	ClassDB::bind_method(D_METHOD("world_cast_ray", "world", "start", "cast_to", "collision_mask", "exceptions", "collide_with_areas", "collide_with_bodies"), &SGPhysics3dServer::world_cast_ray, DEFVAL(Array()), DEFVAL(false), DEFVAL(true));

	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &SGPhysics3dServer::free_rid);

	BIND_ENUM_CONSTANT(SHAPE_UNKNOWN);
	BIND_ENUM_CONSTANT(SHAPE_RECTANGLE);
	BIND_ENUM_CONSTANT(SHAPE_CIRCLE);
	BIND_ENUM_CONSTANT(SHAPE_POLYGON);
	BIND_ENUM_CONSTANT(SHAPE_CAPSULE);

	BIND_ENUM_CONSTANT(OBJECT_UNKNOWN);
	BIND_ENUM_CONSTANT(OBJECT_AREA);
	BIND_ENUM_CONSTANT(OBJECT_BODY);
	BIND_ENUM_CONSTANT(OBJECT_BOTH);

	BIND_ENUM_CONSTANT(BODY_UNKNOWN);
	BIND_ENUM_CONSTANT(BODY_STATIC);
	BIND_ENUM_CONSTANT(BODY_CHARACTER);
}

SGPhysics3dServer *SGPhysics3dServer::get_singleton() {
	return singleton;
}

RID SGPhysics3dServer::shape_create(SGPhysics3dServer::ShapeType p_shape_type) {
	ERR_FAIL_COND_V(p_shape_type == SHAPE_UNKNOWN, RID());
	SGShape3dInternal *shape = nullptr;
	switch (p_shape_type) {
		case SHAPE_RECTANGLE: {
			shape = memnew(SGRectangle3dInternal(SGFixedVector2Internal(fixed(655360), fixed(655360))));
		} break;
		case SHAPE_CIRCLE: {
			shape = memnew(SGCircle3dInternal(fixed(655360)));
		}
		break;
		case SHAPE_CAPSULE: {
			shape = memnew(SGCapsule3dInternal(fixed(655360), fixed(655360)));
		}
		break;
		case SHAPE_POLYGON: {
			shape = memnew(SGPolygon3dInternal);
		}
		break;
		default:
			ERR_FAIL_V_MSG(RID(), "Unknown shape type");
	}
	RID id = shape_owner.make_rid(shape);
	shape->set_data(new SGInternalData(id));
	return id;
}

SGPhysics3dServer::ShapeType SGPhysics3dServer::shape_get_type(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, SHAPE_UNKNOWN);
	return (SGPhysics3dServer::ShapeType)internal->get_shape_type();
}

void SGPhysics3dServer::shape_set_data(RID p_shape, const Variant &p_data) {
	Variant::Type t = p_data.get_type();
	ERR_FAIL_COND_MSG(!(t == Variant::OBJECT && Object::cast_to<Node>(p_data) != nullptr) && t != Variant::STRING && t != Variant::NIL, "Data can only be Node, string or null");

	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	((SGInternalData *)internal->get_data())->set_data_from_variant(p_data);
}

void SGPhysics3dServer::shape_set_data_ptr(RID p_shape, Node *p_node) {
	ERR_FAIL_COND(p_node == nullptr);

	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	((SGInternalData *)internal->get_data())->set_data(p_node);
}

Variant SGPhysics3dServer::shape_get_data(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, Variant());
	return ((SGInternalData *)internal->get_data())->get_data();
}

void SGPhysics3dServer::shape_set_transform(RID p_shape, const Ref<SGFixedTransform3d> &p_transform) {
	ERR_FAIL_COND(p_transform.is_null());
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	internal->set_transform(p_transform->get_internal());
}

Ref<SGFixedTransform3d> SGPhysics3dServer::shape_get_transform(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, Ref<SGFixedTransform3d>());
	return SGFixedTransform3d::from_internal(internal->get_transform());
}

void SGPhysics3dServer::rectangle_set_extents(RID p_shape, const Ref<SGFixedVector2> p_extents) {
	ERR_FAIL_COND(p_extents.is_null());
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_shape_type() != SGShape3dInternal::SHAPE_RECTANGLE);
	((SGRectangle3dInternal *)internal)->set_extents(p_extents->get_internal());
}

Ref<SGFixedVector2> SGPhysics3dServer::rectangle_get_extents(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, Ref<SGFixedVector2>());
	ERR_FAIL_COND_V(internal->get_shape_type() != SGShape3dInternal::SHAPE_RECTANGLE, Ref<SGFixedVector2>());
	return SGFixedVector2::from_internal(((SGRectangle3dInternal *)internal)->get_extents());
}

void SGPhysics3dServer::circle_set_radius(RID p_shape, int64_t p_radius) {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_shape_type() != SGShape3dInternal::SHAPE_CIRCLE);
	((SGCircle3dInternal *)internal)->set_radius(fixed(p_radius));
}

int64_t SGPhysics3dServer::circle_get_radius(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, 0);
	ERR_FAIL_COND_V(internal->get_shape_type() != SGShape3dInternal::SHAPE_CIRCLE, 0);
	return ((SGCircle3dInternal *)internal)->get_radius().value;
}

void SGPhysics3dServer::capsule_set_radius(RID p_shape, int64_t p_radius) {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_shape_type() != SGShape3dInternal::SHAPE_CAPSULE);
	((SGCapsule3dInternal *)internal)->set_radius(fixed(p_radius));
}

int64_t SGPhysics3dServer::capsule_get_radius(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, 0);
	ERR_FAIL_COND_V(internal->get_shape_type() != SGShape3dInternal::SHAPE_CAPSULE, 0);
	return ((SGCapsule3dInternal *)internal)->get_radius().value;
}

void SGPhysics3dServer::capsule_set_height(RID p_shape, int64_t p_height) {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_shape_type() != SGShape3dInternal::SHAPE_CAPSULE);
	((SGCapsule3dInternal *)internal)->set_height(fixed(p_height));
}

int64_t SGPhysics3dServer::capsule_get_height(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, 0);
	ERR_FAIL_COND_V(internal->get_shape_type() != SGShape3dInternal::SHAPE_CAPSULE, 0);
	return ((SGCapsule3dInternal *)internal)->get_height().value;
}

void SGPhysics3dServer::polygon_set_points(RID p_shape, const Array &p_points_array) {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_shape_type() != SGShape3dInternal::SHAPE_POLYGON);

	std::vector<SGFixedVector2Internal> points;
	points.resize(p_points_array.size());

	for (int i = 0; i < p_points_array.size(); i++) {
		Ref<SGFixedVector2> point = p_points_array[i];
		if (point.is_valid()) {
			points[i] = point->get_internal();
		}
	}

	((SGPolygon3dInternal *)internal)->set_points(points);
}

Array SGPhysics3dServer::polygon_get_points(RID p_shape) const {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, Array());
	ERR_FAIL_COND_V(internal->get_shape_type() != SGShape3dInternal::SHAPE_POLYGON, Array());

	Array ret;
	std::vector<SGFixedVector2Internal> points = ((SGPolygon3dInternal *)internal)->get_points();
	ret.resize(points.size());
	for (std::size_t i = 0; i < points.size(); i++) {
		ret[i] = SGFixedVector2::from_internal(points[i]);
	}
	return ret;
}

RID SGPhysics3dServer::collision_object_create(SGPhysics3dServer::CollisionObjectType p_object_type, SGPhysics3dServer::BodyType p_body_type) {
	ERR_FAIL_COND_V(p_object_type == OBJECT_BOTH, RID());
	ERR_FAIL_COND_V(p_object_type == OBJECT_BODY && p_body_type == BODY_UNKNOWN, RID());
	SGCollisionObject3dInternal *object;
	switch (p_object_type) {
		case OBJECT_AREA: {
			object = memnew(SGArea3dInternal());
		} break;
		case OBJECT_BODY: {
			object = memnew(SGBody3dInternal((SGBody3dInternal::BodyType)p_body_type));
		} break;
		default:
			ERR_FAIL_V_MSG(RID(), "Unknown collision object type");
	}
	RID id = object_owner.make_rid(object);
	object->set_data(new SGInternalData(id));
	return id;
}

SGPhysics3dServer::CollisionObjectType SGPhysics3dServer::collision_object_get_type(RID p_object) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, OBJECT_UNKNOWN);
	return (SGPhysics3dServer::CollisionObjectType)internal->get_object_type();
}

void SGPhysics3dServer::collision_object_set_data(RID p_object, const Variant &p_data) {
	Variant::Type t = p_data.get_type();
	ERR_FAIL_COND_MSG(!(t == Variant::OBJECT && Object::cast_to<Node>(p_data) != nullptr) && t != Variant::STRING && t != Variant::NIL, "Data can only be Node, string or null");

	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	((SGInternalData *)internal->get_data())->set_data_from_variant(p_data);
}

void SGPhysics3dServer::collision_object_set_data_ptr(RID p_object, Node *p_node) {
	ERR_FAIL_COND(p_node == nullptr);

	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	((SGInternalData *)internal->get_data())->set_data(p_node);
}

Variant SGPhysics3dServer::collision_object_get_data(RID p_object) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, Variant());
	return ((SGInternalData *)internal->get_data())->get_data();
}

void SGPhysics3dServer::collision_object_set_transform(RID p_object, const Ref<SGFixedTransform3d> &p_transform) {
	ERR_FAIL_COND(p_transform.is_null());
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	internal->set_transform(p_transform->get_internal());
}

Ref<SGFixedTransform3d> SGPhysics3dServer::collision_object_get_transform(RID p_object) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, Ref<SGFixedTransform3d>());
	return SGFixedTransform3d::from_internal(internal->get_transform());
}

void SGPhysics3dServer::collision_object_add_shape(RID p_object, RID p_shape) {
	SGCollisionObject3dInternal *object_internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!object_internal);
	SGShape3dInternal *shape_internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!shape_internal);

	object_internal->add_shape(shape_internal);
}

void SGPhysics3dServer::collision_object_remove_shape(RID p_object, RID p_shape) {
	SGCollisionObject3dInternal *object_internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!object_internal);
	SGShape3dInternal *shape_internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!shape_internal);

	object_internal->remove_shape(shape_internal);
}

void SGPhysics3dServer::collision_object_set_collision_layer(RID p_object, uint32_t p_collision_layer) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	internal->set_collision_layer(p_collision_layer);
}

uint32_t SGPhysics3dServer::collision_object_get_collision_layer(RID p_object) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, 0);
	return internal->get_collision_layer();
}

void SGPhysics3dServer::collision_object_set_collision_mask(RID p_object, uint32_t p_collision_mask) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	internal->set_collision_mask(p_collision_mask);
}

uint32_t SGPhysics3dServer::collision_object_get_collision_mask(RID p_object) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, 0);
	return internal->get_collision_mask();
}

void SGPhysics3dServer::collision_object_set_monitorable(RID p_object, bool p_monitorable) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!internal);
	internal->set_monitorable(p_monitorable);
}

bool SGPhysics3dServer::collision_object_get_monitorable(RID p_object) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, false);
	return internal->get_monitorable();
}

class SGSortedArrayResultHandler : public SGResultHandlerInternal {
private:

	List<SGCollisionObject3dInternal *> result;

public:
	void handle_result(SGCollisionObject3dInternal *p_object, SGShape3dInternal *p_shape) {
		result.push_back(p_object);
	}

	_FORCE_INLINE_ Array get_array() {
		if (result.size() > 1) {
			result.sort_custom<SGCollisionObjectComparator>();
		}

		Array ret;
		for (List<SGCollisionObject3dInternal *>::Element *E = result.front(); E; E = E->next()) {
			SGInternalData *object_data = (SGInternalData *)E->get()->get_data();
			SGCollisionObject3d *object = Object::cast_to<SGCollisionObject3d>(object_data->get_object());
			if (object) {
				ret.push_back(object);
			} else {
				ret.push_back(object_data->rid);
			}
		}

		return ret;
	}
};

Array SGPhysics3dServer::area_get_overlapping_areas(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, Array());
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, Array());

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), Array());

	SGSortedArrayResultHandler result_handler;
	internal->get_world()->get_overlapping_areas(internal, &result_handler);
	return result_handler.get_array();
}

Array SGPhysics3dServer::area_get_overlapping_bodies(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, Array());
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, Array());

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), Array());

	SGSortedArrayResultHandler result_handler;
	internal->get_world()->get_overlapping_bodies(internal, &result_handler);
	return result_handler.get_array();
}

class SGSortedCollisionArrayResultHandler : public SGResultHandlerInternal {
private:

	List<SGAreaCollision3d *> result;

public:
	void handle_result(SGCollisionObject3dInternal *p_object, SGShape3dInternal *p_shape) {
		SGInternalData *object_data = (SGInternalData *)p_object->get_data();
		SGInternalData *shape_data = (SGInternalData *)p_shape->get_data();
		SGAreaCollision3d *collision = memnew(SGAreaCollision3d(
			Object::cast_to<SGCollisionObject3d>(object_data->get_object()),
			object_data->rid,
			Object::cast_to<SGFixedNode3d>(shape_data->get_object()),
			shape_data->rid
		));
		result.push_back(collision);
	}

	_FORCE_INLINE_ Array get_array() {
		if (result.size() > 1) {
			result.sort_custom<SGAreaCollision3dComparator>();
		}

		Array ret;
		for (List<SGAreaCollision3d *>::Element *E = result.front(); E; E = E->next()) {
			ret.push_back(E->get());
		}

		return ret;
	}
};

Array SGPhysics3dServer::area_get_overlapping_area_collisions(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, Array());
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, Array());

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), Array());

	SGSortedCollisionArrayResultHandler result_handler;
	internal->get_world()->get_overlapping_areas(internal, &result_handler);
	return result_handler.get_array();
}

Array SGPhysics3dServer::area_get_overlapping_body_collisions(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, Array());
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, Array());

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), Array());

	SGSortedCollisionArrayResultHandler result_handler;
	internal->get_world()->get_overlapping_bodies(internal, &result_handler);
	return result_handler.get_array();
}

class SGCountResultHandler : public SGResultHandlerInternal {
private:

	int count = 0;

public:
	void handle_result(SGCollisionObject3dInternal *p_object, SGShape3dInternal *p_shape) {
		count++;
	}

	_FORCE_INLINE_ int get_count() {
		return count;
	}
};

int SGPhysics3dServer::area_get_overlapping_area_count(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, 0);
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, 0);

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), 0);

	SGCountResultHandler result_handler;
	internal->get_world()->get_overlapping_areas(internal, &result_handler);
	return result_handler.get_count();
}

int SGPhysics3dServer::area_get_overlapping_body_count(RID p_area) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_area);
	ERR_FAIL_COND_V(!object, 0);
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_AREA, 0);

	SGArea3dInternal *internal = (SGArea3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), 0);

	SGCountResultHandler result_handler;
	internal->get_world()->get_overlapping_bodies(internal, &result_handler);
	return result_handler.get_count();
}

SGPhysics3dServer::BodyType SGPhysics3dServer::body_get_type(RID p_body) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!internal, BODY_UNKNOWN);
	ERR_FAIL_COND_V(internal->get_object_type() != SGCollisionObject3dInternal::OBJECT_BODY, BODY_UNKNOWN);
	return (SGPhysics3dServer::BodyType)((SGBody3dInternal *)internal)->get_body_type();
}

void SGPhysics3dServer::body_set_safe_margin(RID p_body, int p_safe_margin) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_body);
	ERR_FAIL_COND(!internal);
	ERR_FAIL_COND(internal->get_object_type() != SGCollisionObject3dInternal::OBJECT_BODY);
	((SGBody3dInternal *)internal)->set_safe_margin(fixed(p_safe_margin));
}

int SGPhysics3dServer::body_get_safe_margin(RID p_body) const {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!internal, 0);
	ERR_FAIL_COND_V(internal->get_object_type() != SGCollisionObject3dInternal::OBJECT_BODY, 0);
	return ((SGBody3dInternal *)internal)->get_safe_margin().value;
}

bool SGPhysics3dServer::body_unstuck(RID p_body, int p_max_attempts) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!object, false);
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_BODY, false);

	SGBody3dInternal *internal = (SGBody3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), false);

	return internal->get_world()->unstuck_body(internal, p_max_attempts);
}

Ref<SGKinematicCollision3d> SGPhysics3dServer::body_move_and_collide(RID p_body, const Ref<SGFixedVector2> &p_linear_velocity) const {
	SGCollisionObject3dInternal *object = object_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!object, Ref<SGKinematicCollision3d>());
	ERR_FAIL_COND_V(object->get_object_type() != SGCollisionObject3dInternal::OBJECT_BODY, Ref<SGKinematicCollision3d>());

	SGBody3dInternal *internal = (SGBody3dInternal *)object;
	ERR_FAIL_COND_V(!internal->get_world(), Ref<SGKinematicCollision3d>());

	SGWorld3dInternal::BodyCollisionInfo collision;
	if (internal->get_world()->move_and_collide(internal, p_linear_velocity->get_internal(), &collision)) {
		SGInternalData *object_data = (SGInternalData *)collision.collider->get_data();
		SGCollisionObject3d *object = Object::cast_to<SGCollisionObject3d>(object_data->get_object());
		Ref<SGKinematicCollision3d> result = Ref<SGKinematicCollision3d>(memnew(SGKinematicCollision3d(
			object,
			object_data->rid,
			SGFixedVector2::from_internal(collision.normal),
			SGFixedVector2::from_internal(collision.remainder)
		)));
		return result;
	}

	return Ref<SGKinematicCollision3d>();
}

RID SGPhysics3dServer::world_create() {
	int cell_size = 128;
	if (ProjectSettings::get_singleton()->has_setting("physics/3d/cell_size")) {
		cell_size = ProjectSettings::get_singleton()->get_setting("physics/3d/cell_size");
	}

	SGWorld3dInternal *world = memnew(SGWorld3dInternal(cell_size, &sg_compare_collision_objects));
	return world_owner.make_rid(world);
}

RID SGPhysics3dServer::get_default_world() {
	return default_world;
}

void SGPhysics3dServer::world_add_collision_object(RID p_world, RID p_object) {
	SGWorld3dInternal *world_internal = world_owner.get_or_null(p_world);
	ERR_FAIL_COND(!world_internal);

	SGCollisionObject3dInternal *object_internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!object_internal);

	if (object_internal->get_object_type() == SGCollisionObject3dInternal::OBJECT_BODY) {
		world_internal->add_body((SGBody3dInternal *)object_internal);
	}
	else if (object_internal->get_object_type() == SGCollisionObject3dInternal::OBJECT_AREA) {
		world_internal->add_area((SGArea3dInternal *)object_internal);
	}
}

void SGPhysics3dServer::world_remove_collision_object(RID p_world, RID p_object) {
	SGWorld3dInternal *world_internal = world_owner.get_or_null(p_world);
	ERR_FAIL_COND(!world_internal);

	SGCollisionObject3dInternal *object_internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND(!object_internal);

	if (object_internal->get_object_type() == SGCollisionObject3dInternal::OBJECT_BODY) {
		world_internal->remove_body((SGBody3dInternal *)object_internal);
	}
	else if (object_internal->get_object_type() == SGCollisionObject3dInternal::OBJECT_AREA) {
		world_internal->remove_area((SGArea3dInternal *)object_internal);
	}
}

Ref<SGRayCastCollision3d> SGPhysics3dServer::world_cast_ray(RID p_world, const Ref<SGFixedVector2> &p_start, const Ref<SGFixedVector2> &p_cast_to, uint32_t p_collision_mask, Array p_exceptions, bool p_collide_with_areas, bool p_collide_with_bodies) {
	SGWorld3dInternal *internal = world_owner.get_or_null(p_world);
	ERR_FAIL_COND_V(!internal, Ref<SGRayCastCollision3d>());

	std::unordered_set<SGCollisionObject3dInternal *> exceptions;
	for (int i = 0; i < p_exceptions.size(); i++) {
		SGCollisionObject3d *object = Object::cast_to<SGCollisionObject3d>(p_exceptions[i]);
		if (object) {
			exceptions.insert(collision_object_get_internal(object->get_rid()));
		}
	}

	SGWorld3dInternal::RayCastInfo info;
	Ref<SGRayCastCollision3d> ret;

	if (internal->cast_ray(p_start->get_internal(), p_cast_to->get_internal(), p_collision_mask, &exceptions, p_collide_with_areas, p_collide_with_bodies, &info)) {
		SGInternalData *object_data = (SGInternalData *)info.body->get_data();
		SGCollisionObject3d *object = Object::cast_to<SGCollisionObject3d>(object_data->get_object());
		ret = Ref<SGRayCastCollision3d>(memnew(SGRayCastCollision3d(
				object,
				object_data->rid,
				SGFixedVector2::from_internal(info.collision_point),
				SGFixedVector2::from_internal(info.collision_normal))));
	}

	return ret;
}

void SGPhysics3dServer::free_rid(RID p_rid) {
	if (shape_owner.owns(p_rid)) {
		SGShape3dInternal *shape = shape_owner.get_or_null(p_rid);
		ERR_FAIL_COND(shape == nullptr);
		SGCollisionObject3dInternal *owner = shape->get_owner();
		if (owner) {
			owner->remove_shape(shape);
		}
		delete (SGInternalData *)shape->get_data();
		shape_owner.free(p_rid);
	} else if (object_owner.owns(p_rid)) {
		SGCollisionObject3dInternal *object = object_owner.get_or_null(p_rid);
		ERR_FAIL_COND(object == nullptr);
		SGWorld3dInternal *world = object->get_world();
		if (world) {
			SGCollisionObject3dInternal::ObjectType object_type = object->get_object_type();
			if (object_type == SGCollisionObject3dInternal::OBJECT_AREA) {
				world->remove_area((SGArea3dInternal *)object);
			}
			else if (object_type == SGCollisionObject3dInternal::OBJECT_BODY) {
				world->remove_body((SGBody3dInternal *)object);
			}
		}
		SGInternalData *data = (SGInternalData *)object->get_data();
		if (data != nullptr) {
			delete data;
		}
		object_owner.free(p_rid);
	} else if (world_owner.owns(p_rid)) {
		ERR_FAIL_COND_MSG(p_rid == default_world, "Cannot free the default world");
		SGWorld3dInternal *world = world_owner.get_or_null(p_rid);
		ERR_FAIL_COND(world == nullptr);
		delete world;
		world_owner.free(p_rid);
	}
}

SGShape3dInternal *SGPhysics3dServer::shape_get_internal(RID p_shape) {
	SGShape3dInternal *internal = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND_V(!internal, nullptr);
	return internal;
}

SGCollisionObject3dInternal *SGPhysics3dServer::collision_object_get_internal(RID p_object) {
	SGCollisionObject3dInternal *internal = object_owner.get_or_null(p_object);
	ERR_FAIL_COND_V(!internal, nullptr);
	return internal;
}

SGWorld3dInternal *SGPhysics3dServer::world_get_internal(RID p_world) {
	SGWorld3dInternal *internal = world_owner.get_or_null(p_world);
	ERR_FAIL_COND_V(!internal, nullptr);
	return internal;
}

SGWorld3dInternal *SGPhysics3dServer::get_default_world_internal() {
	return world_get_internal(default_world);
}

SGPhysics3dServer::SGPhysics3dServer() {
	singleton = this;
	default_world = world_create();
}

SGPhysics3dServer::~SGPhysics3dServer() {
	singleton = nullptr;

	SGWorld3dInternal *default_world_internal = world_owner.get_or_null(default_world);
	world_owner.free(default_world);
	memdelete(default_world_internal);
}

