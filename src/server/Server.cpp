#include "../../inc/Server.hpp"

Server::Server(int ac, char **av) {
	try {
		ac_check(ac);
		port_check(av);
		port = std::stoi(av[1]);
		password = av[2];
		server_fd = socket(PF_INET, SOCK_STREAM, 0);
		socket_check(server_fd);
		std::cout << "Server with port: " << port << std::endl;
		std::cout << "and password: " << password << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

Server::~Server() {
	close(server_fd);
}

const int	&Server::getPort() const {return (port);}

bool	Server::validPassword(const std::string &pass) const {
	if (pass == password)
		return (true);
	return (false);
}

const std::string	&Server::getServerName() const {return (server_name);}

const char *Server::errorAc::what() const throw() {
	return ("Error: usage ./ircserv <port> <password>");
}

const char *Server::errorPort::what() const throw() {
	return ("Error: value of port is outside of int scope");
}

const char *Server::errorSocket::what() const throw() {
	return ("Error: socket failed, server fd returned: '-1'");
}

void	Server::ac_check(int ac) const {
	if (ac != 3)
		throw errorAc();
}

void	Server::port_check(char **av) const {
	try {
		std::stoi(av[1]);
	} catch (std::exception &e) {
		throw errorPort();
	}
}

void	Server::socket_check(int fd) const {
	if (fd == -1)
		errorSocket();
}
