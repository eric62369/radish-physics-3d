extends Camera3D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta: float) -> void:
	var direction = Vector3.ZERO
	var magnitude = 0.5
	var rot_mag = .01
	if Input.is_action_pressed("player_right"):
		translate_object_local(magnitude*transform.basis.x)
	if Input.is_action_pressed("player_left"):
		translate_object_local(-magnitude*transform.basis.x)
	if Input.is_action_pressed("player_down"):
		translate_object_local(magnitude*transform.basis.z)
	if Input.is_action_pressed("player_up"):
		translate_object_local(-magnitude*transform.basis.z)
	
	if Input.is_action_pressed("player_y_up"):
		self.position.y += magnitude
	elif Input.is_action_pressed("player_y_down"):
		self.position.y += -magnitude

	if Input.is_action_pressed("player_rotate_left"):
		transform.basis = transform.basis.rotated(Vector3(0, 1, 0), -rot_mag)
	elif Input.is_action_pressed("player_rotate_right"):
		transform.basis = transform.basis.rotated(Vector3(0, 1, 0), rot_mag)
	self.orthonormalize()
