from cube_simulator import Pixel, rgb, Cube
import random, math, time
import subprocess as sp
from sys import argv

#Check for simulation vs real cube
cube_type = None
if len(argv) > 1:
	cube_type = argv[1]

#Create cube and list of colors
cube = Cube()

#Comminucate with ffmpeg to process the video through python pipes
FFMPEG_BIN='ffmpeg'
command = [ FFMPEG_BIN,'-i','../resources/movies/BigBuckBunny_320x180.mp4','-f', 'image2pipe','-pix_fmt', 'rgb24','-vf','scale=106:60','-vcodec','rawvideo', '-']
pipe = sp.Popen(command, stdout = sp.PIPE, bufsize=10**8)

#Movie properties, will need to change for a different video
mov_depth = 3
mov_width = 106
mov_height = 60

#Run through a specfic number of frames in movie, adjust as needed
for jj in range(0,10000):
	#Grab pixels from ffmpeg pipe
	pixels = [ii for ii in pipe.stdout.read(mov_width*mov_height*mov_depth)]
	
	#Clear cube
	cube.reset()

	#Copy pixels to cube
	for row in range(0,mov_height):
		for col in range(0,mov_width*mov_depth,mov_depth):
			index = row*64*6 + col//mov_depth + 64
			pix = Pixel(pixels[row*mov_width*mov_depth+col],pixels[row*mov_width*mov_depth+col+1],pixels[row*mov_width*mov_depth+col+2])
			cube.set_led(index,pix)

	#Update cube simulation with image pixels and wait before showing next one	
	cube.update(cube_type)
	time.sleep(0.01)

#Done with pipe
pipe.terminate()
