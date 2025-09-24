extends Node2D

@onready var area1 = $Area2D1
@onready var area2 = $Area2D2
@onready var area3 = $Area2D3

var colliding = [false, false, false]

func move_capsule(move_object: SGArea3D):
	var magnitude = 1
	var input_dir := Input.get_vector("debug_left", "debug_right", "debug_forward", "debug_backward")
	var direction := (Vector3(input_dir.x, 0, input_dir.y)).normalized()
	
	if (direction):
		move_object.fixed_position.x += direction.x * magnitude * 65536
		move_object.fixed_position.z += direction.z * magnitude * 65536
	
	if Input.is_action_pressed("debug_up"):
		move_object.fixed_position.y += magnitude * 65536
	elif Input.is_action_pressed("debug_down"):
		move_object.fixed_position.y += -magnitude * 65536
	move_object.orthonormalize()

func sync_to_physics_engine():
	area1.sync_to_physics_engine()
	area2.sync_to_physics_engine()
	area3.sync_to_physics_engine()

func _physics_process(delta):
	move_capsule(area1)
	sync_to_physics_engine()
	do_get_overlapping_areas()
	#if (area1.get_overlapping_bodies().size() > 0):
		#colliding[0] = true
	#else:
		#colliding[0] = false
	#
	#if (area2.get_overlapping_bodies().size() > 0):
		#colliding[1] = true
	#else:
		#colliding[1] = false
	#
	#if (area3.get_overlapping_bodies().size() > 0):
		#colliding[2] = true
	#else:
		#colliding[2] = false

func do_get_overlapping_areas() -> Array:
	if (area1.get_overlapping_areas().size() > 0):
		colliding[0] = true
	else:
		colliding[0] = false
	
	if (area2.get_overlapping_areas().size() > 0):
		colliding[1] = true
	else:
		colliding[1] = false
	
	if (area3.get_overlapping_areas().size() > 0):
		colliding[2] = true
	else:
		colliding[2] = false
	return area1.get_overlapping_areas()

func _process(delta):
	for child in self.get_children():
		if child is SGArea3D:
			var sg: SGArea3D = child
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
				if child.name == area1.name and colliding[0]:
					color = Color (0, 1, 0, 1)
				if child.name == area2.name and colliding[1]:
					color = Color (0, 1, 0, 1)
				if child.name == area3.name and colliding[2]:
					color = Color (0, 1, 0, 1)
				
				#DebugDraw.draw_cube(Vector3(0,0,0), 10, Color(1, 0, 1, 1))
				DebugDraw.draw_mesh(capsule_mesh, sg.transform, color)
				#DebugDraw.draw_mesh(capsule_mesh, Transform3D(Vector3(1, 0, 0),
													#Vector3(0, 1, 0),
													#Vector3(0, 0, 1),
													#Vector3(0, 0, 0)
													#), Color(1, 1, 1, 1))
				
		else:
			continue
