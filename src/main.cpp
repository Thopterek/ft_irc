#include "../inc/Server.hpp"

int	main(int ac, char **av) {
	std::signal(SIGINT, handler);
	Server	snc(ac, av);
	snc.setupServer();
	snc.runServer();
	return (EXIT_SUCCESS);
}


Gib mir TLS-Zertifikat-Rolle“
„Exportiertes Kibana Dashboard bitte“
„Mach Bastion-Host Setup“
„Auditd-Rolle erstellen“