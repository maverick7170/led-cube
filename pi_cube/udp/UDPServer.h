////////////////////////////////////////////////////////////
////
//// LED Cube for Teaching Introductory Programming Concepts
//// Copyright (C) 2018 Ira Hill (ijh3@ufl.edu)
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////
#ifndef UDPSERVER_H
#define UDPSERVER_H 

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

class UDPServer
{
	public:
		UDPServer(int port) {
            socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (socket_fd < 0) { perror("socket creation failed"); }
            bzero(&server_addr,sizeof(server_addr));
            server_addr.sin_family    = AF_INET; // IPv4
            server_addr.sin_addr.s_addr = INADDR_ANY;
            server_addr.sin_port = htons(port);
            // Bind the socket with the server address
            if ( ::bind(socket_fd, (const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0 )
            {
                perror("bind failed");
                //exit(EXIT_FAILURE);
            }
        }
        int receive_(std::vector<uint8_t> &msg, std::string &sender, int &port) {
            msg.clear();
	    sender.resize(INET_ADDRSTRLEN);
	    port = -1;
            int n = recvfrom(socket_fd, (char *)buffer, 2048, MSG_DONTWAIT, (struct sockaddr*)&client_addr,&client_len);
            if (n > 0) {
	    	for (int ii = 0; ii < n; ++ii) { msg.push_back(buffer[ii]); }
		//char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
		//getnameinfo((struct sockaddr*)&client_addr, client_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
		//char *sender_ = inet_ntoa(client_addr.sin_addr);
		//sender = string(sender_);
		//std::cout << hbuf << " and " << sbuf << std::endl;
		//port = ntohs(client_addr.sin_port);
	    }
	    return n;
        }
        int send_(std::vector<uint8_t> msg, std::string host, int port) {
            struct sockaddr_in remote;
            bzero(&remote,sizeof(remote));
            remote.sin_family = AF_INET;
            remote.sin_port = htons(port);
            remote.sin_addr.s_addr = INADDR_ANY;
            inet_pton(AF_INET, host.c_str(), &(remote.sin_addr));
            return sendto(socket_fd, (const char *)msg.data(), msg.size(),0, (const struct sockaddr *) &remote,sizeof(remote));
        }
	private:
		int socket_fd;
        socklen_t client_len;
        struct sockaddr_in server_addr, client_addr;
        char buffer[2048];
};

#endif

