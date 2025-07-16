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
Bot::Bot(std::string name) : bot_name(name), file("ffy.txt", "a bomb of text on your private msg"), file_type(FileType::TEXT),
server_port(6667), server_password("123"), server_ip("10.12.4.8"), bot_fd(-1), connect_to_bot_fd(-1), polling(), fresh() {
	setupSockets();
	setupMsgs();
	std::string file_path = "bot/files_to_transfer/" + file.first;
	std::ifstream load(file_path);
	content = std::string(std::istreambuf_iterator<char>(load), std::istreambuf_iterator<char>());
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
	setupMsgs();
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
	std::string file_path;
	printRequest("pick a file in the 'bot/files_to_transfer'");
	while (file.first == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, file.first);
		file_path = "bot/files_to_transfer/" + file.first;
		std::ifstream actual(file_path);
		if (actual.good() == false && g_shutdown == 0) {
			printMistake("File not available");
			file.first = "";
		}
		if (std::filesystem::is_regular_file(file_path) == false) {
			printMistake("File type is wrong, choose another");
			file.first = "";
		}
	}
	auto	check = file.first.find(".txt");
	if (check != std::string_view::npos) {
		file_type = FileType::TEXT;
		std::ifstream load(file_path);
		content = std::string(std::istreambuf_iterator<char>(load), std::istreambuf_iterator<char>());
	}
	else
		file_type = FileType::BINARY;
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
	Basic messages to be sent and compared to
	as per how the bot is interacting with clients
*/
void	Bot::setupMsgs() {
	joined = "JOIN :#" + bot_name + "\r\n";
	std::string line_one = " :Hello! If you want to use the bot write info, it will show the usage";
	std::string line_two = " :if you want to start file transfer write msg to " + bot_name + " DCC GET " + file.first;
	manual_one = "PRIVMSG #" + bot_name + line_one + "\r\n";
	manual_two = "PRIVMSG #" + bot_name + line_two + "\r\n";
	info_response = "PRIVMSG #" + bot_name + " :file name: " + file.first + " and description: " + file.second + "\r\n";
	dcc_get = "DCC GET " + file.first + "\r\n";
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
	1:PASS, 2:NICK, 3:USER just like any user would do
	then joining the particular channel to check messages
*/
int	Bot::tryConnect() {
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET, server.sin_port = htons(static_cast<uint16_t>(server_port));
	server.sin_addr.s_addr = inet_addr(server_ip.c_str());
	int check = connect(bot_fd, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
	if (check != 0) {
		std::cerr << "Connect to IRC server failed" << std::endl;
		return (1);
	}
	return (sendInitial());
}

/*
	Sending and checking if it goes through
	1:PASS, 2:NICK, 3:USER, 4:JOIN
*/
int	Bot::sendInitial() {
	std::string pass = "PASS " + server_password + "\r\n";
	ssize_t	check = send(bot_fd, pass.c_str(), pass.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending pass failed" << std::endl;
		return (1);
	}
	std::string nick = "NICK " + bot_name + "\r\n";
	check = send(bot_fd, nick.c_str(), nick.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending nick failed" << std::endl;
		return (1);
	}
	std::string user = "USER " + bot_name + " 0 * :" + bot_name + "\r\n";
	check = send(bot_fd, user.c_str(), user.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending user failed" << std::endl;
		return (1);
	}
	std::string join = "JOIN #" + bot_name + "\r\n";
	check = send(bot_fd, join.c_str(), join.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending user failed" << std::endl;
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
			sendBinary(client_fd);
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
Bot::iter	Bot::recvServer(iter it) {
	std::string	buffer;
	buffer.resize(512);
	int check = recv(it->fd, buffer.data(), buffer.size(), MSG_DONTWAIT);
	if (check < 1) {
		std::cerr << "SERVER FAILED" << std::endl;
		close(it->fd);
		bot_fd = -1;
		return (polling.erase(it));
	}
	else {
		buffer.resize(check);
		for (auto print = buffer.begin(); print != buffer.end(); ++print)
			std::cout << *print << std::flush;
		if (buffer.size() > joined.size()) {
			if (buffer.find(joined) != std::string_view::npos)
				sendManual();
		}
		if (buffer.size() > info.size()) {
			if (buffer.find(info) != std::string_view::npos)
				sendInfoResponse();
		}
		if (buffer.size() > dcc_get.size()) {
			if (buffer.find(dcc_get) != std::string_view::npos)
				DCCsend(buffer);
		}
		if (buffer.size() > ping.size()) {
			if (buffer.find(ping) != std::string_view::npos)
				sendPong();
		}
	}
	buffer.clear();
	return (++it);
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
				else if (it->fd == bot_fd && (it->revents & POLLIN) && g_shutdown == 0)
					it = recvServer(it);
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
	getting the path to the file and opening in binary mode
	skipping all the newlines and getting its size
	finding out the end of the file and loading into vector
	then looping through till all of it is being sent
*/
void	Bot::sendBinary(int fd) {
	std::string path = "bot/files_to_transfer/" + file.first;
	std::ifstream	load_file(path, std::ios::binary);
	load_file.unsetf(std::ios::skipws);
	load_file.seekg(0, std::ios::end);
	std::streampos	size = load_file.tellg();
	load_file.seekg(0, std::ios::beg);
	std::vector<char>	buffer;
	buffer.reserve(size);
	buffer.insert(buffer.begin(), std::istream_iterator<char>(load_file), std::istream_iterator<char>(load_file));
	ssize_t total = 0;
	ssize_t	check = 0;
	while (total < static_cast<ssize_t>(buffer.size())) {
		check = send(fd, buffer.data() + total, buffer.size(), MSG_DONTWAIT);
		if (check == -1) {
			std::cerr << "Error: data couldn't be send properly" << std::endl;
			return;
		}
		total += check;
	}
	std::cout << "File send properly to client: " << fd << std::endl;
}

void	Bot::sendInfoResponse() {
	int check = send(bot_fd, info_response.c_str(), info_response.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending info_response failed" << std::endl;
		return ;
	}
}

void	Bot::DCCsend(std::string find_name) {
	auto pos_col = find_name.find_first_of(":");
	auto pos_end = find_name.find_first_of("!");
	if (pos_col != std::string_view::npos && pos_end != std::string_view::npos) {
		std::string user_name = find_name.substr(pos_col + 1, pos_end - pos_col - 1);
		if (file_type == FileType::BINARY) {
			dcc_send = "PRIVMSG " + user_name + " :DCC SEND " + file.first + " " + server_ip + " " + std::to_string(connect_to_bot_fd) + "\r\n";
			int check = send(bot_fd, dcc_send.c_str(), dcc_send.size(), MSG_DONTWAIT);
			if (check == -1) {
				std::cerr << "Error: sending DCCsend failed" << std::endl;
				return ;
			}
		}
		else {
			std::stringstream stream(content);
			std::string part;
			while (std::getline(stream, part)) {
				dcc_send = "PRIVMSG " + user_name + " :" + part + "\r\n";
				int check = send(bot_fd, dcc_send.c_str(), dcc_send.size(), MSG_DONTWAIT);
				if (check == -1) { 
					std::cerr << "Error: sending DCCsend failed" << std::endl;
					return ;
				}
			}
		}
	}
	else
		std::cerr << "DCC send, couldn't find a name to sent the message to" << std::endl;
}

void	Bot::sendManual() {
	int check = send(bot_fd, manual_one.c_str(), manual_one.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending manual part one failed" << std::endl;
		return ;
	}
	check = send(bot_fd, manual_two.c_str(), manual_two.size(), MSG_DONTWAIT);
	if (check == -1) {
		std::cerr << "Error: sending manual part two failed" << std::endl;
	}
}

void	Bot::sendPong() {
	std::string pong = "PONG nsc-irc.local\r\n";
	int check = send(bot_fd, pong.c_str(), pong.size(), MSG_DONTWAIT);
	if (check == -1)
		std::cerr << "Error: sending pong failed" << std::endl;
}
