from cube_simulator import Pixel, rgb, Cube
import random

#Create cube and list of colors
cube = Cube()
colors = [rgb.white,rgb.red,rgb.green,rgb.blue,rgb.yellow,rgb.magenta,rgb.aqua,rgb.salmon,rgb.silver,rgb.lawn_green,rgb.purple,rgb.navy]

#Change a row of leds on each face to a solid color
for face in range(0,6):
	for row in range(0,64):
		select = random.randint(0,len(colors)-1)
		cube.set_row_face_color(row,face,colors[select])

#Update cube simulation with new colors
cube.update()
