from cube_simulator import Pixel, rgb, Cube
import random, math, time
cube = Cube()
mag = 40
red = [round(ii*7.968) for ii in range(1,33)] + [255]*32
green = [255]*32 + [round(ii*12.14) for ii in range(32,0,-1)]
blue = [0]*64
bar_width = 5
step = 0.2244

while True:
	cube.reset()
	for face in range(1,5):
		baseline = random.randint(0,30)
		bars = [mag*math.sin(ii*step)+baseline+(random.random()-0.5)*20 for ii in range(1,(64//bar_width)+2)]	
		for row in range(63,-1,-1):
			for col in range(0,64):
				bar_index = col//bar_width
				index = row*64*6 + face*64 + col
				if col%bar_width == bar_width-1:
					cube.set_pixel(index,rgb.black)
				else:
					height = 63-row
					if height <= bars[bar_index]:
						cube.set_pixel(index,Pixel(red[height],green[height],blue[height]))
	cube.update()
	time.sleep(0.1)