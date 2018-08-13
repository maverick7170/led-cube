from cube_simulator import Pixel, rgb, Cube
import random, math, time

#Create cube and the shades of red, green, blue in the equalizer bar
#Each bar is 5 leds wide and starts out green and then yellow to red
#as the bar gets taller
cube = Cube()
red = [round(ii*7.968) for ii in range(1,33)] + [255]*32
green = [255]*32 + [round(ii*7.968) for ii in range(32,0,-1)]
blue = [0]*64
print(red)
print(green)
print(blue)

#Equalizer bar properties
mag = 40
bar_width = 5
step = 0.2244

#Run animation for 30 seconds
start_time = time.time();
while time.time()-start_time <= 30:
	#Turn all leds back to black
	cube.reset()

	#Using sin waves, create random equalizer bars on each face
	for face in range(1,5):
		baseline = random.randint(0,30)
		bars = [mag*math.sin(ii*step)+baseline+(random.random()-0.5)*20 for ii in range(1,(64//bar_width)+2)]	
		for row in range(63,-1,-1):
			for col in range(0,64):
				bar_index = col//bar_width
				index = row*64*6 + face*64 + col
				#For each bar, draw a black bar between them for clarity
				if col%bar_width == bar_width-1:
					cube.set_led(index,rgb.black)
				else:
					height = 63-row
					if height <= bars[bar_index]:
						cube.set_led(index,Pixel(red[height],green[height],blue[height]))

	#Update cube simulation with image pixels and wait before showing next one	
	cube.update()
	time.sleep(0.1)