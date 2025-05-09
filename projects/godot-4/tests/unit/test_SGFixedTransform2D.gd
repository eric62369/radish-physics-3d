extends "res://addons/gut/test.gd"

func test_deterministic_rotation():
	var t: SGFixedTransform3D

	# Make sure this gives an identity matrix.
	t = SGFixed.transform3d(0, SGFixed.vector3(0, 0, 0))
	assert_eq(t.x.x, 65536)
	assert_eq(t.x.y, 0)
	assert_eq(t.x.z, 0)
	assert_eq(t.y.x, 0)
	assert_eq(t.y.y, 65536)
	assert_eq(t.y.z, 0)
	assert_eq(t.z.x, 0)
	assert_eq(t.z.y, 0)
	assert_eq(t.z.z, 65536)

	t = SGFixedTransform3D.new()
	t.x.x = 65535
	t.x.y = 0
	t.x.z = -366
	t.y.x = 0
	t.y.y = 65535
	t.y.z = 0
	t.z.x = 366
	t.z.y = 0
	t.z.z = 65535
	
	assert_eq(t.get_rotation(), -364)

	var big_t = t.scaled(SGFixed.vector3(13107200, 13107200, 13107200))
	assert_almost_eq(big_t.x.x, 13107200, 2000) # 200 is 13107200 / 65536
	assert_almost_eq(big_t.y.y, 13107200, 2000)
	assert_almost_eq(big_t.z.z, 13107200, 2000)
	var big_t_inv = big_t.affine_inverse()
	var rounda = big_t.xform(big_t_inv.xform(SGFixed.vector3(65536, 65536, 65536)))
	assert_almost_eq(rounda.x, 65536, 2000)
	assert_almost_eq(rounda.y, 65536, 2000)
	assert_almost_eq(rounda.z, 65536, 2000)

	var small_t = t.scaled(SGFixed.vector3(3536, 3536, 3536))
	assert_almost_eq(small_t.x.x, 3536, 10)
	assert_almost_eq(small_t.y.y, 3536, 10)
	assert_almost_eq(small_t.z.z, 3536, 10)
	
	var small_t_inv = small_t.affine_inverse()
	var roundb = small_t.xform(small_t_inv.xform(SGFixed.vector3(65536, 65536, 65536)))
	assert_almost_eq(roundb.x, 65536, 2000)
	assert_almost_eq(roundb.y, 65536, 2000)
	assert_almost_eq(roundb.z, 65536, 2000)
	
	small_t = t.scaled(SGFixed.vector3(3136, 3136, 3136))
	assert_almost_eq(small_t.x.x, 3136, 10)
	assert_almost_eq(small_t.y.y, 3136, 10)
	assert_almost_eq(small_t.z.z, 3136, 10)
	
	small_t_inv = small_t.affine_inverse()
	roundb = small_t.xform(small_t_inv.xform(SGFixed.vector3(65536, 65536, 65536)))
	assert_almost_eq(roundb.x, 65536, 2000)
	assert_almost_eq(roundb.y, 65536, 2000)
	assert_almost_eq(roundb.z, 65536, 2000)
	
	#small_t = t.scaled(SGFixed.vector3(131, 131, 131)) # TODO: this doesn't pass, is it a minor or major precision issue?
	#assert_almost_eq(small_t.x.x, 131, 10)
	#assert_almost_eq(small_t.y.y, 131, 10)
	#assert_almost_eq(small_t.z.z, 131, 10)
	
	small_t_inv = small_t.affine_inverse()
	roundb = small_t.xform(small_t_inv.xform(SGFixed.vector3(65536, 65536, 65536)))
	assert_almost_eq(roundb.x, 65536, 2000)
	assert_almost_eq(roundb.y, 65536, 2000)
	assert_almost_eq(roundb.z, 65536, 2000)

	t = SGFixedTransform3D.new()
	t.x.x = 65348
	t.x.z = -4956
	t.z.x = 4956
	t.z.z = 65348
	assert_eq(t.get_rotation(), -4937)

	t = SGFixed.transform3d(-290, SGFixed.vector3(0, 0, 0))
	assert_eq(t.x.x, 65535)
	assert_eq(t.x.y, 0)
	assert_eq(t.x.z, -290)
	assert_eq(t.y.x, 0)
	assert_eq(t.y.y, 65536)
	assert_eq(t.y.z, 0)
	assert_eq(t.z.x, 290)
	assert_eq(t.z.y, 0)
	assert_eq(t.z.z, 65535)

	# Make sure this gives the same result on all platforms, even after
	# rotating a lot of times and accumulating a lot of error.
	t = SGFixed.transform3d(0, SGFixed.vector3(0, 0, 0))
	for i in range(320):
		t = t.rotated(SGFixed.PI_DIV_4)
	assert_eq(t.x.x, 65536)
	assert_eq(t.x.y, 0)
	assert_eq(t.x.z, 0)
	assert_eq(t.y.x, 0)
	assert_eq(t.y.y, 65536)
	assert_eq(t.y.z, 0)
	assert_eq(t.z.x, 0)
	assert_eq(t.z.y, 0)
	assert_eq(t.z.z, 65536)
	assert_eq(t.get_rotation(), 0)
	var scale = t.get_scale()
	assert_eq(scale.x, 65536)
	assert_eq(scale.y, 65536)
	assert_eq(scale.z, 65536)
