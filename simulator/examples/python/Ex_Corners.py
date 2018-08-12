from cube_simulator import Pixel, rgb, Cube
cube = Cube()
indices = [0,383,24192,24575]
for ii in range(64,384,64):
	indices += [ii-1,ii]
	indices += [ii+24191,ii+24192]
print(indices)
for index in indices:
	cube.set_pixel(index,rgb.green)
cube.update()