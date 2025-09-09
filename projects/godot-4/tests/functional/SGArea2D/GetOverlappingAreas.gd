extends Node2D


@onready var area1 = $Area2D1
@onready var area2 = $Area2D2
@onready var area3 = $Area2D3

#var capsule_mesh: CapsuleMesh

#func _ready():
	## Create a new MeshInstance3D node
	#var mesh_instance = MeshInstance3D.new()
	#add_child(mesh_instance)
#
	## Create a new CapsuleMesh
	#var capsule_mesh = CapsuleMesh.new()
#
	## (Optional) Customize the CapsuleMesh properties
	#capsule_mesh.radius = .5
	#capsule_mesh.height = 2
	#capsule_mesh.radial_segments = 32
	#capsule_mesh.rings = 8
#
	## Assign the CapsuleMesh to the MeshInstance3D
	#mesh_instance = $MeshInstance3D
	#mesh_instance.mesh = capsule_mesh
#
	## (Optional) Create and assign a material
	#var material = StandardMaterial3D.new()
	#material.albedo_color = Color(1, 0, 0) # Red color
	#capsule_mesh.material = material

func do_get_overlapping_areas() -> Array:
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

				#DebugDraw.draw_cube(Vector3(0,0,0), 10, Color(1, 0, 1, 1))
				DebugDraw.draw_mesh(capsule_mesh, sg.transform, Color(1, 0, 1, 1))
				#DebugDraw.draw_mesh(capsule_mesh, Transform3D(Vector3(1, 0, 0),
													#Vector3(0, 1, 0),
													#Vector3(0, 0, 1),
													#Vector3(0, 0, 0)
													#), Color(1, 1, 1, 1))
		else:
			continue
