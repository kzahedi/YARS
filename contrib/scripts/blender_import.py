import bpy
import mathutils

filename="/Users/zahedi/Desktop/hexabord.log"

# Frame
# hexapod main body
# hexapod rear right femur
# hexapod rear right tibia
# hexapod rear right tarsus
# hexapod centre right femur
# hexapod centre right tibia
# hexapod centre right tarsus
# hexapod front right femur
# hexapod front right tibia
# hexapod front right tarsus
# hexapod rear left femur
# hexapod rear left tibia
# hexapod rear left tarsus
# hexapod centre left femur
# hexapod centre left tibia
# hexapod centre left tarsus
# hexapod front left femur
# hexapod front left tibia
# hexapod front left tarsus

frame_index = 0
fd = open(filename,"r")
lines = fd.readlines()
fd.close()

bpy.context.active_object.animation_data_clear()

for line in lines[1:3000]:
    line=line.rstrip("\n")
    frame_index = frame_index + 1
    values = line.split(",")
    for i in range(0,int(len(values)/7)):
        bpy.context.scene.objects.active = bpy.data.objects[ values[i*7] ]
        ob = bpy.context.active_object
        ob.location = [float(values[i*7+1]), float(values[i*7+2]), float(values[i*7+3])]
        ob.keyframe_insert(data_path='location', index=-1, frame=frame_index)
        ob.rotation_euler = mathutils.Euler( (float(values[i*7+4]), float(values[i*7+5]), float(values[i*7+6])), "XYZ")
        ob.keyframe_insert(data_path='rotation_euler', frame=frame_index)
