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


#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include "./math/sg_fixed_singleton.h"
#include "./math/sg_fixed_vector2.h"
#include "./math/sg_fixed_rect2.h"
#include "./math/sg_fixed_transform_3D.h"
#include "./scene/3D/sg_fixed_position_3D.h"
#include "./scene/3D/sg_area_3D.h"
#include "./scene/3D/sg_static_body_3D.h"
#include "./scene/3D/sg_character_body_3D.h"
#include "./scene/3D/sg_ray_cast_3D.h"
#include "./scene/3D/sg_collision_shape_3D.h"
#include "./scene/3D/sg_collision_polygon_3D.h"
#include "./scene/resources/sg_shapes_3D.h"
#include "./servers/sg_physics_3D_server.h"
#include "../internal/sg_world_3D_internal.h"

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
#include "./editor/sg_collision_shape_3D_editor_plugin.h"
#include "./editor/sg_collision_polygon_3D_editor_plugin.h"
#endif

using namespace godot;

static SGFixed *fixed_singleton;
static SGPhysics3DServer *server_singleton;

void register_sg_physics_3D_extension_types(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(SGFixed);
		GDREGISTER_CLASS(SGFixedVector3);
		GDREGISTER_CLASS(SGFixedRect3);
		GDREGISTER_CLASS(SGFixedTransform3D);

		GDREGISTER_CLASS(SGFixedNode3D);
		GDREGISTER_CLASS(SGFixedPosition3D);
		GDREGISTER_CLASS(SGPhysics3DServer);
		GDREGISTER_VIRTUAL_CLASS(SGCollisionObject3D);
		GDREGISTER_CLASS(SGArea3D);
		GDREGISTER_CLASS(SGAreaCollision3D);
		GDREGISTER_CLASS(SGPhysicsBody3D);
		GDREGISTER_CLASS(SGStaticBody3D);
		GDREGISTER_CLASS(SGCharacterBody3D);
		GDREGISTER_CLASS(SGKinematicCollision3D);
		GDREGISTER_CLASS(SGRayCast3D);
		GDREGISTER_CLASS(SGRayCastCollision3D);

		GDREGISTER_CLASS(SGCollisionShape3D);
		GDREGISTER_CLASS(SGCollisionPolygon3D);

		GDREGISTER_VIRTUAL_CLASS(SGShape3D);
		GDREGISTER_CLASS(SGRectangleShape3D);
		GDREGISTER_CLASS(SGCircleShape3D);
		GDREGISTER_CLASS(SGCapsuleShape3D);

		fixed_singleton = memnew(SGFixed);
		Engine::get_singleton()->register_singleton("SGFixed", SGFixed::get_singleton());

		server_singleton = memnew(SGPhysics3DServer);
		Engine::get_singleton()->register_singleton("SGPhysics3DServer", SGPhysics3DServer::get_singleton());
	}

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(SGCollisionShape3DEditorPlugin);
		GDREGISTER_CLASS(SGCollisionPolygon3DEditor);
		GDREGISTER_CLASS(SGCollisionPolygon3DEditorPlugin);

		EditorPlugins::add_by_type<SGCollisionShape3DEditorPlugin>();
		EditorPlugins::add_by_type<SGCollisionPolygon3DEditorPlugin>();
	}
#endif
}

void unregister_sg_physics_3D_extension_types(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		Engine::get_singleton()->unregister_singleton("SGFixed");
		memdelete(fixed_singleton);

		Engine::get_singleton()->unregister_singleton("SGPhysics3DServer");
		memdelete(server_singleton);
	}

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::remove_by_type<SGCollisionShape3DEditorPlugin>();
	}
#endif

}

extern "C" {
GDExtensionBool GDE_EXPORT sg_physics_3D_extension_init(GDExtensionInterfaceGetProcAddress p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_initializer(register_sg_physics_3D_extension_types);
	init_obj.register_terminator(unregister_sg_physics_3D_extension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
