#include "../inc/Server.hpp"

int	main(int ac, char **av) {
	Server	snc(ac, av);
	snc.setupServer();
	return (EXIT_SUCCESS);
}