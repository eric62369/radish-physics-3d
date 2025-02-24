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

#ifndef SG_WORLD_3D_INTERNAL_H
#define SG_WORLD_3D_INTERNAL_H

#include <vector>
#include <unordered_set>

#include "sg_fixed_vector3_internal.h"
#include "sg_fixed_rect2_internal.h"
#include "sg_result_handler_internal.h"

class SGArea3DInternal;
class SGBody3DInternal;
class SGCollisionObject3DInternal;
class SGShape3DInternal;
class SGBroadphase3DInternal;

class SGWorld3DInternal {
public:
	typedef bool (* CompareCallback)(const SGCollisionObject3DInternal*, const SGCollisionObject3DInternal*);

protected:

	std::vector<SGArea3DInternal *> areas;
	std::vector<SGBody3DInternal *> bodies;
	SGBroadphase3DInternal *broadphase;
	CompareCallback compare_callback;

public:
	struct ShapeOverlapInfo {
		SGShape3DInternal *shape;
		SGFixedVector3Internal separation;
		SGFixedVector3Internal collision_normal;

		ShapeOverlapInfo() {
			shape = nullptr;
		}
	};

	struct BodyOverlapInfo {
		SGCollisionObject3DInternal *collider;
		SGShape3DInternal *collider_shape;
		SGShape3DInternal *local_shape;
		SGFixedVector3Internal separation;
		SGFixedVector3Internal collision_normal;

		BodyOverlapInfo() {
			collider = nullptr;
			collider_shape = nullptr;
			local_shape = nullptr;
		}
	};

	struct BodyCollisionInfo {
		SGCollisionObject3DInternal *collider;
		// @todo How can we get the shape in here?
		SGFixedVector3Internal normal;
		SGFixedVector3Internal remainder;

		BodyCollisionInfo() {
			collider = nullptr;
		}
	};

	struct RayCastInfo {
		SGBody3DInternal *body;
		SGFixedVector3Internal collision_point;
		SGFixedVector3Internal collision_normal;

		RayCastInfo() {
			body = nullptr;
		}
	};

	_FORCE_INLINE_ const std::vector<SGBody3DInternal *> &get_bodies() const { return bodies; }
	_FORCE_INLINE_ const std::vector<SGArea3DInternal *> &get_areas() const { return areas; }
	_FORCE_INLINE_ const SGBroadphase3DInternal *get_broadphase() const { return broadphase; }

	void add_area(SGArea3DInternal *p_area);
	void remove_area(SGArea3DInternal *p_area);
	void add_body(SGBody3DInternal *p_body);
	void remove_body(SGBody3DInternal *p_body);

	bool overlaps(SGCollisionObject3DInternal *p_object1, SGCollisionObject3DInternal *p_object2, fixed p_margin, BodyOverlapInfo *p_info = nullptr) const;
	bool overlaps(SGShape3DInternal *p_shape1, SGShape3DInternal *p_shape2, fixed p_margin, ShapeOverlapInfo *p_info = nullptr) const;

	void get_overlapping_areas(SGCollisionObject3DInternal *p_object, SGResultHandlerInternal *p_result_handler) const;
	void get_overlapping_bodies(SGCollisionObject3DInternal *p_object, SGResultHandlerInternal *p_result_handler) const;

	bool get_best_overlapping_body(SGBody3DInternal *p_body, bool p_use_safe_margin, BodyOverlapInfo *p_info) const;
	bool unstuck_body(SGBody3DInternal *p_body, int p_max_attempts, BodyOverlapInfo *p_info = nullptr) const;
	bool move_and_collide(SGBody3DInternal *p_body, const SGFixedVector3Internal &p_linear_velocity, BodyCollisionInfo *p_collision = nullptr) const;

	bool segment_intersects_shape(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, SGShape3DInternal *p_shape, SGFixedVector3Internal &p_intersection_point, SGFixedVector3Internal &p_collision_normal) const;
	bool cast_ray(const SGFixedVector3Internal &p_start, const SGFixedVector3Internal &p_cast_to, uint32_t p_collision_mask, std::unordered_set<SGCollisionObject3DInternal *> *p_exceptions = nullptr,
		bool collide_with_areas=false, bool collide_with_bodies=true, RayCastInfo *p_info = nullptr) const;

	SGWorld3DInternal(unsigned int p_broadphase_cell_size, CompareCallback p_compare_callback = nullptr);
	~SGWorld3DInternal();
};

#endif
