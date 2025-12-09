#@tool
#
#extends Node3D
#
#
## 3D
#
#var _boxes := []
#var _box_pool := []
#var _box_mesh : Mesh = null
#var _line_material_pool := []
#
#var _lines := []
#var _line_immediate_mesh : ImmediateMesh
#
#var _mesh_instances := []
#var _mesh_instance_pool := []
#var _mesh_material_pool := []
#
## Called when the node enters the scene tree for the first time.
#func _ready():
	#_boxes = []
	#_box_pool = []
	#_box_mesh = null
	#_line_material_pool = []
#
	#_lines = []
	#_line_immediate_mesh = null
#
	#_mesh_instances = []
	#_mesh_instance_pool = []
	#_mesh_material_pool = []
	#pass # hit the project reload to update this
#
#func _process(delta):
	#if Engine.is_editor_hint():
		#for child in get_children():
			#remove_child(child) #remove old meshes
			#
		#for child in get_all_children(get_parent()):
			#if child is SGArea3D:
				#var sg = child
				#var capsule_mesh = CapsuleMesh.new()
				#var shape = sg.get_child(0)
				#if (shape is SGCollisionShape3D):
					#shape = shape.shape
					## (Optional) Customize the CapsuleMesh properties
					#capsule_mesh.radius = SGFixed.to_float(shape.radius)
					#if (shape is SGCapsuleShape3D):
						#capsule_mesh.height = SGFixed.to_float(shape.height + shape.radius*2)
					#else:
						#capsule_mesh.height = SGFixed.to_float(shape.radius)*2
					#capsule_mesh.radial_segments = 18
					#capsule_mesh.rings = 6
					#
					#var color = Color(1, 0, 0, 1)
					##print("WOW")
					#draw_mesh(capsule_mesh, sg.transform, color)
			#else:
				#continue
	#
#
## This function is recursive: it calls itself to get lower levels of child nodes as needed.
## `children_acc` is the accumulator parameter that allows this function to work.
## It should be left to its default value when you call this function directly.
#func get_all_children(in_node, children_acc = []):
	#children_acc.push_back(in_node)
	#for child in in_node.get_children():
		#children_acc = get_all_children(child, children_acc)
#
	#return children_acc
	#
#
#func draw_mesh(mesh: Mesh, transform_: Transform3D, color := Color.WHITE):
	#var mi := _get_mesh_instance()
	#var mat : Material
	#var uses_lines = false
	#if mesh is ArrayMesh:
		#var pt : int = mesh.surface_get_primitive_type(0)
		#if pt == Mesh.PRIMITIVE_LINES or pt == Mesh.PRIMITIVE_LINE_STRIP or \
		#pt == Mesh.PRIMITIVE_POINTS:
			#mat = _get_line_material()
			#uses_lines = true
		#else:
			#mat = _get_mesh_material()
	#else:
		#mat = _get_mesh_material()
	#mat.albedo_color = color
	#mi.material_override = mat
	#mi.transform = transform_
	#mi.mesh = mesh
	#_mesh_instances.append({
		#"node": mi,
		#"uses_lines": uses_lines,
		##"frame": _frame_counter + LINES_LINGER_FRAMES
	#})
#
#
#func _get_mesh_instance() -> MeshInstance3D:
	#var mi : MeshInstance3D
	#if len(_mesh_instance_pool) == 0:
		#mi = MeshInstance3D.new()
		#add_child(mi)
	#else:
		#mi = _mesh_instance_pool[-1]
		#_mesh_instance_pool.pop_back()
	#return mi
#
#func _get_mesh_material() -> StandardMaterial3D:
	#var mat : StandardMaterial3D
	#if len(_mesh_material_pool) == 0:
		#mat = StandardMaterial3D.new()
	#else:
		#mat = _mesh_material_pool[-1]
		#_mesh_material_pool.pop_back()
	#return mat
#
#func _get_line_material() -> StandardMaterial3D:
	#var mat : StandardMaterial3D
	#if len(_line_material_pool) == 0:
		#mat = StandardMaterial3D.new()
		#mat.flags_unshaded = true
		#mat.vertex_color_use_as_albedo = true
	#else:
		#mat = _line_material_pool[-1]
		#_line_material_pool.pop_back()
	#return mat
