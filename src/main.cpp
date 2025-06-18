#include "../inc/Server.hpp"

int	main(int ac, char **av) {
	std::signal(SIGINT, handler);
	Server	snc(ac, av);
	snc.setupServer();
	snc.runServer();
	return (EXIT_SUCCESS);
}