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
#include <godot_cpp/variant/utility_functions.hpp> // TODO: remove this later
#include "sg_collision_detector_3D_internal.h"
#include <algorithm>


using Interval = SGCollisionDetector3DInternal::Interval;

bool SGCollisionDetector3DInternal::AABB_overlaps_AABB(const SGFixedRect3Internal &aabb1, const SGFixedRect3Internal &aabb2) {
	SGFixedVector3Internal min_one = aabb1.get_min();
	SGFixedVector3Internal max_one = aabb1.get_max();
	SGFixedVector3Internal min_two = aabb2.get_min();
	SGFixedVector3Internal max_two = aabb2.get_max();

	return (min_two.x <= max_one.x) && (min_one.x <= max_two.x) && \
		   (min_two.y <= max_one.y) && (min_one.y <= max_two.y);
}

Interval SGCollisionDetector3DInternal::get_interval(const SGShape3DInternal &shape, const SGFixedVector3Internal &axis, fixed p_margin) {
	Interval result;

	if (shape.get_shape_type() == SGShape3DInternal::ShapeType::SHAPE_CIRCLE) {
		const SGCircle3DInternal &circle = (const SGCircle3DInternal&)shape;
		SGFixedTransform3DInternal t = shape.get_global_transform();
		fixed center = axis.dot(t.get_origin());
		fixed radius = circle.get_radius() * t.get_scale().x;
		result.min = center - radius;
		result.max = center + radius;
	}
	else if (shape.get_shape_type() == SGShape3DInternal::ShapeType::SHAPE_CAPSULE) {
		const SGCapsule3DInternal& capsule = (const SGCapsule3DInternal&)shape;
		SGFixedTransform3DInternal t = shape.get_global_transform();
		const fixed scaled_radius = capsule.get_radius() * t.get_scale().x;
		const std::vector<SGFixedVector3Internal> verts = capsule.get_global_vertices();
		result.min = axis.dot(verts[0]);
		result.max = axis.dot(verts[1]);
		if (result.min > result.max) {
			std::swap(result.min, result.max);
		}
		result.min -= scaled_radius;
		result.max += scaled_radius;
	}
	else {
		std::vector<SGFixedVector3Internal> verts = shape.get_global_vertices();
		result.min = result.max = axis.dot(verts[0]);
		for (std::size_t i = 1; i < verts.size(); i++) {
			fixed projection = axis.dot(verts[i]);
			if (projection < result.min) {
				result.min = projection;
			}
			if (projection > result.max) {
				result.max = projection;
			}
		}
	}

	result.min -= p_margin;
	result.max += p_margin;

	return result;
}

bool SGCollisionDetector3DInternal::overlaps_on_axis(const SGShape3DInternal &shape1, const SGShape3DInternal &shape2, const SGFixedVector3Internal &axis, fixed p_margin, fixed &separation) {
	Interval i1 = get_interval(shape1, axis, p_margin);
	Interval i2 = get_interval(shape2, axis, p_margin);

	fixed d1 = i1.max - i2.min;
	fixed d2 = i2.max - i1.min;

	if (d1 > fixed::ZERO && d2 > fixed::ZERO) {
		separation = (d1 < d2) ? d1 : d2;

		// Attempt to make the seperation relative to shape1.
		if (i1.min < i2.min) {
			separation = -separation;
		}
		return true;
	}

	return false;
}

bool SGCollisionDetector3DInternal::sat_test(const SGShape3DInternal &shape1, const SGShape3DInternal &shape2, const std::vector<SGFixedVector3Internal> &axes,
fixed p_margin, SGFixedVector3Internal &best_separation_vector, fixed &best_separation_length, SGFixedVector3Internal &collision_normal) {
	fixed separation_component;

	for (std::size_t i = 0; i < axes.size(); i++) {
		const SGFixedVector3Internal &axis = axes[i];
		if (axis == SGFixedVector3Internal::ZERO) {
			continue;
		}
		if (overlaps_on_axis(shape1, shape2, axis, p_margin, separation_component)) {
			SGFixedVector3Internal separation_vector = (axis * separation_component);

			// if the correction is too small it will be set to 0 and the overlap won't be corrected
			// in that case we don't consider it an overlap
			if ((axis.x == fixed::ZERO || separation_vector.x != fixed::ZERO) && (axis.y == fixed::ZERO || separation_vector.y != fixed::ZERO) && (axis.z == fixed::ZERO || separation_vector.z != fixed::ZERO))
			{
				fixed separation_length = separation_component.abs();
				if (best_separation_vector == SGFixedVector3Internal::ZERO || separation_length < best_separation_length) {
					best_separation_vector = separation_vector;
					best_separation_length = separation_length;
					collision_normal = axes[i] * FIXED_SGN(separation_component);
				}
			}
			else {
				// Consider this as a separation axis
				return false;
			}
		}
		else {
			// Axis of separation found! They don't overlap.
			return false;
		}
	}

	// No axis of separation found, they overlap!
	return true;
}

bool SGCollisionDetector3DInternal::Rectangle_overlaps_Rectangle(const SGRectangle3DInternal &rectangle1, const SGRectangle3DInternal &rectangle2, fixed p_margin, OverlapInfo *p_info) {
	SGFixedVector3Internal best_separation_vector;
	fixed best_separation_length;
	SGFixedVector3Internal collision_normal;

	if (!sat_test(rectangle1, rectangle2, rectangle1.get_global_axes(), p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (!sat_test(rectangle1, rectangle2, rectangle2.get_global_axes(), p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (p_info) {
		p_info->separation = best_separation_vector;
		p_info->collision_normal = collision_normal;
	}

	return true;
}

bool SGCollisionDetector3DInternal::Circle_overlaps_Circle(const SGCircle3DInternal &circle1, const SGCircle3DInternal &circle2, fixed p_margin, OverlapInfo *p_info) {
	SGFixedTransform3DInternal t1 = circle1.get_global_transform();
	SGFixedTransform3DInternal t2 = circle2.get_global_transform();

	SGFixedVector3Internal line = t1.get_origin() - t2.get_origin();
	fixed line_length = line.length();
	// We only multiply by the scale.x because we don't support non-uniform scaling.
	fixed combined_radius = (circle1.get_radius() * t1.get_scale().x + circle2.get_radius() * t2.get_scale().x) + p_margin;

	// A separation of fixed(1) is not considered an overlap
	// as in further computations normalized_vector * fixed(1) = (0, 0)
	fixed separation = combined_radius - line_length;
	bool overlapping;
	SGFixedVector3Internal collision_normal;
	SGFixedVector3Internal separation_vector;
	if (line_length == fixed::ZERO) {
		// If circles share an origin, then we arbitrarily decide that we
		// separate them by moving up.
		overlapping = true;
		collision_normal = SGFixedVector3Internal(fixed::ZERO, fixed::NEG_ONE, fixed::ZERO); // TODO: what is z axis?
		separation_vector = collision_normal * separation;
	}
	else {
		collision_normal = line.normalized();
		separation_vector = collision_normal * separation;
		// if the correction is too small it will be set to 0 and the overlap won't be corrected
		// in that case we don't consider it an overlap
		overlapping = (separation > fixed::ZERO) && (collision_normal.x == fixed::ZERO || separation_vector.x != fixed::ZERO)
			&& (collision_normal.y == fixed::ZERO || separation_vector.y != fixed::ZERO);
	}

	if (overlapping && p_info) {
		p_info->collision_normal = collision_normal;
		p_info->separation = separation_vector;
	}

	return overlapping;
}

bool SGCollisionDetector3DInternal::Circle_overlaps_Rectangle(const SGCircle3DInternal &circle, const SGRectangle3DInternal &rectangle, fixed p_margin, OverlapInfo *p_info) {
	SGFixedVector3Internal best_separation_vector;
	fixed best_separation_length;
	SGFixedVector3Internal collision_normal;

	// First, we see if the circle has any seperation from the rectangle's axes.
	if (!sat_test(rectangle, circle, rectangle.get_global_axes(), p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	// Next, we need to find the axis to check for the circle (it's a vector
	// from the closest vertex to the circle center).

	std::vector<SGFixedVector3Internal> vertices = rectangle.get_global_vertices();
	SGFixedTransform3DInternal ct = circle.get_global_transform();
	SGFixedVector3Internal closest_vertex = vertices[0];
	fixed closest_distance = (ct.get_origin() - vertices[0]).length();

	for (std::size_t i = 1; i < vertices.size(); i++) {
		fixed distance = (ct.get_origin() - vertices[i]).length();
		if (distance < closest_distance) {
			closest_distance = distance;
			closest_vertex = vertices[i];
		}
	}

	std::vector<SGFixedVector3Internal> circle_axes;
	circle_axes.push_back((ct.get_origin() - closest_vertex).normalized());
	if (!sat_test(rectangle, circle, circle_axes, p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (p_info) {
		p_info->collision_normal = -collision_normal;
		p_info->separation = -best_separation_vector;
	}

	return true;
}

bool SGCollisionDetector3DInternal::Capsule_overlaps_Circle(const SGCapsule3DInternal& capsule, const SGCircle3DInternal& circle, fixed p_margin, OverlapInfo* p_info) {
	SGFixedVector3Internal best_separation_vector;
	fixed best_separation_length;
	SGFixedVector3Internal collision_normal;
	std::vector<SGFixedVector3Internal> axes;
	const SGFixedTransform3DInternal capsule_transform = capsule.get_global_transform();
	// Capsule width axis
	axes.push_back(capsule_transform.elements[0].normalized());

	// Center of the circle to capsule internal endpoints
	const std::vector<SGFixedVector3Internal> verts = capsule.get_global_vertices();
	const SGFixedVector3Internal circle_center = circle.get_global_transform().get_origin();
	axes.push_back((circle_center - verts[0]).normalized());
	axes.push_back((circle_center - verts[1]).normalized());
	if (!sat_test(capsule, circle, axes, p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (p_info) {
		p_info->collision_normal = collision_normal;
		p_info->separation = best_separation_vector;
	}

	return true;
}


bool SGCollisionDetector3DInternal::Capsule_overlaps_Rectangle(const SGCapsule3DInternal& capsule, const SGRectangle3DInternal& rectangle, fixed p_margin, OverlapInfo* p_info) {
	SGFixedVector3Internal best_separation_vector;
	fixed best_separation_length;
	SGFixedVector3Internal collision_normal;
	const SGFixedTransform3DInternal capsule_transform = capsule.get_global_transform();
	// Rectangle edges
	if (!sat_test(capsule, rectangle, rectangle.get_global_axes(), p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	std::vector<SGFixedVector3Internal> axes;
	// Capsule width axis
	axes.push_back(capsule_transform.elements[0].normalized());

	// Rectangle vertex to capsule internal endpoints
	const std::vector<SGFixedVector3Internal> capsule_endpoints = capsule.get_global_vertices();
	axes.push_back((rectangle.get_closest_vertex(capsule_endpoints[0]) - capsule_endpoints[0]).normalized());
	axes.push_back((rectangle.get_closest_vertex(capsule_endpoints[1]) - capsule_endpoints[1]).normalized());

	if (!sat_test(capsule, rectangle, axes, p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (p_info) {
		p_info->collision_normal = collision_normal;
		p_info->separation = best_separation_vector;
	}

	return true;
}

SGFixedVector3Internal ClosestPointOnLineSegment(SGFixedVector3Internal A, SGFixedVector3Internal B, SGFixedVector3Internal Point)
{
	SGFixedVector3Internal AB = B - A;
	fixed t = ((Point - A).dot(AB)) / (AB.dot(AB));
	return A + (AB * MIN(MAX(t, fixed(0)), fixed(1))); 
	// return A + saturate(t) * AB; // saturate(t) can be written as: min((max(t, 0), 1)
}

bool SGCollisionDetector3DInternal::Capsule_overlaps_Capsule(const SGCapsule3DInternal& capsule1, const SGCapsule3DInternal& capsule2, fixed p_margin, OverlapInfo* p_info) {
	// SGFixedVector3Internal best_separation_vector;
	// fixed best_separation_length;
	// SGFixedVector3Internal collision_normal;
	// // const SGFixedTransform3DInternal capsule_transform1 = capsule1.get_global_transform();
	// // const SGFixedTransform3DInternal capsule_transform2 = capsule2.get_global_transform();

	// // https://wickedengine.net/2020/04/capsule-collision-detection/
	// // capsule A:
	// const std::vector<SGFixedVector3Internal> capsule1_endpoints = capsule1.get_global_vertices();
	// const std::vector<SGFixedVector3Internal> capsule2_endpoints = capsule2.get_global_vertices();

	// SGFixedVector3Internal a_Normal = (capsule1_endpoints[1] - capsule1_endpoints[0]).normalized(); // TODO: which endpoint is the tip and base?
	// SGFixedVector3Internal a_LineEndOffset = a_Normal * capsule1.get_radius(); 
	// SGFixedVector3Internal a_A = capsule1_endpoints[0] + a_LineEndOffset; // which one's base?
	// SGFixedVector3Internal a_B = capsule1_endpoints[1] - a_LineEndOffset; // which one's tip?

	// // capsule B:
	// SGFixedVector3Internal b_Normal = (capsule2_endpoints[1] - capsule2_endpoints[0]).normalized();
	// SGFixedVector3Internal b_LineEndOffset = b_Normal * capsule2.get_radius(); 
	// SGFixedVector3Internal b_A = capsule2_endpoints[0] + b_LineEndOffset; 
	// SGFixedVector3Internal b_B = capsule2_endpoints[1] - b_LineEndOffset;

	// // vectors between line endpoints:
	// SGFixedVector3Internal v0 = b_A - a_A; 
	// SGFixedVector3Internal v1 = b_B - a_A; 
	// SGFixedVector3Internal v2 = b_A - a_B; 
	// SGFixedVector3Internal v3 = b_B - a_B;

	// // squared distances:
	// fixed d0 = v0.dot(v0); 
	// fixed d1 = v1.dot(v1); 
	// fixed d2 = v2.dot(v2); 
	// fixed d3 = v3.dot(v3);

	// // select best potential endpoint on capsule A:
	// SGFixedVector3Internal bestA;
	// if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
	// 	bestA = a_B;
	// }
	// else {
	// 	bestA = a_A;
	// }

	// // select point on capsule B line segment nearest to best potential endpoint on A capsule:
	// SGFixedVector3Internal bestB = ClosestPointOnLineSegment(b_A, b_B, bestA);

	// // now do the same for capsule A segment:
	// bestA = ClosestPointOnLineSegment(a_A, a_B, bestB);

	// // We selected the two best possible candidates on both capsule axes. What remains is to place spheres on those points and perform the sphere intersection routine:
	// SGFixedVector3Internal penetration_normal = bestA - bestB;
	// fixed len = penetration_normal.length();
	// penetration_normal /= len;  // normalize
	// fixed penetration_depth = capsule1.get_radius() + capsule2.get_radius() - len;
	// bool intersects = penetration_depth > fixed(0);

	// // godot::UtilityFunctions::print("Total capsule radius: ", (capsule1.get_radius() + capsule2.get_radius()).to_int());
	// godot::UtilityFunctions::print("Pen. Distance: ", len.to_int());
	// godot::UtilityFunctions::print("Best A Endpoint x: ", bestA.x.to_int());
	// godot::UtilityFunctions::print("Best A Endpoint y: ", bestA.y.to_int());
	// godot::UtilityFunctions::print("Best A Endpoint z: ", bestA.z.to_int());
	// godot::UtilityFunctions::print("Best B Endpoint x: ", bestB.x.to_int());
	// godot::UtilityFunctions::print("Best B Endpoint y: ", bestB.y.to_int());
	// godot::UtilityFunctions::print("Best B Endpoint z: ", bestB.z.to_int());
	// return intersects;

	SGFixedVector3Internal best_separation_vector;
	fixed best_separation_length;
	SGFixedVector3Internal collision_normal;
	std::vector<SGFixedVector3Internal> axes;
	// Capsule width axis
	axes.push_back(capsule1.get_global_transform().elements[0].normalized());
	axes.push_back(capsule2.get_global_transform().elements[0].normalized());
	if (!sat_test(capsule1, capsule2, axes, p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	// Internal endpoints to internal endpoints
	const std::vector<SGFixedVector3Internal> capsule1_endpoints = capsule1.get_global_vertices();
	const std::vector<SGFixedVector3Internal> capsule2_endpoints = capsule2.get_global_vertices();
	axes.clear();
	axes.push_back((capsule1_endpoints[0] - capsule2_endpoints[0]).normalized());
	axes.push_back((capsule1_endpoints[0] - capsule2_endpoints[1]).normalized());
	axes.push_back((capsule1_endpoints[1] - capsule2_endpoints[0]).normalized());
	axes.push_back((capsule1_endpoints[1] - capsule2_endpoints[1]).normalized());

	if (!sat_test(capsule1, capsule2, axes, p_margin, best_separation_vector, best_separation_length, collision_normal)) {
		return false;
	}

	if (p_info) {
		p_info->collision_normal = collision_normal;
		p_info->separation = best_separation_vector;
	}

	return true;
}

bool SGCollisionDetector3DInternal::segment_intersects_Capsule(const SGFixedVector3Internal& p_start, const SGFixedVector3Internal& p_cast_to, const SGCapsule3DInternal& capsule, SGFixedVector3Internal& p_intersection_point, SGFixedVector3Internal& p_collision_normal) {
	SGCircle3DInternal circle(capsule.get_radius());
	auto t = capsule.get_global_transform();
	t.set_origin(capsule.get_global_vertices()[0]);
	circle.set_transform(t);
	SGFixedVector3Internal intersection_point;
	SGFixedVector3Internal collision_normal;
	fixed closest_distance_squared;
	bool colliding = false;
	if (segment_intersects_Circle(p_start, p_cast_to, circle, intersection_point, collision_normal)) {
		closest_distance_squared = intersection_point.distance_squared_to(p_start);
		p_intersection_point = intersection_point;
		p_collision_normal = collision_normal;
		colliding = true;
	}
	t.set_origin(capsule.get_global_vertices()[1]);
	circle.set_transform(t);
	if (segment_intersects_Circle(p_start, p_cast_to, circle, intersection_point, collision_normal)) {
		fixed distance_squared = intersection_point.distance_squared_to(p_start);
		if (!colliding || closest_distance_squared > distance_squared) {
			closest_distance_squared = distance_squared;
			p_intersection_point = intersection_point;
			p_collision_normal = collision_normal;
		}
		colliding = true;
	}
	SGRectangle3DInternal rectangle(capsule.get_radius(), fixed(capsule.get_height().value / 2), capsule.get_radius()); // TODO: z collision how?
	rectangle.set_transform(capsule.get_global_transform());
	if (segment_intersects_Polygon(p_start, p_cast_to, rectangle, intersection_point, collision_normal)) {
		fixed distance_squared = intersection_point.distance_squared_to(p_start);
		if (!colliding || closest_distance_squared > distance_squared) {
			p_intersection_point = intersection_point;
			p_collision_normal = collision_normal;
		}
		colliding = true;
	}

	return colliding;
}


// Algorithm from https://stackoverflow.com/a/565282
//
// License: CC BY-SA 3.0
// Author: Gareth Rees
//
// p = p_start_1
// r = p_cast_to_1
// q = p_start_2
// s = p_cast_to_2
bool SGCollisionDetector3DInternal::segment_intersects_segment(const SGFixedVector3Internal &p_start_1, const SGFixedVector3Internal &p_cast_to_1, const SGFixedVector3Internal &p_start_2, const SGFixedVector3Internal &p_cast_to_2, SGFixedVector3Internal &p_intersection_point) {
	fixed denominator = p_cast_to_1.cross(p_cast_to_2);
	fixed u_nominator = (p_start_2 - p_start_1).cross(p_cast_to_1);

	if (denominator == fixed::ZERO && u_nominator == fixed::ZERO) {
		// Line segments are collinear.
		//
		// They could overlap, but since we are always dealing with polygons,
		// we know that there will always be another edge that shares one of
		// the 2nd line segments end points, so we can trust that we'll
		// collide with that edge (so long as there aren't two collinear edges).
		return false;
	}

	if (denominator == fixed::ZERO) {
		// Line segments are parallel and so non-intersecting.
		return false;
	}

	fixed u = u_nominator / denominator;
	if (u < fixed::ZERO || u > fixed::ONE) {
		// Intersection would happen before the start or after the end of the 2nd segment.
		return false;
	}

	fixed t = (p_start_2 - p_start_1).cross(p_cast_to_2) / denominator;
	if (t < fixed::ZERO || t > fixed::ONE) {
		// Intersection would happen before the start or after the end of the 1st segment.
		return false;
	}

	p_intersection_point = p_start_1 + (p_cast_to_1 * t);

	return true;
}

bool SGCollisionDetector3DInternal::segment_intersects_Polygon(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, const SGShape3DInternal &polygon, SGFixedVector3Internal &p_intersection_point, SGFixedVector3Internal &p_collision_normal) {
	std::vector<SGFixedVector3Internal> verts = polygon.get_global_vertices();

	bool intersecting = false;

	SGFixedVector3Internal closest_intersection_point;
	SGFixedVector3Internal closest_collision_normal;
	fixed closest_distance;

	SGFixedVector3Internal previous = verts[verts.size() - 1];
	for (std::size_t i = 0; i < verts.size(); i++) {
		SGFixedVector3Internal cur = verts[i];
		SGFixedVector3Internal edge = cur - previous;
		SGFixedVector3Internal intersection_point;
		if (segment_intersects_segment(p_start, p_cast_to, previous, edge, intersection_point)) {
			fixed distance = (intersection_point - p_start).length();
			if (!intersecting || distance < closest_distance) {
				closest_distance = distance;
				closest_intersection_point = intersection_point;
				closest_collision_normal = SGFixedVector3Internal(edge.y, -edge.x, edge.z).normalized(); // TODO: z collision normal how?
			}
			intersecting = true;
		}
		previous = cur;
	}

	if (intersecting) {
		p_collision_normal = closest_collision_normal;
		p_intersection_point = closest_intersection_point;
	}

	return intersecting;
}

// Algorithm from https://stackoverflow.com/a/1084899
//
// License: CC BY-SA 4.0
// Original author: bobobobo
//
// The code has been adapted to this engine and fixed-point path.
//
// E = p_start
// d = p_cast_to
bool SGCollisionDetector3DInternal::segment_intersects_Circle(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, const SGCircle3DInternal &circle, SGFixedVector3Internal &p_intersection_point, SGFixedVector3Internal &p_collision_normal) {
	SGFixedTransform3DInternal ct = circle.get_global_transform();

	SGFixedVector3Internal C = ct.get_origin();
	SGFixedVector3Internal f = p_start - C;

	fixed r = circle.get_radius() * ct.get_scale().x;

	fixed a = p_cast_to.dot(p_cast_to);
	if (a == fixed::ZERO) {
		// This would lead to division by zero.
		return false;
	}

	fixed b = fixed::TWO * f.dot(p_cast_to);
	fixed c = f.dot(f) - (r * r);

	// Reduce precision - we're just going to use whole integers to calculate
	// the determinant, in an attempt to avoid overflowing 64-bits.
	int64_t small_a = a.value >> 16;
	int64_t small_b = b.value >> 16;
	int64_t small_c = c.value >> 16;

	int64_t small_discriminant = (small_b * small_b) - (4 * small_a * small_c);
	if (small_discriminant < 0) {
		// No intersection.
		return false;
	}
	else {
		// Get the square root and return to our normal precision.
		fixed discriminant(sg_sqrt_64(small_discriminant) << 16);

		fixed t1 = (-b - discriminant) / (fixed::TWO * a);
		fixed t2 = (-b + discriminant) / (fixed::TWO * a);

		// This is where we're outside the circle, and intersect at least once.
		if (t1 >= fixed::ZERO && t1 <= fixed::ONE) {
			p_intersection_point = p_start + p_cast_to * t1;
			p_collision_normal = (p_intersection_point - C).normalized();
			return true;
		}

		// This is where we're inside the circle, and intersect the outer edge.
		if (t2 >= fixed::ZERO && t2 <= fixed::ONE) {
			p_intersection_point = p_start + p_cast_to * t2;
			p_collision_normal = (p_intersection_point - C).normalized();
			return true;
		}
	}

	return false;
}
