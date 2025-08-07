extends Node2D

@onready var area = $Area2D
@onready var static_body1 = $StaticBody1
@onready var static_body2 = $StaticBody2

func do_get_overlapping_bodies() -> Array:
	return area.get_overlapping_bodies()

func _process(delta):
	for child in self.get_children():
		if child is SGArea3D:
			var sg: SGFixedNode3D = child
			DebugDraw.draw_box(Vector3(
					SGFixed.to_float(sg.fixed_position.x), 
					SGFixed.to_float(sg.fixed_position.y), 
					SGFixed.to_float(sg.fixed_position.z)), 
				Vector3(
					SGFixed.to_float(sg.fixed_scale.x),
					SGFixed.to_float(sg.fixed_scale.y),
					SGFixed.to_float(sg.fixed_scale.z)), Color(0, 1, 0))
		elif child is SGStaticBody3D:
			var sg: SGFixedNode3D = child
			DebugDraw.draw_box(Vector3(
					SGFixed.to_float(sg.fixed_position.x), 
					SGFixed.to_float(sg.fixed_position.y), 
					SGFixed.to_float(sg.fixed_position.z)), 
				Vector3(
					SGFixed.to_float(sg.fixed_scale.x),
					SGFixed.to_float(sg.fixed_scale.y),
					SGFixed.to_float(sg.fixed_scale.z)), Color(1, 1, 0))
		else:
			continue
