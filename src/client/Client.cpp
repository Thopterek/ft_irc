#include "../../inc/Client.hpp"

Client::Client() : users() {}

Client::~Client() {}

void	Client::addUser(const int &fd) {
	static_cast<void>(fd);
}