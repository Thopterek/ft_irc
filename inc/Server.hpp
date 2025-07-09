#ifndef	SERVER_HPP
#define	SERVER_HPP

#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <iterator>
/*
	above C++ libraries
	below C style libraries
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
/*
	includes of our headers
	for different functionality
*/
#include "Client.hpp"
#include "Parser.hpp"
#include "Channel.hpp"

/*
	handler and actual signal flag
	to shutdown server gracefully
*/
extern volatile std::sig_atomic_t g_shutdown;
void	handler(int signum);

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
		/*
			containers for different functionality
			- keeping track of the poll file descriptors
			- vector to add the new ones in the polling
			- processing all the clients data
		*/
		std::vector<struct pollfd> polling;
		std::vector<struct pollfd> fresh;
		Client	clients;
		Parser	pars;
		// Channel	channels;
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
		void	printDone() const;
		/*
			getting past all of the above points
			setting up for upcomming connection
			and exiting if error gets caught
		*/
		void	setupServer();
		void	cleanExit();
		/*
			error checking for the setup
			- reuse address and disbale Nagle algo
			- checking current flag and set non blocking
			- converting port and assining address
			- making a passive socket for accept
			some more desribing names in fts
		*/
		class	errorOptions : public std::exception {
			public:
				const char *what() const throw();
		};
		class	errorFcntl : public std::exception {
			public:
				const char *what() const throw();
		};
		class	errorBind : public std::exception {
			public:
				const char *what() const throw();
		};
		class	errorListen : public std::exception {
			public:
				const char *what() const throw();
		};
		void	socket_options();
		void	non_blocking();
		void	assign_address();
		void	use_to_connect();
		/*
			typedef to check for proper polling
			without the problem of iterators
		*/
		typedef	std::vector<struct pollfd>::iterator iter;
		/*
			running the actual server
			taking the clients conenctions
			- main caller and the infinite loop
			- accepting the conneciton que to add later
			- removing from the poll invalid fds
			- close socket for already disconnected
			- cleaning the open fds that had errors
			- receving the data from current members
		*/
		void	runServer();
		void	acceptingClient();
		iter	removeInvalid(iter it);
		iter	removeDisconnected(iter it);
		iter	removeError(iter it);
		iter	receivingData(iter it);
		/*
			helper function for output
			- debbuging the runServer / cleaning stage
			- wrapper for sending the msg
			- checking which errno is it
		*/
		void	runError(const std::string &msg, const int &fd) const;
		void	sendMsg(std::string msg, int fd);
		void	recvErrno();
};


#endif