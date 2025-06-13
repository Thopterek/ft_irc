#include "../../inc/Server.hpp"

Server::Server(int ac, char **av) : server_fd(-1) {
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
	if (server_fd >= 0) {
		close(server_fd);
		server_fd = -1;
	}
}

const int	&Server::getPort() const {return (port);}

bool	Server::validPassword(const std::string &pass) const {
	if (pass == password)
		return (true);
	return (false);
}

const std::string	&Server::getServerName() const {return (server_name);}

const char *Server::errorAc::what() const throw() {
	return ("Error: 'constructor' usage ./ircserv <port> <password>");
}

const char *Server::errorPort::what() const throw() {
	return ("Error: 'constructor' port is outside of scope '1 <= 65535'");
}

const char *Server::errorSocket::what() const throw() {
	return ("Error: 'constructor' socket failed, server fd returned: '-1'");
}

void	Server::ac_check(int ac) const {
	if (ac != 3)
		throw errorAc();
}

void	Server::port_check(char **av) const {
	try {
		int tmp = std::stoi(av[1]);
		if (tmp <= 0 || tmp > 65535)
			throw errorPort();
	} catch (std::exception &e) {
		throw errorPort();
	}
}

void	Server::socket_check(int fd) const {
	if (fd == -1)
		throw errorSocket();
}

/*
	second part of getting through before running the server
	setting up the non blocking and other basic functionality
*/
void	Server::setupServer() {
	try {
		socket_options();
		non_blocking();
		assign_address();
		use_to_connect();
		std::cout << "Server setup went through correctly" << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		cleanExit();
	}
}

void	Server::cleanExit() {
	if (server_fd >= 0) {
		close(server_fd);
		server_fd = -1;
	}
	exit(EXIT_FAILURE);
}

const char *Server::errorOptions::what() const throw() {
	return ("Error: 'setup' setsockopt(2) failed, rtfm");
}

const char *Server::errorFcntl::what() const throw() {
	return ("Error: 'setup' fcntl(2) failed, rtfm");
}

const char *Server::errorBind::what() const throw() {
	return ("Error: 'setup' bind(2) failed, rtfm");
}

const char *Server::errorListen::what() const throw() {
	return ("Error: 'setup' listen(2) failed, rtfm");
}

void	Server::socket_options() {
	int tmp = 1;
	int check = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
	check = setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
}

void	Server::non_blocking() {
	int	flag = fcntl(server_fd, F_GETFL, 0);
	if (flag == -1)
		throw errorFcntl();
	flag = fcntl(server_fd, F_SETFL, flag | O_NONBLOCK);
	if (flag == -1)
		throw errorFcntl();
}

void	Server::assign_address() {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(port));
	int check = bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
	if (check == -1)
		throw errorBind();
}

void	Server::use_to_connect() {
	int check = listen(server_fd, SOMAXCONN);
	if (check == -1)
		throw errorListen();
}