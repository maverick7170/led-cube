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

%Led Cube Example: Circles
clc; clear variables;

%Create cube class
cube = Cube_Simulator;

%Update each face of the cube
circle_radi = randi([5,31],1,6);
circle_color = randi([1 size(cube.colors,1)],1,size(cube.colors,1));
for face = 1:1:6
    for row = 1:1:64
        for col = 1:1:64
            if (row-32.5)^2 + (col-32.5)^2 <= circle_radi(face)^2
                index = (row-1)*64*6 + (face-1)*64 + col;
                cube.set_led(index,cube.colors(circle_color(face),:));
            end
        end
    end
end

%Send cube the new colors
cube.update(0);
