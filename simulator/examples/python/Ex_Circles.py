from cube_simulator import Pixel, rgb, Cube
import random
colors = [rgb.white,rgb.red,rgb.green,rgb.blue,rgb.yellow,rgb.magenta,rgb.aqua,rgb.salmon,rgb.silver,rgb.lawn_green,rgb.purple,rgb.navy]
cube = Cube()
circle_radi = [random.randint(5,31) for ii in range(0,6)]
circle_color = [random.randint(0,len(colors)-1) for ii in range(0,len(colors)-1)]

for face in range(0,6):
	for row in range(0,64):
		for col in range(0,64):
			if (row-31.5)**2 + (col-31.5)**2 <= circle_radi[face]**2:
				index = row*64*6 + face*64 + col
				cube.set_pixel(index,colors[circle_color[face]])
cube.update()