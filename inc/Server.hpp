#ifndef	SERVER_HPP
#define	SERVER_HPP

#include <sys/socket.h>
#include <iostream>
#include <string>

class	Server {
	private:
		int	port;
		std::string password;
		/*
			basic information needed for connection
			and stored data per the needed funcitons
		*/
		int	server_fd;
	public:
		Server() = delete;
		Server(const Server &other) = delete;
		Server	&operator=(const Server &other) = delete;
		/*
			useless OCF above per requirments
			below the basics for the server side
		*/
		Server(int ac, char **av);
		~Server();
};

/*
	functions related to error_server file
	all the possible checks for server fts
	their return values and passed arguments
*/
void	ac_check(int ac);
void	port_check(char **av);
void	socket_check(int server_fd);

#endif