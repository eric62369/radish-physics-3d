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

#include "sg_shapes_3D.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/templates/vector.hpp>

#include "../../servers/sg_physics_3D_server.h"

void SGShape3D::_bind_methods() {
}

SGShape3D::SGShape3D() {
}

SGShape3D::~SGShape3D() {
}

void SGRectangleShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_extents"), &SGRectangleShape3D::get_extents);
	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &SGRectangleShape3D::set_extents);
	ClassDB::bind_method(D_METHOD("_get_extents_x"), &SGRectangleShape3D::_get_extents_x);
	ClassDB::bind_method(D_METHOD("_set_extents_x", "x"), &SGRectangleShape3D::_set_extents_x);
	ClassDB::bind_method(D_METHOD("_get_extents_y"), &SGRectangleShape3D::_get_extents_y);
	ClassDB::bind_method(D_METHOD("_set_extents_y", "y"), &SGRectangleShape3D::_set_extents_y);

	// For editor and storage.
	ADD_PROPERTY(PropertyInfo(Variant::INT, "extents_x"), "_set_extents_x", "_get_extents_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "extents_y"), "_set_extents_y", "_get_extents_y");

	// For code only.
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "extents", PROPERTY_HINT_NONE, "", 0), "set_extents", "get_extents");
}

void SGRectangleShape3D::set_extents(const Ref<SGFixedVector3>& p_extents) {
	ERR_FAIL_COND(!p_extents.is_valid());

	extents->set_internal(p_extents->get_internal());
	emit_changed();
}

Ref<SGFixedVector3> SGRectangleShape3D::get_extents() {
	return extents;
}

void SGRectangleShape3D::fixed_vector2_changed(SGFixedVector3 *p_vector) {
	emit_changed();
}

RID SGRectangleShape3D::create_internal_shape() const {
	return SGPhysics3DServer::get_singleton()->shape_create(SGPhysics3DServer::SHAPE_RECTANGLE);
}

int64_t SGRectangleShape3D::_get_extents_x() const {
	return extents->get_x();
}

void SGRectangleShape3D::_set_extents_x(int64_t p_x) {
	extents->set_x(p_x);
}

int64_t SGRectangleShape3D::_get_extents_y() const {
	return extents->get_y();
}

void SGRectangleShape3D::_set_extents_y(int64_t p_y) {
	extents->set_y(p_y);
}

void SGRectangleShape3D::sync_to_physics_engine(RID p_internal_shape) const {
	SGPhysics3DServer::get_singleton()->rectangle_set_extents(p_internal_shape, extents);
}

// void SGRectangleShape3D::draw(const RID &p_to_rid, const Color &p_color) {  // TODO: draw rectangle3D?
// 	Vector3 float_extents = extents->to_float();

// 	RenderingServer::get_singleton()->canvas_item_add_rect(p_to_rid, Rect2(-float_extents, float_extents * 2.0), p_color);
// }

SGRectangleShape3D::SGRectangleShape3D() : SGShape3D(),
	extents(Ref<SGFixedVector3>(memnew(SGFixedVector3(SGFixedVector3Internal(fixed(655360), fixed(655360), fixed(655360))))))
{
	extents->set_watcher(this);
}

SGRectangleShape3D::~SGRectangleShape3D() {
	extents->set_watcher(nullptr);
}


void SGCircleShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_radius"), &SGCircleShape3D::get_radius);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SGCircleShape3D::set_radius);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "radius"), "set_radius", "get_radius");
}

void SGCircleShape3D::set_radius(int64_t p_radius) {
	radius = p_radius;
	emit_changed();
}

int64_t SGCircleShape3D::get_radius() const {
	return radius;
}

RID SGCircleShape3D::create_internal_shape() const {
	return SGPhysics3DServer::get_singleton()->shape_create(SGPhysics3DServer::SHAPE_CIRCLE);
}

void SGCircleShape3D::sync_to_physics_engine(RID p_internal_shape) const {
	SGPhysics3DServer::get_singleton()->circle_set_radius(p_internal_shape, radius);
}

void SGCircleShape3D::draw(const RID &p_to_rid, const Color &p_color) {
	float float_radius = fixed(radius).to_float();

	PackedVector2Array points;
	for (int i = 0; i < 24; i++) {

		points.push_back(Vector2(Math::cos(i * Math_PI * 2 / 24.0), Math::sin(i * Math_PI * 2 / 24.0)) * float_radius);
	}

	PackedColorArray col;
	col.push_back(p_color);
	RenderingServer::get_singleton()->canvas_item_add_polygon(p_to_rid, points, col);
}

SGCircleShape3D::SGCircleShape3D() : SGShape3D(),
	radius(655360)
{
}

SGCircleShape3D::~SGCircleShape3D() {
}


void SGCapsuleShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_radius"), &SGCapsuleShape3D::get_radius);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SGCapsuleShape3D::set_radius);
	ClassDB::bind_method(D_METHOD("get_height"), &SGCapsuleShape3D::get_height);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &SGCapsuleShape3D::set_height);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "radius"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");
}

void SGCapsuleShape3D::set_radius(int p_radius) {
	radius = p_radius;
	emit_changed();
}

int64_t SGCapsuleShape3D::get_radius() const {
	return radius;
}

void SGCapsuleShape3D::set_height(int p_height) {
	height = p_height;
	emit_changed();
}

int64_t SGCapsuleShape3D::get_height() const {
	return height;
}

RID SGCapsuleShape3D::create_internal_shape() const {
	return SGPhysics3DServer::get_singleton()->shape_create(SGPhysics3DServer::SHAPE_CAPSULE);
}

void SGCapsuleShape3D::sync_to_physics_engine(RID p_internal_shape) const {
	SGPhysics3DServer *physics_server = SGPhysics3DServer::get_singleton();
	physics_server->capsule_set_radius(p_internal_shape, radius);
	physics_server->capsule_set_height(p_internal_shape, height);
}

void SGCapsuleShape3D::draw(const RID& p_to_rid, const Color& p_color) {
	float float_height = fixed(height).to_float();
	float float_radius = fixed(radius).to_float();

	PackedVector2Array points;
	for (int i = 12; i < 24; i++) {
		points.push_back(Vector2(Math::cos(i * Math_PI * 2 / 24.0), Math::sin(i * Math_PI * 2 / 24.0)) * float_radius + Vector2(0, -float_height/2));
	}
	for (int i = 0; i < 12; i++) {
		points.push_back(Vector2(Math::cos(i * Math_PI * 2 / 24.0), Math::sin(i * Math_PI * 2 / 24.0)) * float_radius + Vector2(0, float_height / 2));
	}

	PackedColorArray col;
	col.push_back(p_color);
	RenderingServer::get_singleton()->canvas_item_add_polygon(p_to_rid, points, col);
}

SGCapsuleShape3D::SGCapsuleShape3D() : SGShape3D(),
radius(655360),
height(655360)
{
}

SGCapsuleShape3D::~SGCapsuleShape3D() {
}
