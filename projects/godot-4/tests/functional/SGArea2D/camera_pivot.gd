extends Node3D

@export var speed: float = 1.0
@export var LOOKAROUND_SPEED: float = .005

var rot_x = 0
var rot_y = 0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta: float) -> void:
	var magnitude = 0.5 * speed
	var rot_mag = .01
	var input_dir := Input.get_vector("player_left", "player_right", "player_up", "player_down")
	var direction := (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	
	if (direction):
		self.position.x += direction.x * magnitude
		self.position.z += direction.z * magnitude
	
	if Input.is_action_pressed("player_y_up"):
		self.position.y += magnitude
	elif Input.is_action_pressed("player_y_down"):
		self.position.y += -magnitude
	self.orthonormalize()

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:# and event.button_mask & 1:
		# modify accumulated mouse rotation
		rot_x += -event.relative.x * LOOKAROUND_SPEED
		rot_y += -event.relative.y * LOOKAROUND_SPEED
		transform.basis = Basis() # reset rotation
		rotate_object_local(Vector3(0, 1, 0), rot_x) # first rotate in Y
		rotation.x = clamp(rotation.x, deg_to_rad(-30), deg_to_rad(60))
		$Camera3D.transform.basis = Basis()
		$Camera3D.rotate_object_local(Vector3(1, 0, 0), rot_y) # then rotate in X
