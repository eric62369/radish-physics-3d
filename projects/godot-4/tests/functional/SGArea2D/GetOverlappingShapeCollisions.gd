extends Node2D

@onready var detector_area1 = $DetectorArea1
@onready var detector_area2 = $DetectorArea2

@onready var colliding_area = $CollidingArea
@onready var colliding_area_shape1 = $CollidingArea/Shape1
@onready var colliding_area_shape2 = $CollidingArea/Shape2

@onready var colliding_body = $CollidingBody
@onready var colliding_body_shape1 = $CollidingBody/Shape1
@onready var colliding_body_shape2 = $CollidingBody/Shape2

@onready var areas = [detector_area1, detector_area2, colliding_area, colliding_body]
var colliding: Array = [false, false, false, false]

func do_get_overlapping_bodies() -> Array:
	for i in range(0, len(areas)):
		if (areas[i] is SGArea3D and areas[i].get_overlapping_areas().size() > 0):
			colliding[i] = true
		elif (areas[i] is SGArea3D and areas[i].get_overlapping_bodies().size() > 0):
			colliding[i] = true
		else:
			colliding[i] = false
	return areas[0].get_overlapping_bodies()

func move_capsule(move_object: SGArea3D, mod=1):
	var magnitude = 3*mod
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
	for area in areas:
		area.sync_to_physics_engine()
	
func _physics_process(delta):
	move_capsule(detector_area1)
	sync_to_physics_engine()
	do_get_overlapping_bodies()

func _process(delta):
	for child in self.get_children():
		if child is SGArea3D or child is SGStaticBody3D:
			var sg:SGFixedNode3D = child
			for SGshape in sg.get_children():
				var capsule_mesh = CapsuleMesh.new()
				if (SGshape is SGCollisionShape3D):
					var shape = SGshape.shape
					# (Optional) Customize the CapsuleMesh properties
					capsule_mesh.radius = SGFixed.to_float(shape.radius)
					if (shape is SGCapsuleShape3D):
						capsule_mesh.height = SGFixed.to_float(shape.height + shape.radius*2)
					else:
						capsule_mesh.height = SGFixed.to_float(shape.radius)*2
					capsule_mesh.radial_segments = 18
					capsule_mesh.rings = 6
					
					var color = Color(1, 0, 0, 1)
					for i in range(0, len(areas)):
						if child.name == areas[i].name and colliding[i]:
							color = Color (0, 1, 0, 1)
					var transform:Transform3D = sg.transform
					var other_transform:Transform3D = SGshape.transform
					transform.origin.x += other_transform.origin.x
					transform.origin.y += other_transform.origin.y
					transform.origin.z += other_transform.origin.z
					DebugDraw.draw_mesh(capsule_mesh, transform, color)
		else:
			continue
