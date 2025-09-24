extends Node2D

@export var speed = 1.0

@onready var area1 = $Area1
@onready var area2 = $Area2

var colliding: Array = [false, false]

func do_get_overlapping_bodies() -> Array:
	if (area1.get_overlapping_areas().size() > 0):
		colliding[0] = true
	else:
		colliding[0] = false
	
	if (area2.get_overlapping_areas().size() > 0):
		colliding[1] = true
	else:
		colliding[1] = false
	return area1.get_overlapping_bodies()

func move_capsule(move_object: SGArea3D, mod=1):
	var magnitude = speed*mod
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

func _physics_process(delta):
	move_capsule(area1)
	move_capsule(area2, -0.5)
	sync_to_physics_engine()
	do_get_overlapping_bodies()

func _process(delta):
	for child in self.get_children():
		if child is SGArea3D:
			var sg = child
			var mesh = SphereMesh.new()
			var shape = sg.get_child(0)
			if (shape is SGCollisionShape3D and shape.shape is SGCircleShape3D):
				shape = shape.shape
				# (Optional) Customize the CapsuleMesh properties
				mesh.radius = SGFixed.to_float(shape.radius)
				mesh.height = SGFixed.to_float(shape.radius)*2
				mesh.radial_segments = 18
				mesh.rings = 6
				
				var color = Color(1, 0, 0, 1)
				if child.name == area1.name and colliding[0]:
					color = Color (0, 1, 0, 1)
				if child.name == area2.name and colliding[1]:
					color = Color (0, 1, 0, 1)

				DebugDraw.draw_mesh(mesh, sg.transform, color)
		else:
			continue
