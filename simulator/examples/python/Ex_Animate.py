from cube_simulator import Pixel, rgb, Cube
import random, math, time, png
cube = Cube()
all_pixels = []
image_count = 12
for ii in range(1,image_count+1):
	filename = '../resources/mario/mario_' + str(ii) + '.png'
	with open(filename,'rb') as fid:
		fid_png = png.Reader(file=fid)
		raw_data = fid_png.read()
		width = raw_data[0]
		height = raw_data[1]
		planes = raw_data[3]['planes']
		pixels = list(raw_data[2])
		blank_pixels = [[Pixel(0,0,0)]*64 for ii in range(0,64)]
		for row in range(0,min(height,64)):
			for col in range(0,width*planes,planes):
				#print(pixels[row][col+1])
				blank_pixels[row][col//planes] = Pixel(pixels[row][col],pixels[row][col+1],pixels[row][col+2])
		all_pixels.append(blank_pixels)

ticker = 0
while True:
	cube.leds = [255 for ii in range(0,24576*3)]
	for row in range(0,64):
		for col in range(0,64):
			index = row*64*6 + col + 64
			offset = 0
			if col + ticker%256 > 255:
				offset = -256
			#print('Index: {} | Ticker: {} | Row: {} | Col: {}'.format(index+ticker%256+offset,ticker,row,col))
			cube.set_pixel(index+ticker%256+offset,all_pixels[ticker%image_count][row][col])
	cube.update()
	ticker += 1
	time.sleep(0.025)
