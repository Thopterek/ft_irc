#ifndef	BOT_HPP
#define	BOT_HPP

#include <utility>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <csignal>
#include <cstring>
#include <cerrno>
/*
	C++ style libraries above
	and the C ones below
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

/*
	Type of a file thats can be sent
	unset is the base before checkings
*/
enum class	FileType {
	UNSET,
	TEXT,
	BINARY
};

extern volatile std::sig_atomic_t g_shutdown;
void	handler(int signum);
/*
	handler and actual signal flag
	to shutdown bot gracefully
	below bot class and fts
*/
class	Bot {
	private:
		std::string	bot_name;
		std::pair<std::string, std::string> file;
		FileType	file_type;
		int	server_port;
		std::string server_password, server_ip;
		/*
			above is the part filled by the user
			only exception is the file_type check
			below things handled automatically
		*/
		int	bot_fd, connect_to_bot_fd;
		std::vector<struct pollfd> polling;
		std::vector<struct pollfd> fresh;
	public:
		Bot();
		Bot(std::string name);
		~Bot();
		Bot(const Bot &other) = delete;
		Bot	&operator=(const Bot &other) = delete;
		/*
			above the mandator OCF form per norm
			and extra constructor for testing
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
		void	setupSockets();
		void	cleanExit(const std::string &msg);
		/*
			helper typedef to pass around easier
			the iterator used by the polling
		*/
		typedef	std::vector<struct pollfd>::iterator iter;
		/*
			acutally connecting to the server
			creating the channel and waiting
			sending the intial combination
		*/
		void	initialPolling();
		int	tryConnect();
		int	sendInitial();
		/*
			logic for managing the connections
			the ones from the server side
			and ones to call for file transfer
		*/
		void	runBot();
		void	acceptUser();
		iter	recvUser(iter it);
		iter	recvServer(iter it);
		/*
			helper functions for the manager
			and actual file transfer function
			plus the sending the messages
		*/
		void	sendBinary(int fd);
		void	sendInfo();
		void	sendDCC();
};

#endif