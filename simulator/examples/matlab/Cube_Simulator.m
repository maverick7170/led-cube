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
classdef Cube_Simulator < handle
    properties
        leds
        colors
    end
    methods
        function obj = Cube_Simulator()
            obj.leds = uint8(zeros(1,24576*3));
            if libisloaded('MATLAB_udp_socket')
                unloadlibrary('MATLAB_udp_socket')
            end
			if ismac
				loadlibrary('MATLAB_udp_socket.dylib','MATLAB_udp_socket.h')
			elseif isunix
				loadlibrary('MATLAB_udp_socket.so','MATLAB_udp_socket.h')
			elseif ispc
				loadlibrary('MATLAB_udp_socket.dll','MATLAB_udp_socket.h')
			end
            if calllib('MATLAB_udp_socket','sf_bind',9090) ~= 0
                error('Unable to create udp socket for communicating with cube')
            end
            obj.colors = [0,0,0; 255,255,255; 255,0,0; 0,255,0; 0,0,255;
                255,255,0; 255,0,255; 0,255,255; 250,128,114;
                192,192,192; 124,252,0; 128,0,128; 0,0,128];
        end
        function [] = reset(obj)
            obj.leds = zeros(1,24576*3);
        end
        function [] = set_all_leds(obj,pixel)
            obj.leds = repmat(uint8(pixel(1:1:3)),1,24756);
        end
        function [] = set_face_color(obj,face,pixel)
            for row = 0:63
                for col = 0:63
                    index = (face-1)*64*3 + row*64*6*3 + col*3 + 1;
                    obj.leds(index:index+2) = uint8(pixel(1,1:3));
                end
            end
        end
        function [] = set_row_face_color(obj,row,face,pixel)
            for col = 0:63
                index = (row-1)*64*6*3 + (face-1)*64*3 + col*3 + 1;
                obj.leds(index:index+2) = uint8(pixel(1,1:3));
            end
        end
        function [] = set_col_face_color(obj,col,face,pixel)
            for row = 0:63
                index = row*64*6*3 + (face-1)*64*3 + (col-1)*3 + 1;
                obj.leds(index:index+2) = uint8(pixel(1,1:3));
            end
        end
        function [] = set_led(obj,index,pixel)
            obj.leds( [ ((index-1)*3+1):((index-1)*3+3)] ) = uint8(pixel(1,1:3));
        end
        function [sent] = update(obj,type)
            sent = calllib('MATLAB_udp_socket','sf_update',obj.leds,type);
            if sent ~= 64
                error('Unable to send udp data to cube')
            end
        end
    end
end