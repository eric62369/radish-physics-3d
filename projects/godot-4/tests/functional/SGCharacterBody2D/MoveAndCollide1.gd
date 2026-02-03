extends Node3D

@onready var static_body1: SGStaticBody3D = $StaticBody1
@onready var static_body2: SGStaticBody3D = $StaticBody2
@onready var kinematic_body: SGCharacterBody3D = $KinematicBody
@onready var start_transform: SGFixedTransform3D = kinematic_body.fixed_transform.copy()

func reset_kinematic_body() -> void:
	kinematic_body.fixed_transform = start_transform
	kinematic_body.sync_to_physics_engine()

func do_move_and_collide() -> SGKinematicCollision3D:
	var vector = SGFixed.vector3(0, -SGFixed.TWO, 0)
	var collision = kinematic_body.move_and_collide(vector)
	var colliderObject = collision.collider
	var colliderRID = collision.collider_rid
	var normal = collision.normal
	var remainder = collision.remainder

	return collision

func _on_Button_pressed() -> void:
	reset_kinematic_body()
	do_move_and_collide()
