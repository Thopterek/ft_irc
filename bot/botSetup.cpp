#include "Bot.hpp"

static void welcome() {
	std::cout << "\033[1m" << std::flush;
	std::cout << "███████████            █████                         █████  " << std::endl;
	std::cout << "░░███░░░░░███          ░░███                         ░░███  " << std::endl;
	std::cout << " ░███    ░███  ██████  ███████       █████   ██████  ███████   █████ ████ ████████ " << std::endl;
	std::cout << " ░██████████  ███░░███░░░███░       ███░░   ███░░███░░░███░   ░░███ ░███ ░░███░░███ " << std::endl;
	std::cout << " ░███░░░░░███░███ ░███  ░███       ░░█████ ░███████   ░███     ░███ ░███  ░███ ░███ " << std::endl;
	std::cout << " ░███    ░███░███ ░███  ░███ ███    ░░░░███░███░░░    ░███ ███ ░███ ░███  ░███ ░███ " << std::endl;
	std::cout << " ███████████ ░░██████   ░░█████     ██████ ░░██████   ░░█████  ░░████████ ░███████ " << std::endl;
	std::cout << "░░░░░░░░░░░   ░░░░░░     ░░░░░     ░░░░░░   ░░░░░░     ░░░░░    ░░░░░░░░  ░███░░░  " << std::endl;
	std::cout << "                                                                          ░███     " << std::endl;
	std::cout << "Please fill out all of the necessary informations                         █████  " << std::endl;
	std::cout << "                                                                         ░░░░░  " << std::endl;
	std::cout << "\033[0m" << std::endl;
}

int	main(void) {
	std::signal(SIGINT, handler);
	welcome();
	Bot	marvin;
	// Bot	marvin("Marvin");
	marvin.runBot();
	return (0);
}