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

#include "sg_collision_polygon_3D_editor_plugin.h"

Node3D *SGCollisionPolygon3DEditor::_get_node() const {
	return node;
}

void SGCollisionPolygon3DEditor::_set_node(Node *p_polygon) {

	node = Object::cast_to<SGCollisionPolygon3D>(p_polygon);
}

SGCollisionPolygon3DEditor::SGCollisionPolygon3DEditor(EditorNode *p_editor) :
		AbstractPolygon3DEditor(p_editor) {
	node = NULL;
}

SGCollisionPolygon3DEditorPlugin::SGCollisionPolygon3DEditorPlugin(EditorNode *p_node) :
		AbstractPolygon3DEditorPlugin(p_node, memnew(SGCollisionPolygon3DEditor(p_node)), "SGCollisionPolygon3D") {
}
