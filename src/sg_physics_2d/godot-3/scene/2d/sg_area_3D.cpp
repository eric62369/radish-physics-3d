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

#include "sg_area_3D.h"

#include <core/engine.h>

#include "sg_static_body_3D.h"
#include "sg_kinematic_body_3D.h"
#include "../../servers/sg_physics_3D_server.h"
#include "../../../internal/sg_bodies_3D_internal.h"
#include "../../../internal/sg_world_3D_internal.h"

void SGArea3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_overlapping_areas"), &SGArea3D::get_overlapping_areas);
	ClassDB::bind_method(D_METHOD("get_overlapping_bodies"), &SGArea3D::get_overlapping_bodies);

	ClassDB::bind_method(D_METHOD("get_overlapping_area_collisions"), &SGArea3D::get_overlapping_area_collisions);
	ClassDB::bind_method(D_METHOD("get_overlapping_body_collisions"), &SGArea3D::get_overlapping_body_collisions);

	ClassDB::bind_method(D_METHOD("get_overlapping_area_count"), &SGArea3D::get_overlapping_area_count);
	ClassDB::bind_method(D_METHOD("get_overlapping_body_count"), &SGArea3D::get_overlapping_body_count);

	ClassDB::bind_method(D_METHOD("set_monitorable", "monitorable"), &SGArea3D::set_monitorable);
	ClassDB::bind_method(D_METHOD("get_monitorable"), &SGArea3D::get_monitorable);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "monitorable"), "set_monitorable", "get_monitorable");
}

Array SGArea3D::get_overlapping_areas() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_areas(rid);
}

Array SGArea3D::get_overlapping_bodies() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_bodies(rid);
}

Array SGArea3D::get_overlapping_area_collisions() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_area_collisions(rid);
}

Array SGArea3D::get_overlapping_body_collisions() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_body_collisions(rid);
}

int SGArea3D::get_overlapping_area_count() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_area_count(rid);
}

int SGArea3D::get_overlapping_body_count() const {
	return SGPhysics3DServer::get_singleton()->area_get_overlapping_body_count(rid);
}

void SGArea3D::set_monitorable(bool p_monitorable) {
	if (monitorable == p_monitorable) {
		return;
	}

	monitorable = p_monitorable;
	SGPhysics3DServer::get_singleton()->collision_object_set_monitorable(rid, p_monitorable);
}

bool SGArea3D::get_monitorable() const {
	return monitorable;
}

SGArea3D::SGArea3D()
	: SGCollisionObject3D(SGPhysics3DServer::get_singleton()->collision_object_create(SGPhysics3DServer::OBJECT_AREA))
{
}

SGArea3D::~SGArea3D() {
}
