from cube_simulator import Pixel, rgb, Cube
import random
cube = Cube()
colors = [rgb.white,rgb.red,rgb.green,rgb.blue,rgb.yellow,rgb.magenta,rgb.aqua,rgb.salmon,rgb.silver,rgb.lawn_green,rgb.purple,rgb.navy]
for face in range(0,6):
	for col in range(0,64):
		select = random.randint(0,len(colors)-1)
		cube.set_col_face_color(col,face,colors[select])
cube.update()
