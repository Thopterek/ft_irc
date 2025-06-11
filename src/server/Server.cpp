#include "../../inc/Server.hpp"

Server::Server(int ac, char **av) {
	ac_check(ac);
	port_check(av);
	port = std::stoi(av[1]);
	password = av[2];
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	socket_check(server_fd);
	std::cout << "Server with port: " << port << std::endl;
	std::cout << "and password: " << password << std::endl;
}

Server::~Server() {}
