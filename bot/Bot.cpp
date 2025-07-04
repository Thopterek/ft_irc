#include "Bot.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

/*
	Constructor made just for testing
	prefilled information without checks
*/
Bot::Bot(std::string name) : bot_name(name), file("ffy.txt", "simple text file"), file_type(FileType::TEXT),
server_port(123), server_password("123"), server_ip("10.12.4.8"), bot_fd(-1), connect_to_bot_fd(-1), polling(), fresh() {
	setupSockets();
	std::cout << "\033[31m\033[1m" << "PREFILLED TEST CONSTRUCTOR USED, ONLY FOR DEBUGGING" << "\033[0m" << std::endl;
}

/*
	Default constructor asking for all of the informations
	has checks to be intact with all of the IRC requirments
*/
Bot::Bot() : bot_name(""), file("", ""), file_type(FileType::UNSET), server_port(0),
server_password(""), server_ip(""), bot_fd(-1), connect_to_bot_fd(-1), polling(), fresh() {
	setupName();
	setupFile();
	setupPort();
	setupPass();
	setupSockets();
}

/*
	name is gonna be used as the:
	real name, nick, username
*/
void	Bot::setupName() {
	printRequest("bot name has to follow IRC rules (RFC 2812)");
	while (bot_name == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, bot_name);
		if ((bot_name.size() > 9 || bot_name.empty() == true) && g_shutdown == 0) {
			printMistake("Name doesn't follow the rule");
			bot_name = "";
		}
	}
}

/*
	information about file to transfer
	to be used in XDCC type connection
*/
void	Bot::setupFile() {
	printRequest("pick a file in the 'bot/files_to_transfer'");
	while (file.first == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, file.first);
		std::string file_path = "bot/files_to_transfer/" + file.first;
		std::ifstream actual(file_path);
		if (actual.good() == false && g_shutdown == 0) {
			printMistake("File not available");
			file.first = "";
		}
	}
	/*
		needed logic to setup the types of file
		as for now file_type wasn't great for it
	*/
	file_type = FileType::TEXT;
	printRequest("write a description between 1 and 510 characters for it");
	while (file.second == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, file.second);
		if ((file.second.size() > 510 || file.second.size() < 1) && g_shutdown == 0) {
			printMistake("Description is not in range");
			file.second = "";
		}
	}
}

/*
	server of the port to be connected to
	the bot port is being setup on its own
*/
void	Bot::setupPort() {
	printRequest("choose the same port as the one that server is running on");
	std::string	input;
	while (server_port == 0 && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, input);
		try {
			server_port = std::stoi(input);
			if ((server_port < 3 || server_port > 65535) && g_shutdown == 0) {
				printMistake("Port must be in range 3 till 65535");
				server_port = 0;
			}
		} catch (std::exception &e) {
			printMistake("Port is not in an integer range");
		}
	}
}

/*
	password used by the server
	and its IPV4 address
*/
void	Bot::setupPass() {
	printRequest("copy the password used to connect to the server");
	while (server_password == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, server_password);
		if (server_password == "" && g_shutdown == 0)
			printMistake("Password can't be empty");
	}
	printRequest("copy the ip address of machine that server runs on");
	while (server_ip == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, server_ip);
		if (server_ip.empty() == true && g_shutdown == 0)
			printMistake("IP address for sure can't be empty");
	}
}

/*
	part of the setup that happens on its own
	sockets made on the free ones for machine
*/
void	Bot::setupSockets() {
	bot_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (bot_fd < 0)
		cleanExit("socket on bot_fd returned error");
	connect_to_bot_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (connect_to_bot_fd < 0)
		cleanExit("socket on connect_to_bot_fd returned error");
	int yes = 1, check = setsockopt(connect_to_bot_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (check == -1)
		cleanExit("setting socket option with REUSE address failed");
	check = setsockopt(connect_to_bot_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	if (check == -1)
		cleanExit("setting socket option to disable Nagle algo failed");
	int flag = fcntl(connect_to_bot_fd, F_SETFL, O_NONBLOCK);
	if (flag < 0)
		cleanExit("fcntl failed to set non blocking for connect_to_bot_fd");
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET, address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(connect_to_bot_fd));
	check = bind(connect_to_bot_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
	if (check == -1)
		cleanExit("bind for the connect_to_bot_fd failed in setupSockets");
	check = listen(connect_to_bot_fd, SOMAXCONN);
	if (check == -1)
		cleanExit("listen for connect_to_bot_fd failed in setupSockets");
	std::cout << "\033[33m\033[1m" << "Bot waits for connection at port: " << "\033[0m" << connect_to_bot_fd << std::endl;
}

/*
	used before the polling loop
	cleaning up all the information
*/
void	Bot::cleanExit(const std::string &msg) {
	std::cerr << "Error: part of the setup failed, " << msg << std::endl;
	if (bot_fd > 0) {
		if (close(bot_fd) != 0)
			std::cerr << "Close for bot_fd in cleanExit failed" << std::endl;
	}
	if (connect_to_bot_fd > 0) {
		if (close(connect_to_bot_fd) != 0)
			std::cerr << "Close for connect_to_bot_fd in cleanExit failed" << std::endl;
	}
	for (iter it = polling.begin(); it != polling.end(); ++it) {
		if (it->fd != bot_fd && it->fd != connect_to_bot_fd)
			close(it->fd);
	}
	polling.clear();
	exit(1);	
}

/*
	getting the messages from the server on the bot_fd
	keeping track of incoming connections on connect_to_bot_fd
*/
void	Bot::initialPolling() {
	if (g_shutdown == 0) {
		struct pollfd connect_bot;
		memset(&connect_bot, 0, sizeof(connect_bot));
		connect_bot.fd = bot_fd, connect_bot.events = POLLIN;
		polling.push_back(connect_bot);
		struct pollfd file_transfer;
		memset(&file_transfer, 0, sizeof(file_transfer));
		file_transfer.fd = connect_to_bot_fd, file_transfer.events = POLLIN;
		polling.push_back(file_transfer);
	}
}

/*
	try to connect to the server, if works correctly send:
	1:PASS, 2:USER, 3:NICK just like any user would do
	then joining the particular channel to check messages
*/
int	Bot::tryConnect() {
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET, server.sin_port = htons(static_cast<uint16_t>(server_port));
	server.sin_addr.s_addr = inet_addr(server_ip.c_str());
	int check = connect(bot_fd, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
	if (check != 0) {
		std::cerr << "Connect failed" << std::endl;
		return (1);
	}
	return (0);
}

/*
	accepting connection for file transfer
	keeping the track of the clients
*/
void	Bot::acceptUser() {
	struct	sockaddr_in client_info;
	memset(&client_info, 0, sizeof(client_info));
	socklen_t	info_size = sizeof(client_info);
	int client_fd = accept(connect_to_bot_fd, reinterpret_cast<struct sockaddr*>(&client_info), &info_size);
	if (client_fd == -1)
		return ;
	else {
		int flag = fcntl(client_fd, F_SETFL, O_NONBLOCK);
		if (flag == -1) {
			close(client_fd);
			return ;
		}
		else {
			struct pollfd new_client;
			memset(&new_client, 0, sizeof(new_client));
			new_client.fd = client_fd;
			new_client.events = POLLIN;
			fresh.push_back(new_client);
			std::cout << "USER CONNECTED TO THE BOT" << std::endl;
		}
	}
}

/*
	receving information from ones who connected above
	closing the connection if the client would be killed/closed
*/
Bot::iter	Bot::recvUser(iter it) {
	std::string	buffer;
	buffer.resize(512);
	int check_receive = recv(it->fd, buffer.data(), buffer.size(), MSG_DONTWAIT);
	if (check_receive == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
		std::cerr << "Client with fd: " << it->fd << " had some error" << std::endl;
		close(it->fd);
		buffer.clear();
		it = polling.erase(it);
		return (it);
	}
	else if (check_receive == 0) {
		std::cout << "Client with fd: '" << it->fd << "' disconnected" << std::endl;
		close(it->fd);
		buffer.clear();
		it = polling.erase(it);
		return (it);
	}
	buffer.resize(check_receive);
	for (auto print = buffer.begin(); print != buffer.end(); ++print)
		std::cout << *print << std::flush;
	std::cout << " <- from client: " << it->fd << std::endl;
	buffer.clear();
	return (++it);
}

/*
	getting the messages that were sent from the server
	checking for the particular types to send the response
*/
void	Bot::recvServer() {
	std::string	buffer;
	buffer.resize(512);
	int check = recv(bot_fd, buffer.data(), buffer.size(), MSG_DONTWAIT);
	if (check < 1)
		std::cerr << "WE GOT AN ERROR" << std::endl;
	else {
		buffer.resize(check);
		// if (buffer == "info\n\r")
		for (auto print = buffer.begin(); print != buffer.end(); ++print)
			std::cout << *print << std::flush;
	}
	buffer.clear();
}

/*
	main loop of the bot to handle all connections
	and main caller depending on the poll data
*/
void	Bot::runBot() {
	initialPolling();
	if (tryConnect() != 0 && g_shutdown == 0)
		return;
	while (g_shutdown == 0) {
		int	check = poll(polling.data(), polling.size(), -1);
		if (check == -1 && g_shutdown == 0)
			std::cerr << "Poll returned error" << std::endl;
		else {
			for (iter it = polling.begin(); it != polling.end();) {
				if (it->fd == connect_to_bot_fd && (it->revents & POLLIN) && g_shutdown == 0) {
					acceptUser();
					++it;
				}
				else if (it->fd != connect_to_bot_fd && it->fd != bot_fd && (it->revents & POLLIN) && g_shutdown == 0)
					it = recvUser(it);
				else if (it->fd == bot_fd && (it->revents & POLLIN) && g_shutdown == 0) {
					recvServer();
					++it;
				}
				else
					++it;
			}
			for (const struct pollfd &saved : fresh)
				polling.push_back(saved);
			fresh.clear();
		}
	}
}

/*
	Destructor with some safety checks
	handling everything after going to runBot
*/
Bot::~Bot() {
	if (bot_fd > 0) {
		if (close(bot_fd) != 0)
			std::cerr << "Close for bot_fd in destructor failed" << std::endl;
	}
	if (connect_to_bot_fd > 0) {
		if (close(connect_to_bot_fd) != 0)
			std::cerr << "Close for connect_to_bot_fd in destructor failed" << std::endl;
	}
	for (iter it = polling.begin(); it != polling.end(); ++it) {
		if (it->fd != bot_fd && it->fd != connect_to_bot_fd)
			close(it->fd);
	}
	polling.clear();
}

/*
	Helps with handling signal as to not break program
	when user is inputing the bot information
*/
void	Bot::checkEof() {
	if (g_shutdown == 0) {
		if (std::cin.eof()) {
			std::cin.clear();
			freopen("/dev/tty", "r", stdin);
		}
	}
}

/*
	wrappers for repeated questions for user
*/
void	Bot::printRequest(const std::string &rules) const {
	std::cout << "\033[33m\033[1m" << "Rules: "  << "\033[0m" << rules << std::endl;
	std::cout << "\033[33m\033[1m" << "Please fill out: " << "\033[0m" << std::flush;
}

void	Bot::printMistake(const std::string &mistake) const {
	std::cout << "\033[31m\033[1m" << mistake << ", try again: " << "\033[0m" << std::flush;
}
