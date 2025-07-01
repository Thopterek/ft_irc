#include "Bot.hpp"

// Bot::Bot() : server_port(0), server_password(""), bot_name(""), files() {}

Bot::Bot() {}

Bot::~Bot() {}

void	Bot::checkEof() {
	if (std::cin.eof()) {
		std::cin.clear();
		freopen("/dev/tty", "r", stdin);
	}
}