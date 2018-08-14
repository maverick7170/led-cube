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

#Create a list of the 4 corners on every face
indices = [0,383,24192,24575]
for ii in range(64,384,64):
	indices += [ii-1,ii]
	indices += [ii+24191,ii+24192]

#Set each corner to a random color
for index in indices:
	select = random.randint(0,len(colors)-1)
	cube.set_led(index,colors[select])

#Update cube simulation with new colors
cube.update(cube_type)