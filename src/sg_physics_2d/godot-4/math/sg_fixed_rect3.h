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

#ifndef SG_FIXED_RECT3_H
#define SG_FIXED_RECT3_H

#include <godot_cpp/classes/ref_counted.hpp>

#include "sg_fixed_vector3.h"
#include "../../internal/sg_fixed_rect3_internal.h"

using namespace godot;

class SGFixedRect3 : public RefCounted {

	GDCLASS(SGFixedRect3, RefCounted);

	Ref<SGFixedVector3> position;
	Ref<SGFixedVector3> size;

protected:
	static void _bind_methods();

public:

	_FORCE_INLINE_ SGFixedRect3Internal get_internal() const {
		return SGFixedRect3Internal(position->get_internal(), size->get_internal());
	}

	_FORCE_INLINE_ void set_internal(const SGFixedRect3Internal &p_internal) {
		position->set_internal(p_internal.position);
		size->set_internal(p_internal.size);
	}

	Ref<SGFixedVector3> get_position();
	void set_position(const Ref<SGFixedVector3> &p_position);

	Ref<SGFixedVector3> get_size();
	void set_size(const Ref<SGFixedVector3> &p_size);

	bool has_point(const Ref<SGFixedVector3> &p_point) const;
	bool intersects(const Ref<SGFixedRect3> &p_other) const;
	Ref<SGFixedRect3> merge(const Ref<SGFixedRect3> &p_rect) const;
	Ref<SGFixedRect3> expanded(const Ref<SGFixedVector3> &p_vector);

	_FORCE_INLINE_ static Ref<SGFixedRect3> from_internal(const SGFixedRect3Internal &p_internal) {
		Ref<SGFixedRect3> ret(memnew(SGFixedRect3));
		ret->set_internal(p_internal);
		return ret;
	}

	SGFixedRect3();
	SGFixedRect3(const SGFixedRect3Internal &p_internal);
	~SGFixedRect3();
};

#endif
