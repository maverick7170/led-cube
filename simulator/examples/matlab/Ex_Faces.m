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

%Led Cube Example: Faces
clc; clear variables;

%Create cube class
cube = Cube_Simulator; 

%Update each face of the cube
for ii = 1:1:6
    select = randi(size(cube.colors,1));
    cube.set_face_color(ii,cube.colors(select,:));
end

%Send cube the new colors
cube.update(0);
