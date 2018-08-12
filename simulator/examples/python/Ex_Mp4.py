from cube_simulator import Pixel, rgb, Cube
import random, math, time
import subprocess as sp

cube = Cube()
FFMPEG_BIN='ffmpeg'

#command = [ FFMPEG_BIN, '-ss', '00:00:5','-i','movies/BigBuckBunny_320x180.mp4','-ss','1','-f', 'image2pipe','-pix_fmt', 'rgb24','-vf','scale=106:60','-vcodec','rawvideo', '-']
command = [ FFMPEG_BIN,'-i','../resources/movies/BigBuckBunny_320x180.mp4','-f', 'image2pipe','-pix_fmt', 'rgb24','-vf','scale=106:60','-vcodec','rawvideo', '-']
#command = [ FFMPEG_BIN,'-s:v','106x60','-r','25','-i','movies/BigBuckBunny_S.yuv','-f', 'image2pipe','-pix_fmt', 'rgb24','-vcodec','rawvideo', '-']
pipe = sp.Popen(command, stdout = sp.PIPE, bufsize=10**8)
mov_depth = 3
mov_width = 106
mov_height = 60
for jj in range(0,10000):
	pixels = [ii for ii in pipe.stdout.read(mov_width*mov_height*mov_depth)]
	cube.leds = [0 for ii in range(0,24576*3)]
	for row in range(0,mov_height):
		for col in range(0,mov_width*mov_depth,mov_depth):
			index = row*64*6 + col//mov_depth + 64
			pix = Pixel(pixels[row*mov_width*mov_depth+col],pixels[row*mov_width*mov_depth+col+1],pixels[row*mov_width*mov_depth+col+2])
			cube.set_pixel(index,pix)
	cube.update()
	time.sleep(0.01)
	#break
pipe.terminate()
