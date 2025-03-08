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

#ifndef SG_FIXED_NODE_3D_H
#define SG_FIXED_NODE_3D_H

#include <godot_cpp/classes/node3D.hpp>

#include "../../math/sg_fixed_vector3.h"
#include "../../math/sg_fixed_transform_3D.h"

class SGCollisionObject3D;

class SGFixedNode3D : public Node3D, public SGFixedVector3Watcher {
	GDCLASS(SGFixedNode3D, Node3D);

	friend SGCollisionObject3D;

	Ref<SGFixedTransform3D> fixed_transform;
	Ref<SGFixedVector3> fixed_scale;
	int64_t fixed_rotation;
	bool fixed_xform_dirty;

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	bool updating_transform;
#endif

protected:
	static void _bind_methods();
	void _notification(int p_what);

#if defined(TOOLS_ENABLED) || defined(DEBUG_ENABLED)
	//virtual void _changed_callback(Object *p_changed, const char *p_prop) override;
#endif

	void _update_fixed_transform_rotation_and_scale();

	_FORCE_INLINE_ SGFixedTransform3DInternal get_fixed_transform_internal() const { return fixed_transform->get_internal(); }
	SGFixedTransform3DInternal get_global_fixed_transform_internal() const;

	void update_fixed_transform_internal(const SGFixedTransform3DInternal &p_transform);
	void update_global_fixed_transform_internal(const SGFixedTransform3DInternal &p_global_transform);


	int64_t _get_fixed_position_x() const;
	void _set_fixed_position_x(int64_t p_x);
	int64_t _get_fixed_position_y() const;
	void _set_fixed_position_y(int64_t p_y);
	int64_t _get_fixed_position_z() const;
	void _set_fixed_position_z(int64_t p_z);

	int64_t _get_fixed_scale_x() const;
	void _set_fixed_scale_x(int64_t p_x);
	int64_t _get_fixed_scale_y() const;
	void _set_fixed_scale_y(int64_t p_y);
	int64_t _get_fixed_scale_z() const;
	void _set_fixed_scale_z(int64_t p_z);

	void transform_changed();

public:
	void set_fixed_transform(const Ref<SGFixedTransform3D> &p_transform);
	Ref<SGFixedTransform3D> get_fixed_transform() const;

	void set_fixed_position(const Ref<SGFixedVector3> &p_fixed_position);
	Ref<SGFixedVector3> get_fixed_position() const;

	void set_fixed_scale(const Ref<SGFixedVector3> &p_fixed_scale);
	Ref<SGFixedVector3> get_fixed_scale() const;

	void set_fixed_rotation(int64_t p_fixed_rotation);
	int64_t get_fixed_rotation() const;

	void set_global_fixed_transform(const Ref<SGFixedTransform3D> &p_global_transform);
	Ref<SGFixedTransform3D> get_global_fixed_transform() const;

	void set_global_fixed_position(const Ref<SGFixedVector3> &p_fixed_position);
	Ref<SGFixedVector3> get_global_fixed_position();

	void set_fixed_position_internal(const SGFixedVector3Internal &p_fixed_position);
	void set_global_fixed_position_internal(const SGFixedVector3Internal &p_fixed_position);

	void set_global_fixed_rotation(int64_t p_fixed_rotation);
	int64_t get_global_fixed_rotation() const;

	void update_float_transform();

	void fixed_vector3_changed(SGFixedVector3 *p_vector);

	SGFixedNode3D();
	~SGFixedNode3D();
};

#endif
