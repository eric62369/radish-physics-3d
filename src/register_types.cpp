#include "register_types.h"

#include "gdexample.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>


#include "./math/sg_fixed_singleton.h"
#include "./math/sg_fixed_vector2.h"
#include "./math/sg_fixed_rect2.h"
#include "./math/sg_fixed_transform_2d.h"
#include "./scene/2d/sg_fixed_position_2d.h"
#include "./scene/2d/sg_area_2d.h"
#include "./scene/2d/sg_static_body_2d.h"
#include "./scene/2d/sg_character_body_2d.h"
#include "./scene/2d/sg_ray_cast_2d.h"
#include "./scene/2d/sg_collision_shape_2d.h"
#include "./scene/2d/sg_collision_polygon_2d.h"
#include "./scene/resources/sg_shapes_2d.h"
#include "./servers/sg_physics_2d_server.h"
#include "./internal/sg_world_2d_internal.h"

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
#include "./editor/sg_collision_shape_2d_editor_plugin.h"
#include "./editor/sg_collision_polygon_2d_editor_plugin.h"
#endif

using namespace godot;

static SGFixed *fixed_singleton;
static SGPhysics2DServer *server_singleton;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(SGFixed);
		GDREGISTER_CLASS(SGFixedVector2);
		GDREGISTER_CLASS(SGFixedRect2);
		GDREGISTER_CLASS(SGFixedTransform2D);

		GDREGISTER_CLASS(SGFixedNode2D);
		GDREGISTER_CLASS(SGFixedPosition2D);
		GDREGISTER_CLASS(SGPhysics2DServer);
		GDREGISTER_VIRTUAL_CLASS(SGCollisionObject2D);
		GDREGISTER_CLASS(SGArea2D);
		GDREGISTER_CLASS(SGAreaCollision2D);
		GDREGISTER_CLASS(SGPhysicsBody2D);
		GDREGISTER_CLASS(SGStaticBody2D);
		GDREGISTER_CLASS(SGCharacterBody2D);
		GDREGISTER_CLASS(SGKinematicCollision2D);
		GDREGISTER_CLASS(SGRayCast2D);
		GDREGISTER_CLASS(SGRayCastCollision2D);

		GDREGISTER_CLASS(SGCollisionShape2D);
		GDREGISTER_CLASS(SGCollisionPolygon2D);

		GDREGISTER_VIRTUAL_CLASS(SGShape2D);
		GDREGISTER_CLASS(SGRectangleShape2D);
		GDREGISTER_CLASS(SGCircleShape2D);
		GDREGISTER_CLASS(SGCapsuleShape2D);

		fixed_singleton = memnew(SGFixed);
		Engine::get_singleton()->register_singleton("SGFixed", SGFixed::get_singleton());

		server_singleton = memnew(SGPhysics2DServer);
		Engine::get_singleton()->register_singleton("SGPhysics2DServer", SGPhysics2DServer::get_singleton());
	}

	GDREGISTER_CLASS(GDExample);
	//GDREGISTER_CLASS(YourClass);
#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(SGCollisionShape2DEditorPlugin);
		GDREGISTER_CLASS(SGCollisionPolygon2DEditor);
		GDREGISTER_CLASS(SGCollisionPolygon2DEditorPlugin);

		EditorPlugins::add_by_type<SGCollisionShape2DEditorPlugin>();
		EditorPlugins::add_by_type<SGCollisionPolygon2DEditorPlugin>();
	}
#endif
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		Engine::get_singleton()->unregister_singleton("SGFixed");
		memdelete(fixed_singleton);

		Engine::get_singleton()->unregister_singleton("SGPhysics2DServer");
		memdelete(server_singleton);
	}

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::remove_by_type<SGCollisionShape2DEditorPlugin>();
	}
#endif
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT rdphysics_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
