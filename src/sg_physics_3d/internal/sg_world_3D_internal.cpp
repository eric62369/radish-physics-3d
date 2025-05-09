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

#include "sg_world_3D_internal.h"

#include "sg_bodies_3D_internal.h"
#include "sg_shapes_3D_internal.h"
#include "sg_broadphase_3D_internal.h"
#include "sg_collision_detector_3D_internal.h"
#include "sg_utils_internal.h"

void SGWorld3DInternal::add_area(SGArea3DInternal *p_area) {
	areas.push_back(p_area);
	p_area->add_to_broadphase(broadphase);
	p_area->set_world(this);
}

void SGWorld3DInternal::remove_area(SGArea3DInternal *p_area) {
	sg_remove_by_value(areas, p_area);
	p_area->remove_from_broadphase();
	p_area->set_world(nullptr);
}

void SGWorld3DInternal::add_body(SGBody3DInternal *p_body) {
	bodies.push_back(p_body);
	p_body->add_to_broadphase(broadphase);
	p_body->set_world(this);
}

void SGWorld3DInternal::remove_body(SGBody3DInternal *p_body) {
	sg_remove_by_value(bodies, p_body);
	p_body->remove_from_broadphase();
	p_body->set_world(this);
}

bool SGWorld3DInternal::overlaps(SGCollisionObject3DInternal *p_object1, SGCollisionObject3DInternal *p_object2, fixed p_margin, SGWorld3DInternal::BodyOverlapInfo *p_info) const {
	bool overlapping = false;

	SGWorld3DInternal::ShapeOverlapInfo shape_overlap_info;
	fixed longest_separation;

	for (SGShape3DInternal *shape1 : p_object1->get_shapes()) {
		for (SGShape3DInternal *shape2 : p_object2->get_shapes()) {
			if (overlaps(shape1, shape2, p_margin, &shape_overlap_info)) {
				overlapping = true;
				if (!p_info) {
					return overlapping;
				}

				fixed separation_length = shape_overlap_info.separation.length();
				if (separation_length > longest_separation) {
					longest_separation = separation_length;
					p_info->collider = p_object2;
					p_info->collider_shape = shape2;
					p_info->local_shape = shape1;
					p_info->separation = shape_overlap_info.separation;
					p_info->collision_normal = shape_overlap_info.collision_normal;
				}
			}
		}
	}

	return overlapping;
}

bool SGWorld3DInternal::overlaps(SGShape3DInternal *p_shape1, SGShape3DInternal *p_shape2, fixed p_margin, SGWorld3DInternal::ShapeOverlapInfo *p_info) const {
	using ShapeType = SGShape3DInternal::ShapeType;

	ShapeType shape1_type = p_shape1->get_shape_type();
	ShapeType shape2_type = p_shape2->get_shape_type();

	SGCollisionDetector3DInternal::OverlapInfo overlap_info;
	SGCollisionDetector3DInternal::OverlapInfo *overlap_info_ptr = p_info ? &overlap_info : nullptr;

	bool overlapping = false;
	bool swap = false;

	if (shape1_type == ShapeType::SHAPE_RECTANGLE && shape2_type == ShapeType::SHAPE_RECTANGLE) {
		overlapping = SGCollisionDetector3DInternal::Rectangle_overlaps_Rectangle(*((SGRectangle3DInternal *)p_shape1), *((SGRectangle3DInternal *)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_CIRCLE && shape2_type == ShapeType::SHAPE_CIRCLE) {
		overlapping = SGCollisionDetector3DInternal::Circle_overlaps_Circle(*((SGCircle3DInternal *)p_shape1), *((SGCircle3DInternal *)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_CIRCLE && shape2_type == ShapeType::SHAPE_RECTANGLE) {
		overlapping = SGCollisionDetector3DInternal::Circle_overlaps_Rectangle(*((SGCircle3DInternal *)p_shape1), *((SGRectangle3DInternal *)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_RECTANGLE && shape2_type == ShapeType::SHAPE_CIRCLE) {
		overlapping = SGCollisionDetector3DInternal::Circle_overlaps_Rectangle(*((SGCircle3DInternal *)p_shape2), *((SGRectangle3DInternal *)p_shape1), p_margin, overlap_info_ptr);
		swap = true;
	}
	else if (shape1_type == ShapeType::SHAPE_CAPSULE && shape2_type == ShapeType::SHAPE_CIRCLE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Circle(*((SGCapsule3DInternal*)p_shape1), *((SGCircle3DInternal*)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_CIRCLE && shape2_type == ShapeType::SHAPE_CAPSULE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Circle(*((SGCapsule3DInternal*)p_shape2), *((SGCircle3DInternal*)p_shape1), p_margin, overlap_info_ptr);
		swap = true;
	}
	else if (shape1_type == ShapeType::SHAPE_CAPSULE && shape2_type == ShapeType::SHAPE_RECTANGLE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Rectangle(*((SGCapsule3DInternal*)p_shape1), *((SGRectangle3DInternal*)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_RECTANGLE && shape2_type == ShapeType::SHAPE_CAPSULE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Rectangle(*((SGCapsule3DInternal*)p_shape2), *((SGRectangle3DInternal*)p_shape1), p_margin, overlap_info_ptr);
		swap = true;
	}
	else if (shape1_type == ShapeType::SHAPE_CAPSULE && shape2_type == ShapeType::SHAPE_CAPSULE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Capsule(*((SGCapsule3DInternal*)p_shape1), *((SGCapsule3DInternal*)p_shape2), p_margin, overlap_info_ptr);
	}
	else if (shape1_type == ShapeType::SHAPE_CAPSULE && shape2_type == ShapeType::SHAPE_CAPSULE) {
		overlapping = SGCollisionDetector3DInternal::Capsule_overlaps_Capsule(*((SGCapsule3DInternal*)p_shape2), *((SGCapsule3DInternal*)p_shape1), p_margin, overlap_info_ptr);
		swap = true;
	}

	if (overlapping && p_info) {
		// Make sure the info is from the perspective of the first shape.
		p_info->shape = p_shape2;
		p_info->collision_normal = swap ? -overlap_info.collision_normal : overlap_info.collision_normal;
		p_info->separation = swap ? -overlap_info.separation : overlap_info.separation;
	}

	return overlapping;
}

class SGOverlappingResultHandler : public SGResultHandlerInternal {
private:

	const SGWorld3DInternal *world;
	SGCollisionObject3DInternal *object;
	SGResultHandlerInternal *result_handler;

public:

	void handle_result(SGCollisionObject3DInternal *p_object, SGShape3DInternal *p_shape) {
		if (object == p_object) {
			return;
		}

		if (!object->test_collision_layers(p_object)) {
			return;
		}

		SGWorld3DInternal::BodyOverlapInfo overlap_info;

		if (world->overlaps(object, p_object, fixed::ZERO, &overlap_info)) {
			result_handler->handle_result(p_object, overlap_info.collider_shape);
		}
	}

	_FORCE_INLINE_ SGOverlappingResultHandler(const SGWorld3DInternal *p_world, SGCollisionObject3DInternal *p_object, SGResultHandlerInternal *p_result_handler)
		: world(p_world), object(p_object), result_handler(p_result_handler) { }

};

void SGWorld3DInternal::get_overlapping_areas(SGCollisionObject3DInternal *p_object, SGResultHandlerInternal *p_result_handler) const {
	SGOverlappingResultHandler overlapping_handler(this, p_object, p_result_handler);
	broadphase->find_nearby(p_object->get_bounds(), &overlapping_handler, SGCollisionObject3DInternal::OBJECT_AREA);
}

void SGWorld3DInternal::get_overlapping_bodies(SGCollisionObject3DInternal *p_object, SGResultHandlerInternal *p_result_handler) const {
	SGOverlappingResultHandler overlapping_handler(this, p_object, p_result_handler);
	broadphase->find_nearby(p_object->get_bounds(), &overlapping_handler, SGCollisionObject3DInternal::OBJECT_BODY);
}

class SGBestOverlappingResultHandler : public SGResultHandlerInternal {
private:

	const SGWorld3DInternal *world;
	SGCollisionObject3DInternal *object;
	fixed margin;
	SGWorld3DInternal::BodyOverlapInfo *overlap_info;
	SGWorld3DInternal::CompareCallback compare;

	bool overlapping;
	SGWorld3DInternal::BodyOverlapInfo test_overlap_info;
	fixed longest_separation;

public:

	void handle_result(SGCollisionObject3DInternal *p_object, SGShape3DInternal *p_shape) {
		if (object == p_object) {
			return;
		}

		SGBody3DInternal *other = (SGBody3DInternal *)p_object;
		if (!object->test_collision_layers(other)) {
			return;
		}

		if (world->overlaps(object, other, margin, &test_overlap_info)) {
			overlapping = true;

			fixed separation_length = test_overlap_info.separation.length();
			if (separation_length > longest_separation) {
				longest_separation = separation_length;
				*overlap_info = test_overlap_info;
			}
			// If we find another with the same separation, use the compare
			// callback to decide which is first.
			else if (separation_length == longest_separation && compare != nullptr && overlap_info->collider != nullptr) {
				if (compare(other, overlap_info->collider)) {
					*overlap_info = test_overlap_info;
				}
			}
		}
	}

	_FORCE_INLINE_ bool is_overlapping() const { return overlapping; }

	_FORCE_INLINE_ SGBestOverlappingResultHandler(const SGWorld3DInternal *p_world, SGCollisionObject3DInternal *p_object, fixed p_margin, SGWorld3DInternal::BodyOverlapInfo *p_overlap_info, SGWorld3DInternal::CompareCallback p_compare)
		: world(p_world), object(p_object), margin(p_margin), overlap_info(p_overlap_info), compare(p_compare), overlapping(false) { }

};

bool SGWorld3DInternal::get_best_overlapping_body(SGBody3DInternal *p_body, bool p_use_safe_margin, SGWorld3DInternal::BodyOverlapInfo *p_info) const {
	fixed safe_margin = p_use_safe_margin ? p_body->get_safe_margin() : fixed::ZERO;

	SGFixedRect3Internal bounds = p_body->get_bounds();
	bounds.grow_by(safe_margin);

	SGBestOverlappingResultHandler result_handler(this, p_body, safe_margin, p_info, compare_callback);
	broadphase->find_nearby(bounds, &result_handler, SGCollisionObject3DInternal::OBJECT_BODY);
	return result_handler.is_overlapping();
}

bool SGWorld3DInternal::unstuck_body(SGBody3DInternal *p_body, int p_max_attempts, BodyOverlapInfo *p_info) const {
	BodyOverlapInfo overlap_info;

	// First, get our body unstuck, if it's stuck.
	bool stuck = get_best_overlapping_body(p_body, false, &overlap_info);
	if (stuck) {
		stuck = get_best_overlapping_body(p_body, true, &overlap_info);
		for (int i = 0; i < p_max_attempts; i++) {
			SGFixedTransform3DInternal t = p_body->get_transform();
			t.set_origin(t.get_origin() + overlap_info.separation);
			p_body->set_transform(t);

			stuck = get_best_overlapping_body(p_body, true, &overlap_info);
			if (!stuck) {
				break;
			}
		}
	}

	if (stuck && p_info != nullptr) {
		*p_info = overlap_info;
	}

	return stuck;
}

bool SGWorld3DInternal::move_and_collide(SGBody3DInternal *p_body, const SGFixedVector3Internal &p_linear_velocity, SGWorld3DInternal::BodyCollisionInfo *p_collision) const {
	BodyOverlapInfo overlap_info;

	// First, get our body unstuck, if it's stuck.
	bool stuck = unstuck_body(p_body, 4, &overlap_info);
	if (stuck) {
		// We can't really continue. Bail with some sort of reasonable values.
		if (p_collision) {
			p_collision->collider = overlap_info.collider;
			p_collision->normal = SGFixedVector3Internal::ZERO;
			p_collision->remainder = p_linear_velocity;
		}
		return true;
	}

	// Move the body the full amount.
	SGFixedTransform3DInternal original_transform = p_body->get_transform();
	SGFixedTransform3DInternal test_transform = original_transform;
	SGFixedVector3Internal destination = original_transform.get_origin() + p_linear_velocity;
	test_transform.set_origin(destination);
	p_body->set_transform(test_transform);

	// Check if we're colliding. If not, we're done here!
	if (!get_best_overlapping_body(p_body, false, &overlap_info)) {
		return false;
	}

	// Use binary search to find the point at which we collide, and the point just before that.
	fixed low = fixed::ZERO;
	fixed hi = fixed::ONE;
	SGFixedVector3Internal last_safe_position = original_transform.get_origin();
	for (int i = 0; i < 8; i++) {
		fixed cur = (low + hi) * fixed::HALF;
		SGFixedVector3Internal test_position = original_transform.get_origin() + (p_linear_velocity * cur);
		test_transform.set_origin(test_position);
		p_body->set_transform(test_transform);
		if (get_best_overlapping_body(p_body, false, &overlap_info)) {
			hi = cur;
		}
		else {
			low = cur;
			last_safe_position = test_transform.get_origin();
		}
	}

	// Whatever was last set to our fixed position will be a safe position, so
	// let's make sure that's what ends up in the transform (in case the
	// last test_transform was a 'hi' position).
	if (test_transform.get_origin() != last_safe_position) {
		test_transform.set_origin(last_safe_position);
		p_body->set_transform(test_transform);
	}

	// At this point, the overlap_info will contain info about the collision at 'hi'
	// which is what we want to store in p_collision.
	if (p_collision) {
		p_collision->collider = overlap_info.collider;
		p_collision->normal = overlap_info.collision_normal;
		p_collision->remainder = p_linear_velocity - (p_linear_velocity * low);
	}

	return true;
}

bool SGWorld3DInternal::segment_intersects_shape(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, SGShape3DInternal *p_shape, SGFixedVector3Internal &p_intersection_point, SGFixedVector3Internal &p_collision_normal) const {
	using ShapeType = SGShape3DInternal::ShapeType;

	ShapeType shape_type = p_shape->get_shape_type();

	switch (shape_type) {
		case ShapeType::SHAPE_RECTANGLE:
		case ShapeType::SHAPE_CIRCLE:
			return SGCollisionDetector3DInternal::segment_intersects_Circle(p_start, p_cast_to, *(SGCircle3DInternal *)p_shape, p_intersection_point, p_collision_normal);

		case ShapeType::SHAPE_CAPSULE:
			return SGCollisionDetector3DInternal::segment_intersects_Capsule(p_start, p_cast_to, *(SGCapsule3DInternal*)p_shape, p_intersection_point, p_collision_normal);
	}

	return false;
}

class SGRayCastResultHandler : public SGResultHandlerInternal {
private:

	const SGWorld3DInternal *world;
	const SGFixedVector3Internal &start;
	const SGFixedVector3Internal &cast_to;
	uint32_t collision_mask;
	std::unordered_set<SGCollisionObject3DInternal *> *exceptions;

	bool intersects;
	SGCollisionObject3DInternal *collider;
	fixed shortest_distance;
	SGFixedVector3Internal closest_intersection_point;
	SGFixedVector3Internal closest_collision_normal;
	SGFixedVector3Internal intersection_point;
	SGFixedVector3Internal collision_normal;

public:

	void handle_result(SGCollisionObject3DInternal *p_object, SGShape3DInternal *p_shape) {
		if (exceptions && exceptions->find(p_object) != exceptions->end()) {
			return;
		}

		if (!(p_object->get_collision_layer() & collision_mask)) {
			return;
		}

		for (SGShape3DInternal *shape : p_object->get_shapes()) {
			if (world->segment_intersects_shape(start, cast_to, shape, intersection_point, collision_normal)) {
				intersects = true;

				fixed distance = (intersection_point - start).length();
				if (collider == nullptr || distance < shortest_distance) {
					shortest_distance = distance;
					collider = p_object;
					closest_intersection_point = intersection_point;
					closest_collision_normal = collision_normal;
				}
			}
		}
	}

	_FORCE_INLINE_ void populate_info(SGWorld3DInternal::RayCastInfo *p_info) {
		if (collider) {
			p_info->body = (SGBody3DInternal *)collider;
			p_info->collision_point = closest_intersection_point;
			p_info->collision_normal = closest_collision_normal;
		}
	}

	_FORCE_INLINE_ bool is_intersecting() const {
		return intersects;
	}

	_FORCE_INLINE_ SGRayCastResultHandler(const SGWorld3DInternal *p_world, const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, uint32_t p_collision_mask, std::unordered_set<SGCollisionObject3DInternal *> *p_exceptions)
		: world(p_world), start(p_start), cast_to(p_cast_to), collision_mask(p_collision_mask), exceptions(p_exceptions), intersects(false), collider(nullptr) {
		SGFixedRect3Internal bounds(p_start, SGFixedVector3Internal());
		bounds.expand_to(p_start + p_cast_to);
	}

};

bool SGWorld3DInternal::cast_ray(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, uint32_t p_collision_mask, std::unordered_set<SGCollisionObject3DInternal *> *p_exceptions,
		bool collide_with_areas, bool collide_with_bodies, SGWorld3DInternal::RayCastInfo *p_info) const {
	SGRayCastResultHandler result_handler(this, p_start, p_cast_to, p_collision_mask, p_exceptions);

	SGFixedRect3Internal bounds(p_start, SGFixedVector3Internal());
	bounds.expand_to(p_start + p_cast_to);

	int collide_with = 0;
	if (collide_with_areas) {
		collide_with |= SGCollisionObject3DInternal::OBJECT_AREA;
	}
	if (collide_with_bodies) {
		collide_with |= SGCollisionObject3DInternal::OBJECT_BODY;
	}
	broadphase->find_nearby(bounds, &result_handler, collide_with);
	if (p_info) {
		result_handler.populate_info(p_info);
	}
	return result_handler.is_intersecting();
}

SGWorld3DInternal::SGWorld3DInternal(unsigned int p_broadphase_cell_size, CompareCallback p_compare_callback) {
	broadphase = new SGBroadphase3DInternal(p_broadphase_cell_size);
	compare_callback = p_compare_callback;
}

SGWorld3DInternal::~SGWorld3DInternal() {
	delete broadphase;
}
