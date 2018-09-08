from cube_simulator import Pixel, rgb, Cube
import random, math, time, png
from sys import argv

#Check for simulation vs real cube
cube_type = None
if len(argv) > 1:
	cube_type = argv[1]

#Create cube and a list to hold all 12 images
cube = Cube()
all_pixels = []
image_count = 12

#Load all 12 images into list
for ii in range(1,image_count+1):
	filename = '../resources/mario/mario_' + str(ii) + '.png'
	with open(filename,'rb') as fid:
		fid_png = png.Reader(file=fid)
		raw_data = fid_png.read()
		width = raw_data[0]
		height = raw_data[1]
		planes = raw_data[3]['planes']
		pixels = list(raw_data[2])
		blank_pixels = [[Pixel(255,255,255)]*64 for ii in range(0,64)]
		for row in range(0,min(height,64)):
			for col in range(0,width*planes,planes):
				blank_pixels[row][col//planes] = Pixel(pixels[row][col],pixels[row][col+1],pixels[row][col+2])
		all_pixels.append(blank_pixels)

#Run animation for 30 seconds
ticker = 0
start_time = time.time();
while time.time()-start_time <= 30:
	#Set background color
	cube.set_all_leds(rgb.white)
	#Draw image
	for row in range(0,64):
		for col in range(0,64):
			index = row*64*6 + col + 64
			offset = 0
			if col + ticker%256 > 255:
				offset = -256
			cube.set_led(index+ticker%256+offset,all_pixels[ticker%image_count][row][col])

	#Update cube simulation with image pixels and wait before showing next one	
	cube.update(cube_type)
	ticker += 1
	time.sleep(0.025)
