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

#include "sg_bodies_3D_internal.h"

#include <algorithm>

#include "sg_world_3D_internal.h"
#include "sg_broadphase_3D_internal.h"
#include "sg_utils_internal.h"

void SGCollisionObject3DInternal::set_transform(const SGFixedTransform3DInternal &p_transform) {
	transform = p_transform;
	for(SGShape3DInternal *shape : shapes) {
		shape->mark_global_xform_dirty();
	}

	if (broadphase && monitorable) {
		if (broadphase_element) {
			broadphase->update_element(broadphase_element);
		}
		else {
			broadphase_element = broadphase->create_element(this);
		}
	}
}

void SGCollisionObject3DInternal::add_shape(SGShape3DInternal *p_shape) {
	p_shape->set_owner(this);
	shapes.push_back(p_shape);

	if (broadphase && monitorable && broadphase_element) {
		broadphase->update_element(broadphase_element);
	}
}

void SGCollisionObject3DInternal::remove_shape(SGShape3DInternal *p_shape) {
	p_shape->set_owner(nullptr);
	sg_remove_by_value(shapes, p_shape);

	if (broadphase && monitorable && broadphase_element) {
		broadphase->update_element(broadphase_element);
	}
}

SGFixedRect3Internal SGCollisionObject3DInternal::get_bounds() const {
	if (shapes.size() == 0) {
		return SGFixedRect3Internal(transform.get_origin(), SGFixedVector3Internal());
	}

	SGFixedRect3Internal bounds = shapes[0]->get_bounds();
	for (std::vector<SGShape3DInternal *>::size_type i = 1; i < shapes.size(); i++) {
		bounds = bounds.merge(shapes[i]->get_bounds());
	}

	return bounds;
}

void SGCollisionObject3DInternal::add_to_broadphase(SGBroadphase3DInternal *p_broadphase) {
	remove_from_broadphase();
	broadphase = p_broadphase;
	if (transform == SGFixedTransform3DInternal()) {
		// Defer creation of the broadphase element until we update the transform.
		broadphase_element = nullptr;
	}
	else {
		broadphase_element = broadphase->create_element(this);
	}
}

void SGCollisionObject3DInternal::remove_from_broadphase() {
	if (broadphase) {
		if (monitorable && broadphase_element) {
			broadphase->delete_element(broadphase_element);
		}
		broadphase = nullptr;
		broadphase_element = nullptr;
	}
}

void SGCollisionObject3DInternal::set_monitorable(bool p_monitorable) {
	if (monitorable == p_monitorable) {
		return;
	}

	if (broadphase) {
		if (!monitorable) {
			broadphase_element = broadphase->create_element(this);
		}
		else if (broadphase_element) {
			broadphase->delete_element(broadphase_element);
			broadphase_element = nullptr;
		}
	}
	monitorable = p_monitorable;
}

SGCollisionObject3DInternal::SGCollisionObject3DInternal(ObjectType p_type) {
	object_type = p_type;
	world = nullptr;
	broadphase = nullptr;
	broadphase_element = nullptr;
	data = nullptr;
	collision_layer = 1;
	collision_mask = 1;
	monitorable = true;
}

SGCollisionObject3DInternal::~SGCollisionObject3DInternal() {
}

SGArea3DInternal::SGArea3DInternal()
	: SGCollisionObject3DInternal(OBJECT_AREA)
{
}

SGArea3DInternal::~SGArea3DInternal() {
}

SGBody3DInternal::SGBody3DInternal(BodyType p_type)
	: SGCollisionObject3DInternal(OBJECT_BODY)
{
	body_type = p_type;
	safe_margin = fixed(64);
}

SGBody3DInternal::~SGBody3DInternal() {
}
