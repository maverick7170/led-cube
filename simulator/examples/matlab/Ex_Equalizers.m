%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% LED Cube for Teaching Introductory Programming Concepts
% Copyright (C) 2018 Ira Hill (ijh3@ufl.edu)
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http:%%www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Led Cube Example: Equalizer
clc; clear variables;

%Create cube class
cube = Cube_Simulator;

% Create vectors to hold equalizer bars
red = zeros(1,64)+255;
green = zeros(1,64)+255;
blue = zeros(1,64);
for ii = 1:1:32
    red(ii) = round(ii*7.968);
    green(ii+32) = round((33-ii)*7.968);
end

% Equalize bar properties
mag = 40;
step = 0.2244;
bar_width = 5;

%Random numbers for bar baseline and shift up/down
% 	std::mt19937 engine(time(0));
% 	std::uniform_int_distribution<int> dist_baseline(0,30);
% 	std::uniform_real_distribution<double> dist_shift(-10,10);
% 
%Loop for 60 seconds, showing mario walking
tic;
while toc <= 30
    %Clear all colors
	cube.reset();
    %Using sin waves, create random equalizer bars on each face
    for face = 0:1:5
        baseline = randi([0 30]);
        bars = [];
        for ii = 1:1:(64/bar_width)+2
            bars(ii) = mag*sin(ii*step)+baseline+randi([-10 10]);
        end
        for row = 63:-1:0
            for col = 0:1:63
                bar_index = floor(col/bar_width)+1;
                index = row*64*6 + face*64 + col + 1;
                %For each bar, draw a black bar between them for clarity
                if mod(col,bar_width) == bar_width-1
                    cube.set_led(index,[0 0 0]);
                else
                    height = 64-row;
                    if height <= bars(bar_index)
                        cube.set_led(index,[red(height) green(height) blue(height)]);
                    end
                end
            end
        end
    end
    %Update cube simulation with image and wait before showing next one
    cube.update(0);
    pause(0.1)
end    
