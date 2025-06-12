#ifndef	SERVER_HPP
#define	SERVER_HPP

#include <iostream>
#include <string>
#include <exception>
/*
	above C++ libraries
	below C style libraries
*/
#include <sys/socket.h>
#include <unistd.h>

class	Server {
	private:
		/*
			arguments needed to run server
			and later on to connect to it
		*/
		int	port;
		std::string password;
		/*
			establishing the base for server
			- return value of the socket
			- name of the server per DNS
		*/
		int	server_fd;
		const std::string	server_name = "nsc-irc.local";
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
		/*
			Getter functions for private vars
			and some of the validations methods
		*/
		const int	&getPort() const;
		bool	validPassword(const std::string &pass) const;
		const std::string	&getServerName() const;
		/*
			basic error classes for checks
			and related member functions
			- argument count
			- validating passed port
			- checking the socket status
		*/
		class	errorAc : public std::exception {
			public:
				const char *what() const throw();
		};
		class	errorPort : public std::exception {
			public:
				const char *what() const throw();
		};
		class	errorSocket : public std::exception {
			public:
				const char *what() const throw();
		};
		void	ac_check(int ac) const;
		void	port_check(char **av) const;
		void	socket_check(int server_fd) const;
};


#endif