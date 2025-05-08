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

#ifndef SG_SHAPES_3D_H
#define SG_SHAPES_3D_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include "../../math/sg_fixed_vector3.h"

class SGCollisionShape3D;

class SGShape3D : public Resource {
	GDCLASS(SGShape3D, Resource);

	friend class SGCollisionShape3D;

	Ref<ArrayMesh> debug_mesh_cache;
	
	Color debug_color = Color(0.0, 0.0, 0.0, 0.0);
	bool debug_fill = true;
#ifdef DEBUG_ENABLED
	bool debug_properties_edited = false;
#endif // DEBUG_ENABLED

protected:
	static void _bind_methods();

	virtual RID create_internal_shape() const { return RID(); }

	virtual Vector<Vector3> get_debug_mesh_lines() const = 0; // { return Vector<Vector3>(); }
	virtual Ref<ArrayMesh> get_debug_arraymesh_faces(const Color &p_modulate) const = 0;
	Ref<ArrayMesh> get_debug_mesh();

	SGShape3D();
public:
	virtual void sync_to_physics_engine(RID p_internal_shape) const {}

	virtual void draw(const RID &p_to_rid, const Color &p_color) {}

	virtual ~SGShape3D();
};

class SGRectangleShape3D : public SGShape3D, public SGFixedVector3Watcher {
	GDCLASS(SGRectangleShape3D, SGShape3D);

	Ref<SGFixedVector3> extents;

protected:
	static void _bind_methods();

	virtual RID create_internal_shape() const override;

	int64_t _get_extents_x() const;
	void _set_extents_x(int64_t p_x);
	int64_t _get_extents_y() const;
	void _set_extents_y(int64_t p_y);
	int64_t _get_extents_z() const;
	void _set_extents_z(int64_t p_y);

public:
	void set_extents(const Ref<SGFixedVector3>& p_extents);
	Ref<SGFixedVector3> get_extents();

	void fixed_vector3_changed(SGFixedVector3 *p_vector);

	virtual void sync_to_physics_engine(RID p_internal_shape) const override;

	virtual void draw(const RID &p_to_rid, const Color &p_color) override;

	SGRectangleShape3D();
	~SGRectangleShape3D();
};

class SGCircleShape3D : public SGShape3D {
	GDCLASS(SGCircleShape3D, SGShape3D);

	int64_t radius;

protected:
	static void _bind_methods();

	virtual RID create_internal_shape() const override;

public:
	void set_radius(int64_t p_radius);
	int64_t get_radius() const;

	virtual void sync_to_physics_engine(RID p_internal_shape) const override;

	virtual void draw(const RID &p_to_rid, const Color &p_color) override;

	SGCircleShape3D();
	~SGCircleShape3D();
};

class SGCapsuleShape3D : public SGShape3D {
	GDCLASS(SGCapsuleShape3D, SGShape3D);

	int64_t radius;
	int64_t height;

protected:
	static void _bind_methods();

	virtual RID create_internal_shape() const override;

public:
	void set_radius(int p_radius);
	int64_t get_radius() const;
	void set_height(int p_height);
	int64_t get_height() const;

	virtual void sync_to_physics_engine(RID p_internal_shape) const override;

	virtual void draw(const RID& p_to_rid, const Color& p_color) override;

	SGCapsuleShape3D();
	~SGCapsuleShape3D();
};

#endif
