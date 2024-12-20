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

#ifndef SG_COLLISION_POLYGON_3D_EDITOR_PLUGIN_H
#define SG_COLLISION_POLYGON_3D_EDITOR_PLUGIN_H

#include "../scene/3D/sg_collision_polygon_3D.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/confirmation_dialog.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/panel.hpp>

class SGCollisionPolygon3DEditor : public HBoxContainer {
	GDCLASS(SGCollisionPolygon3DEditor, HBoxContainer);

	Button *button_create = nullptr;
	Button *button_edit = nullptr;
	Button *button_delete = nullptr;

	struct Vertex {
		Vertex() {}
		Vertex(int p_vertex) :
				vertex(p_vertex) {}
		Vertex(int p_polygon, int p_vertex) :
				polygon(p_polygon),
				vertex(p_vertex) {}

		bool operator==(const Vertex &p_vertex) const;
		bool operator!=(const Vertex &p_vertex) const;

		bool valid() const;

		int polygon = -1;
		int vertex = -1;
	};

	struct PosVertex : public Vertex {
		PosVertex() {}
		PosVertex(const Vertex &p_vertex, const Vector2 &p_pos) :
				Vertex(p_vertex.polygon, p_vertex.vertex),
				pos(p_pos) {}
		PosVertex(int p_polygon, int p_vertex, const Vector2 &p_pos) :
				Vertex(p_polygon, p_vertex),
				pos(p_pos) {}

		Vector2 pos;
	};

	PosVertex edited_point;
	Vertex hover_point; // point under mouse cursor
	Vertex selected_point; // currently selected
	PosVertex edge_point; // adding an edge point?

	PackedVector2Array pre_move_edit;
	PackedVector2Array wip;
	bool wip_active = false;

	bool _polygon_editing_enabled = false;

	SGCollisionPolygon3D *node;

	EditorPlugin *plugin = nullptr;
	Panel *panel = nullptr;
	ConfirmationDialog *create_resource = nullptr;

protected:
	enum {
		MODE_CREATE,
		MODE_EDIT,
		MODE_DELETE,
		MODE_CONT,
	};

	int mode = MODE_EDIT;

	void _menu_option(int p_option);

	// @todo Replace with callable_mp() and Callable.bind().
	void _menu_option_create();
	void _menu_option_edit();
	void _menu_option_delete();

	void _wip_changed();
	void _wip_close();
	void _wip_cancel();

	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

	Transform3D get_canvas_transform() const;

	void remove_point(const Vertex &p_vertex);
	Vertex get_active_point() const;
	PosVertex closest_point(const Vector2 &p_pos) const;
	PosVertex closest_edge_point(const Vector2 &p_pos) const;
	static Vector2 get_closest_point_to_segment(const Vector2 &p_point, const Vector2 *p_segment);

	bool _is_empty() const;

	int _get_polygon_count() const;
	Vector2 _get_offset(int p_idx) const;
	Variant _get_polygon(int p_idx) const;
	void _set_polygon(int p_idx, const Variant &p_polygon) const;

	void _action_add_polygon(const Variant &p_polygon);
	void _action_remove_polygon(int p_idx);
	void _action_set_polygon(int p_idx, const Variant &p_polygon);
	void _action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon);
	void _commit_action();

	bool _has_resource() const;
	void _create_resource();

public:
	void disable_polygon_editing(bool p_disable, String p_reason);

	bool forward_gui_input(const Ref<InputEvent> &p_event);
	void forward_canvas_draw_over_viewport(Control *p_overlay);

	void edit(SGCollisionPolygon3D *p_polygon);
	SGCollisionPolygon3DEditor(EditorPlugin *p_plugin = nullptr);
};

class SGCollisionPolygon3DEditorPlugin : public EditorPlugin {
	GDCLASS(SGCollisionPolygon3DEditorPlugin, EditorPlugin);

	SGCollisionPolygon3DEditor *polygon_editor = nullptr;

protected:
	static void _bind_methods();

	void _notification(int p_what);

public:
	virtual bool _forward_canvas_gui_input(const Ref<InputEvent> &p_event) override { return polygon_editor->forward_gui_input(p_event); }
	virtual void _forward_canvas_draw_over_viewport(Control *p_overlay) override { polygon_editor->forward_canvas_draw_over_viewport(p_overlay); }

	bool _has_main_screen() const override { return false; }
	virtual String _get_plugin_name() const override { return "SGCollisionPolygon3D"; }
	virtual void _edit(Object *p_object) override;
	virtual bool _handles(Object *p_object) const override;
	virtual void _make_visible(bool p_visible) override;

	SGCollisionPolygon3DEditorPlugin();
	~SGCollisionPolygon3DEditorPlugin();
};

#endif
