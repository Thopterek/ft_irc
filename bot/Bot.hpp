#ifndef	BOT_HPP
#define	BOT_HPP

#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
/*
	C++ style libraries above
	and the C ones below
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

extern volatile std::sig_atomic_t g_shutdown;
void	handler(int signum);

/*
	handler and actual signal flag
	to shutdown bot gracefully
	below the implementation
*/

class	Bot {
	private:
		std::string	bot_name;
		std::pair<std::string, std::string> file;
		int	server_port;
		std::string server_password;
		int	bot_fd;
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
		void	printRequest(const std::string &rules) const;
		void	printMistake(const std::string &mistake) const;
		/*
			constructor related functions
			that change private members
		*/
		void	setupName();
		void	setupFile();
		void	setupPort();
		void	setupPass();
		void	setupSocket();
		/*
			acutally connecting to the server
			and sending all requried messages
		*/
		void	connectBot();
		void	runBot();

};

#endif