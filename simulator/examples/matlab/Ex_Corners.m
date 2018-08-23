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

%Led Cube Example: Corners
clc; clear variables;

%Create cube class
cube = Cube_Simulator;

%Update each corner of the cube
indices = [0 383 24192 24575];
for ii = 64:64:383
	indices = [indices ii-1 ii];
	indices = [indices ii+24191 ii+24192];
end
indices = indices+1;

%Change every index chosen to green
select = randi(size(cube.colors,1));
for index = indices
	cube.set_led(index,cube.colors(select,:))
end

%Send cube the new colors
cube.update(0);
