#include "../../inc/Server.hpp"

static void	error_exit(std::string msg) {
	std::cerr << "Error: " << msg << std::endl;
	exit(1);
}

void	ac_check(int ac) {
	if (ac != 3)
		error_exit("usage ./ircserv <port> <password>");
}

void	port_check(char **av) {
	try {
		std::stoi(av[1]);
	} catch (std::exception &e) {
		error_exit("value of first argument is outside of int scope");
	}
}

void	socket_check(int server_fd) {
	if (server_fd == -1)
		error_exit("socket failed, server fd returned: '-1'");
}


