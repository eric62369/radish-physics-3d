extends Node2D

@onready var area = $Area2D
@onready var static_body1 = $StaticBody1
@onready var static_body2 = $StaticBody2

func do_get_overlapping_bodies() -> Array:
	return area.get_overlapping_bodies()


func _process(delta):
	for child in self.get_children():
		if child is SGArea3D or child is SGStaticBody3D:
			var sg = child
			var capsule_mesh = CapsuleMesh.new()
			var shape = sg.get_child(0)
			if (shape is SGCollisionShape3D and shape.shape is SGCapsuleShape3D):
				shape = shape.shape
				# (Optional) Customize the CapsuleMesh properties
				capsule_mesh.radius = SGFixed.to_float(shape.radius)
				capsule_mesh.height = SGFixed.to_float(shape.height)
				capsule_mesh.radial_segments = 18
				capsule_mesh.rings = 6
				
				var color = Color(1, 0, 0, 1)
				if child is SGArea3D:
					color = Color (0, 1, 0, 1)

				DebugDraw.draw_mesh(capsule_mesh, sg.transform, color)
		else:
			continue


#func _process(delta):
	#for child in self.get_children():
		#if child is SGArea3D:
			#var sg: SGFixedNode3D = child
			#DebugDraw.draw_box(Vector3(
					#SGFixed.to_float(sg.fixed_position.x), 
					#SGFixed.to_float(sg.fixed_position.y), 
					#SGFixed.to_float(sg.fixed_position.z)), 
				#Vector3(
					#SGFixed.to_float(sg.fixed_scale.x),
					#SGFixed.to_float(sg.fixed_scale.y),
					#SGFixed.to_float(sg.fixed_scale.z)), Color(0, 1, 0))
		#elif child is SGStaticBody3D:
			#var sg: SGFixedNode3D = child
			#DebugDraw.draw_box(Vector3(
					#SGFixed.to_float(sg.fixed_position.x), 
					#SGFixed.to_float(sg.fixed_position.y), 
					#SGFixed.to_float(sg.fixed_position.z)), 
				#Vector3(
					#SGFixed.to_float(sg.fixed_scale.x),
					#SGFixed.to_float(sg.fixed_scale.y),
					#SGFixed.to_float(sg.fixed_scale.z)), Color(1, 1, 0))
		#else:
			#continue
