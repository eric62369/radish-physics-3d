extends "res://addons/gut/test.gd"

func test_initial_scale():
	var n: SGFixedNode3D
	
	# Ensure that the initial scale and transform match expectations.
	n = SGFixedNode3D.new()
	assert_eq(n.fixed_scale.x, 65536)
	assert_eq(n.fixed_scale.y, 65536)
	assert_eq(n.fixed_scale.z, 65536)
	assert_eq(n.fixed_transform.x.x, 65536)
	assert_eq(n.fixed_transform.x.y, 0)
	assert_eq(n.fixed_transform.x.z, 0)
	assert_eq(n.fixed_transform.y.x, 0)
	assert_eq(n.fixed_transform.y.y, 65536)
	assert_eq(n.fixed_transform.y.z, 0)
	assert_eq(n.fixed_transform.z.x, 0)
	assert_eq(n.fixed_transform.z.y, 0)
	assert_eq(n.fixed_transform.z.z, 65536)

	# Ensure that setting the scale to the default value won't change those values.
	n.fixed_scale = SGFixed.vector3(65536, 65536, 65536)
	assert_eq(n.fixed_scale.x, 65536)
	assert_eq(n.fixed_scale.y, 65536)
	assert_eq(n.fixed_scale.z, 65536)
	assert_eq(n.fixed_transform.x.x, 65536)
	assert_eq(n.fixed_transform.x.y, 0)
	assert_eq(n.fixed_transform.x.z, 0)
	assert_eq(n.fixed_transform.y.x, 0)
	assert_eq(n.fixed_transform.y.y, 65536)
	assert_eq(n.fixed_transform.y.z, 0)
	assert_eq(n.fixed_transform.z.x, 0)
	assert_eq(n.fixed_transform.z.y, 0)
	assert_eq(n.fixed_transform.z.z, 65536)
