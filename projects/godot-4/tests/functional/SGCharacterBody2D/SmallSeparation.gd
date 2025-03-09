extends Node2D

@onready var kinematic := $SGCharacterBody2D
@onready var circle := $SGStaticBody2D/Circle
@onready var square := $SGStaticBody2D/Square
@onready var polygon := $SGStaticBody2D/Polygon
var velocity := SGFixedVector3.new()
@onready var original_fixed_position : SGFixedVector3 = kinematic.fixed_position.copy()


func test_with_circle() -> SGKinematicCollision3D:
	circle.disabled = false
	square.disabled = true
	polygon.disabled = true
	kinematic.fixed_position = original_fixed_position.copy()
	kinematic.sync_to_physics_engine() 
	kinematic.move_and_collide(velocity)
	kinematic.fixed_position_x -= 1
	kinematic.fixed_position_y -= 1
	kinematic.sync_to_physics_engine() 
	return kinematic.move_and_collide(velocity)

func test_with_square() -> SGKinematicCollision3D:
	circle.disabled = true
	square.disabled = false
	polygon.disabled = true
	kinematic.fixed_position = original_fixed_position.copy()
	kinematic.sync_to_physics_engine() 
	kinematic.move_and_collide(velocity)
	kinematic.fixed_position_x -= 1
	kinematic.fixed_position_y -= 1
	kinematic.sync_to_physics_engine() 
	return kinematic.move_and_collide(velocity)

func test_with_polygon() -> SGKinematicCollision3D:
	circle.disabled = true
	square.disabled = true
	polygon.disabled = false
	kinematic.fixed_position = original_fixed_position.copy()
	kinematic.sync_to_physics_engine() 
	kinematic.move_and_collide(velocity)
	kinematic.fixed_position_x -= 1
	kinematic.fixed_position_y -= 1
	kinematic.sync_to_physics_engine() 
	return kinematic.move_and_collide(velocity)
