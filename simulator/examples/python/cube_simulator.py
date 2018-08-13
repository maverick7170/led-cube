import sys
import socket
import time
import random

class Pixel:
	def __init__(self,r,g,b):
		if r < 0: r = 0
		if r > 255: r = 255
		if g < 0: g = 0
		if g > 255: g = 255
		if b < 0: b = 0
		if b > 0: b = 255
		self.r = r
		self.g = g
		self.b = b

class RGB:
	def __init__(self):
		self.black = Pixel(0,0,0)
		self.white = Pixel(255,255,255)
		self.red = Pixel(255,0,0)
		self.green = Pixel(0,255,0)
		self.blue = Pixel(0,0,255)
		self.yellow = Pixel(255,255,0)
		self.magenta = Pixel(255,0,255)
		self.aqua = Pixel(0,255,255)
		self.salmon = Pixel(250,128,114)
		self.silver = Pixel(192,192,192)
		self.lawn_green = Pixel(124,252,0)
		self.purple = Pixel(128,0,128)
		self.navy = Pixel(0,0,128)

rgb = RGB()

class Cube:
	"""Interactive control of led cube"""
	def __init__(self):
		self.leds = [0 for ii in range(0,24576*3)]
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		
	def reset(self):
		for ii in range(0,24576*3):
			self.leds[ii] = 0

	def set_all_leds(self,pixel):
		for ii in range(0,24576*3,3):
			self.leds[ii:ii+3] = (pixel.r,pixel.g,pixel.b)

	def in_range(self,v,low,high):
		if v < low or v > high:
			raise RuntimeError("Something bad happened")

	def set_face_color(self,face,pixel):
		self.in_range(face,0,5)
		for row in range(0,64):
			for col in range(0,64):
				index = face*64*3 + row*64*6*3 + col*3
				self.leds[index:index+3] = (pixel.r,pixel.g,pixel.b)

	def set_row_face_color(self,row,face,pixel):
		self.in_range(row,0,63)
		self.in_range(face,0,5)
		for col in range(0,64):
			index = row*64*6*3 + face*64*3 + col*3
			self.leds[index:index+3] = (pixel.r,pixel.g,pixel.b)

	def set_col_face_color(self,col,face,pixel):
		self.in_range(col,0,63)
		self.in_range(face,0,5)
		for row in range(0,64):
			index = row*64*6*3 + face*64*3 + col*3
			self.leds[index:index+3] = (pixel.r,pixel.g,pixel.b)		

	def set_led(self,index,pixel):
		self.in_range(index,0,24575)
		self.leds[index*3:index*3+3] = (pixel.r,pixel.g,pixel.b)

	def update_raw(self,ip,port,delay):
		length = 1159
		offset = 0
		for row in range(0,64):
			flag = 1
			if row == 63: flag = 0
			msg = bytes([128,length&0xff,(length>>8)&0xff,row,offset&0xff,(offset>>8)&0xff,flag])
			sent = self.sock.sendto(msg+bytes(self.leds[(row*64*6*3):((row+1)*64*6*3)]),(ip,port))
			if row%3 == 0:
				time.sleep(delay)
		
	def update(self,flag=None):
		if flag is None:
			self.update_raw('127.0.0.1',8080,0.001)
		else:
			self.update_raw('10.0.0.1',8080,0.001)

def TabCompleteNames(line):
	period_index = None
	start_index = 0
	for ii,let in enumerate(line[-1::-1]):
		if let == '.':
			period_index = ii
		elif let == ' ' or let == '(':
			start_index = ii
			break
	raw_cmd = line[-start_index:]
	if period_index is not None:
		var_name = raw_cmd[0:-period_index-1]
		if period_index == 0:
			var_member = ''
		else:
			var_member = line[-period_index:]
	else:
		var_name = None
		var_member = None
	return (raw_cmd,var_name,var_member)

BUILT_IN_COMMANDS = { 'a':['abs(','all(','and','any(','as','ascii(','assert','async','await'],
'b':['bin(','bool(','break','breakpoint(','bytearray(','bytes('],
'c':['callable(','chr(','class','classmethod(','compile(','complex(','continue','copyright(','credits('],
'd':['def','del','delattr(','dict(','dir(','divmod('],
'e':['elif','else','enumerate(','eval(','except(','exec(','exit('],
'f':['filter(','finally:','float(','for','format(','from','frozenset('],
'g':['getattr(','global','globals('],
'h':['hasattr(','hash(','help(','hex('],
'i':['id(','import','input(','is','issubclass(','if','in','int(','isinstance(','iter('],
'j':[],
'k':[], 
'l':['lambda','len(','license(','list(','locals('],
'm':['map(','max(','memoryview(','min('],
'n':['next(','nonlocal','not'],
'o':['object(','oct(','open(','or','ord('],
'p':['pass','pow(','print(','property('],
'q':['quit('],
'r':['raise','range(','repr','return','reversed(','round('],
's':['set(','setattr(','slice(','sorted(','staticmethod(','str(','sum(','super('],
't':['try:','tuple(','type('],
'u':[],
'v':['vars('],
'w':['while','with'],
'x':[],
'y':['yield'],
'z':['zip('] }

if __name__ == "__main__":
	cube_ = Cube()
	cube_.reset()
	cube_.set_face_color(1,RGB.red)
	#colors = [[255,0,0],[0,255,0],[0,0,255],[255,255,0],[255,0,255],[255,192,208]]
	#for face in range(0,6):
	#	select = random.randint(0,5)
	#	cube_.set_face_color(face,colors[select][0],colors[select][1],colors[select][2])
	cube_.update('real')	
