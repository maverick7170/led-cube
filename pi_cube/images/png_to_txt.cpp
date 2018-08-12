#include <iostream>
#include <cmath>
#include <png.hpp>
#include <fstream>
#include <string>

using namespace std;
using namespace png;

int main(int argc, char* argv[])
{
	if (argc == 1) {
		cout << "Please list corresponding panels for image" << endl;
		return 0;
	}
	string panels;
	for (int ii = 1; ii < argc; ++ii) {
		panels += string(argv[ii]) + " ";
	}
	string filename;
	cout << "Enter filaname to process: ";
	cin >> filename;
	ofstream fid(filename.substr(0,filename.size()-3)+"txt");
	image<rgb_pixel> pixels(filename);
	auto height = pixels.get_height();
	auto width = pixels.get_width();
	fid << "Width: " << width << endl;
	fid << "Height: " << height << endl;
	fid << "Panels: " << panels << endl;
	for (auto r = 0; r < height; ++r) {
		for (auto c = 0; c < width; ++c) {
			rgb_pixel pix = pixels[r][c];
			uint32_t color = 0;
			color |= (pix.blue << 16);
			color |= (pix.green << 8);
			color |= (pix.red);
			fid << color << " ";
		}
		fid << endl;
	}
	fid.close();
	return 0;
}
