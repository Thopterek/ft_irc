#ifndef	BOT_HPP
#define	BOT_HPP

#include <unordered_map>
#include <iostream>

class	Bot {
	private:
		// int	server_port;
		// std::string server_password;
		// std::string	bot_name;
		// std::unordered_map<std::string, std::string> files;
	public:
		Bot();
		~Bot();
		Bot(const Bot &other) = delete;
		Bot	&operator=(const Bot &other) = delete;
		/*
			above the mandator OCF form per norm
			below helper functions for bot setup
		*/
		void	checkEof();
};

#endif