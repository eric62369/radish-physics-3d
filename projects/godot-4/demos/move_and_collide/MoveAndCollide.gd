extends Node3D

@onready var character = $Character
@onready var area = $Area
@onready var ray_cast = $Character/SGRayCast2D
@onready var spot = $Spot

var rotation_speed = 3277 # 0.05
var velocity = SGFixed.vector3(0, 0, 0)

func _physics_process(delta: float) -> void:
	var rotating := false
	if Input.is_action_pressed("ui_left"):
		character.rotate_and_slide(-rotation_speed)
		rotating = true
	elif Input.is_action_pressed("ui_right"):
		character.rotate_and_slide(rotation_speed)
		rotating = true

	velocity.y = 0
	velocity.x = 0
	if Input.is_action_pressed("ui_up"):
		velocity.x = SGFixed.ONE
	elif Input.is_action_pressed("ui_down"):
		velocity.x = SGFixed.NEG_ONE

	if velocity.x != 0 or rotating:
		velocity.imul(SGFixed.ONE*3)
		velocity.rotate(character.fixed_rotation)
		var collision = character.move_and_collide(velocity)
		if collision:
			print(" == COLLIDES! ==")
			print (collision.collider)
			print ("normal: %s" % collision.normal.to_float())
			print ("remainder: %s" % collision.remainder.to_float())
			print ("slide remainder: %s" % collision.remainder.slide(collision.normal).to_float())

		ray_cast.update_raycast_collision()
		var overlapping_bodies = area.get_overlapping_bodies()

		if ray_cast.is_colliding() or (overlapping_bodies.size() > 0 && overlapping_bodies[0] == character):
			character.modulate = Color(1.0, 0.0, 0.0, 1.0)
		else:
			character.modulate = Color(1.0, 1.0, 1.0, 1.0)

		if ray_cast.is_colliding():
			spot.position = ray_cast.get_collision_point().to_float() - Vector2(5, 5)


func _process(delta):
	for child in self.get_children():
		if child is SGArea3D:
			var sg = child
			var capsule_mesh = CapsuleMesh.new()
			var shape = sg.get_child(0)
			if (shape is SGCollisionShape3D and shape.shape is SGCapsuleShape3D):
				shape = shape.shape
				# (Optional) Customize the CapsuleMesh properties
				capsule_mesh.radius = SGFixed.to_float(shape.radius)
				if (shape is SGCapsuleShape3D):
					capsule_mesh.height = SGFixed.to_float(shape.height + shape.radius*2)
				else:
					capsule_mesh.height = SGFixed.to_float(shape.radius)*2
				capsule_mesh.radial_segments = 18
				capsule_mesh.rings = 6
				
				var color = Color(1, 0, 0, 1)
				#for i in range(0, len(areas)):
					#if child.name == areas[i].name and colliding[i]:
						#color = Color (0, 1, 0, 1)

				DebugDraw.draw_mesh(capsule_mesh, sg.transform, color)
		else:
			continue
