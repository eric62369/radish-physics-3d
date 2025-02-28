#######
# NOTE: This is used for building the Godot 3 module!
#       It has to be at the top-level for technical reasons.
#######

def can_build(env, platform):
    return True

def configure(env):
    pass

def get_doc_path():
    return "godot-4/doc_classes"

def get_doc_classes():
    return [
        'SGArea3D',
        'SGAreaCollision3D',
        'SGAStar3D',
        'SGCapsuleShape3D',
        'SGCircleShape3D',
        'SGCollisionObject3D',
        'SGCollisionPolygon3D',
        'SGCollisionShape3D',
        'SGCurve3D',
        'SGFixed',
        'SGFixedNode3D',
        'SGFixedPosition3D',
        'SGFixedRect3',
        'SGFixedTransform3D',
        'SGFixedVector3',
        'SGKinematicBody3D',
        'SGKinematicCollision3D',
        'SGPath3D',
        'SGPathFollow3D',
        'SGPhysics3DServer',
        'SGRayCast3D',
        'SGRectangleShape3D',
        'SGShape3D',
        'SGStaticBody3D',
        'SGTween',
        'SGYSort',
    ]
