#ifndef	CLIENT_HPP
#define	CLIENT_HPP

#include <unordered_map>
#include <iostream>
/*

*/
#include "User.hpp"

class	Client {
	private:
		std::unordered_map<int, User*> users;
	public:
		Client();
		~Client();
		Client(const Client &other) = delete;
		Client	operator=(const Client &other) = delete;
		/*
			above the basic OCF
			under some basic functions
		*/
		// void	addUser(const int &fd, std::string ip, std::string hostname);
};

#endif