# MyCustomEditorPlugin.gd
@tool
extends EditorPlugin

const MyCustomGizmoPlugin = preload("res://addons/sg-shape-gizmo/shape3d_gizmo.gd")

var gizmo_plugin = MyCustomGizmoPlugin.new()

func _init():
	create_material("main", Color(1, 0, 0))
	create_handle_material("handles")


func _redraw(gizmo):
	gizmo.clear()

	var node3d = gizmo.get_node_3d()

	var lines = PackedVector3Array()

	lines.push_back(Vector3(0, 1, 0))
	lines.push_back(Vector3(0, node3d.my_custom_value, 0))

	var handles = PackedVector3Array()

	handles.push_back(Vector3(0, 1, 0))
	handles.push_back(Vector3(0, node3d.my_custom_value, 0))

	gizmo.add_lines(lines, get_material("main", gizmo), false)
	gizmo.add_handles(handles, get_material("handles", gizmo), [])

func _has_gizmo(node):
	return node is MyCustomNode3D

func _enter_tree():
	add_node_3d_gizmo_plugin(gizmo_plugin)


func _exit_tree():
	remove_node_3d_gizmo_plugin(gizmo_plugin)
