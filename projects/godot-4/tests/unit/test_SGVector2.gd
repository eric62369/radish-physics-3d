extends "res://addons/gut/test.gd"

func test_normalize():
	var v: SGFixedVector3
	
	# Normal kind of vector
	v = SGFixed.vector3(65536, 65536, 65536).normalized()
	assert_eq(v.x, 46341)
	assert_eq(v.y, 46341)
	assert_eq(v.z, 46341)
	assert_eq(v.length(), 65536)
	
	# Super tiny X.
	v = SGFixed.vector3(1, 0, 0).normalized()
	assert_eq(v.x, 65536)
	assert_eq(v.y, 0)
	assert_eq(v.z, 0)
	assert_eq(v.length(), 65536)
	
	# Super tiny Y.
	v = SGFixed.vector3(256, 0, 0).normalized()
	assert_eq(v.x, 65536)
	assert_eq(v.y, 0)
	assert_eq(v.z, 0)
	assert_eq(v.length(), 65536)

	v = SGFixed.vector3(0, 0, 256).normalized()
	assert_eq(v.x, 0)
	assert_eq(v.y, 0)
	assert_eq(v.z, 65536)
	assert_eq(v.length(), 65536)
	
	# Super tiny X, with super huge Y.
	v = SGFixed.vector3(255, 45000000, 255).normalized()
	assert_eq(v.x, 0)
	assert_eq(v.y, 65536)
	assert_eq(v.z, 0)

	# Super tiny Y, with super huge X.
	v = SGFixed.vector3(45000000, 255, 255).normalized()
	assert_eq(v.x, 65536)
	assert_eq(v.y, 0)
	assert_eq(v.z, 0)
	
	# Super tiny XY, with super huge Z.
	v = SGFixed.vector3(255, 255, 45000000).normalized()
	assert_eq(v.x, 0)
	assert_eq(v.y, 0)
	assert_eq(v.z, 65536)

	# Super tiny X, with Y just on the edge of being super huge.
	v = SGFixed.vector3(255, 1482909, 255).normalized()
	assert_gt(v.y, 0, "Y axis of the vector flipped")
	assert_gt(v.x, 0, "X axis of the vector flipped")
	assert_gt(v.z, 0, "Z axis of the vector flipped")
	assert_eq(v.x, 11)
	assert_eq(v.y, 65535)
	assert_eq(v.z, 11)
	assert_eq(v.length(), 65535)
	
	# Smallish values that (due to imprecision) resist normalization.
	v = SGFixed.vector3(334, -667, 334).normalized()
	assert_true(v.is_normalized())
