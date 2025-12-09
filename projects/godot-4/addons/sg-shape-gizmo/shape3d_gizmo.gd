# my_custom_gizmo_plugin.gd
extends EditorNode3DGizmoPlugin

func _get_gizmo_name():
	return "SGShape3DGizmo"

func _has_gizmo(node):
	return node is MyCustomNode3D
