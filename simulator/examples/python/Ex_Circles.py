from cube_simulator import Pixel, rgb, Cube
import random
from sys import argv

#Check for simulation vs real cube
cube_type = None
if len(argv) > 1:
	cube_type = argv[1]

#Create cube and list of colors
cube = Cube()
colors = [rgb.white,rgb.red,rgb.green,rgb.blue,rgb.yellow,rgb.magenta,rgb.aqua,rgb.salmon,rgb.silver,rgb.lawn_green,rgb.purple,rgb.navy]

#Select different size radii and color for each circle 
circle_radi = [random.randint(5,31) for ii in range(0,6)]
circle_color = [random.randint(0,len(colors)-1) for ii in range(0,len(colors)-1)]

#Only change leds that fit within the circle radius on each face
for face in range(0,6):
	for row in range(0,64):
		for col in range(0,64):
			if (row-31.5)**2 + (col-31.5)**2 <= circle_radi[face]**2:
				index = row*64*6 + face*64 + col
				cube.set_led(index,colors[circle_color[face]])

#Update cube simulation with new colors
cube.update(cube_type)