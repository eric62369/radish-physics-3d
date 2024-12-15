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

/*
 * NOTE: This is used for building the Godot 3 module!
 * It has to be at the top-level for technical reasons.
 */

#include "register_types.h"

#include <core/class_db.h>
#include <core/engine.h>

#include "./godot-3/math/sg_fixed_singleton.h"
#include "./godot-3/math/sg_fixed_vector2.h"
#include "./godot-3/math/sg_fixed_rect2.h"
#include "./godot-3/math/sg_fixed_transform_3D.h"
#include "./godot-3/math/sg_a_star.h"
#include "./godot-3/scene/3D/sg_fixed_position_3D.h"
#include "./godot-3/scene/3D/sg_area_3D.h"
#include "./godot-3/scene/3D/sg_static_body_3D.h"
#include "./godot-3/scene/3D/sg_kinematic_body_3D.h"
#include "./godot-3/scene/3D/sg_ray_cast_3D.h"
#include "./godot-3/scene/3D/sg_collision_shape_3D.h"
#include "./godot-3/scene/3D/sg_collision_polygon_3D.h"
#include "./godot-3/scene/3D/sg_ysort.h"
#include "./godot-3/scene/resources/sg_shapes_3D.h"
#include "./godot-3/scene/resources/sg_curve_3D.h"
#include "./godot-3/scene/3D/sg_path_3D.h"
#include "./godot-3/scene/animation/sg_tween.h"
#include "./godot-3/servers/sg_physics_3D_server.h"
#include "./internal/sg_world_3D_internal.h"

#ifdef TOOLS_ENABLED
#include "./godot-3/editor/sg_fixed_math_editor_plugin.h"
#include "./godot-3/editor/sg_collision_shape_3D_editor_plugin.h"
#include "./godot-3/editor/sg_collision_polygon_3D_editor_plugin.h"
#include "./godot-3/editor/sg_path_3D_editor_plugin.h"
#endif

static SGFixed *fixed_singleton;
static SGPhysics3DServer *server_singleton;

void register_sg_physics_3D_types() {
	ClassDB::register_class<SGFixed>();
	ClassDB::register_class<SGFixedVector2>();
	ClassDB::register_class<SGFixedRect2>();
	ClassDB::register_class<SGFixedTransform3D>();

	ClassDB::register_class<SGFixedNode3D>();
	ClassDB::register_class<SGFixedPosition3D>();
	ClassDB::register_class<SGPhysics3DServer>();
	ClassDB::register_virtual_class<SGCollisionObject3D>();
	ClassDB::register_class<SGArea3D>();
	ClassDB::register_class<SGAreaCollision3D>();
	ClassDB::register_class<SGStaticBody3D>();
	ClassDB::register_class<SGKinematicBody3D>();
	ClassDB::register_class<SGKinematicCollision3D>();
	ClassDB::register_class<SGRayCast3D>();
	ClassDB::register_class<SGYSort>();

	ClassDB::register_class<SGCollisionShape3D>();
	ClassDB::register_class<SGCollisionPolygon3D>();

	ClassDB::register_virtual_class<SGShape3D>();
	ClassDB::register_class<SGRectangleShape3D>();
	ClassDB::register_class<SGCircleShape3D>();
	ClassDB::register_class<SGCapsuleShape3D>();

	ClassDB::register_class<SGCurve3D>();
	ClassDB::register_class<SGPath3D>();
	ClassDB::register_class<SGPathFollow3D>();

	ClassDB::register_class<SGTween>();
	ClassDB::register_class<SGAStar3D>();

	fixed_singleton = memnew(SGFixed);
	Engine::get_singleton()->add_singleton(Engine::Singleton("SGFixed", SGFixed::get_singleton()));

	server_singleton = memnew(SGPhysics3DServer);
	Engine::get_singleton()->add_singleton(Engine::Singleton("SGPhysics3DServer", SGPhysics3DServer::get_singleton()));

#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<SGFixedMathEditorPlugin>();
	EditorPlugins::add_by_type<SGCollisionShape3DEditorPlugin>();
	EditorPlugins::add_by_type<SGCollisionPolygon3DEditorPlugin>();
	EditorPlugins::add_by_type<SGPath3DEditorPlugin>();
#endif
}

void unregister_sg_physics_3D_types() {
	memdelete(fixed_singleton);
	memdelete(server_singleton);
}
