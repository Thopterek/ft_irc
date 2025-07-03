#include "Bot.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

Bot::Bot(std::string name) : bot_name(name), file("ffy.txt", "simple text file"),
file_type(FileType::TEXT), server_port(123), server_password("123"), bot_fd(-1), file_fd(-1) {
	setupSockets();
	std::cout << "\033[31m\033[1m" << "PREFILLED TEST CONSTRUCTOR USED, ONLY FOR DEBUGGING" << "\033[0m" << std::endl;
}

Bot::Bot() : bot_name(""), file("", ""), file_type(FileType::UNSET), server_port(0),
server_password(""), bot_fd(-1), file_fd(-1) {
	setupName();
	setupFile();
	setupPort();
	setupPass();
	setupSockets();
}

void	Bot::setupName() {
	printRequest("bot name has to be max 9 characters");
	while (bot_name == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, bot_name);
		if (bot_name.size() > 9 && g_shutdown == 0) {
			printMistake("Name doesn't follow the rule");
			bot_name = "";
		}
	}
}

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

void	Bot::setupPass() {
	printRequest("copy the password used to connect to the server");
	while (server_password == "" && g_shutdown == 0) {
		checkEof();
		std::getline(std::cin, server_password);
		if (server_password == "" && g_shutdown == 0)
			printMistake("Password can't be empty");
	}
}

void	Bot::setupSockets() {
	bot_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (bot_fd < 0)
		cleanExit("socket on bot_fd returned error");
	int flag = fcntl(bot_fd, F_SETFL, O_NONBLOCK);
	if (flag < 0)
		cleanExit("fcntl failed to set non blocking for bot_fd");
	file_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (file_fd < 0)
		cleanExit("socket on file_fd returned error");
	int yes = 1, check = setsockopt(file_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (check == -1)
		cleanExit("setting socket option with REUSE address failed");
	check = setsockopt(file_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
	if (check == -1)
		cleanExit("setting socket option to disable Nagle algo failed");
	flag = fcntl(file_fd, F_SETFL, O_NONBLOCK);
	if (flag < 0)
		cleanExit("fcntl failed to set non blocking for file_fd");
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET, address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(file_fd));
	check = bind(file_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
	if (check == -1)
		cleanExit("bind for the file_fd failed in setupSockets");
	check = listen(file_fd, SOMAXCONN);
	if (check == -1)
		cleanExit("listen for file_fd failed in setupSockets");
	std::cout << "\033[33m\033[1m" << "Bot waits for connection at port: " << "\033[0m" << file_fd << std::endl;
}

void	Bot::cleanExit(const std::string &msg) {
	std::cerr << "Error: part of the setup failed, " << msg << std::endl;
	if (bot_fd > 0) {
		if (close(bot_fd) != 0)
			std::cerr << "Close for bot_fd in cleanExit failed" << std::endl;
	}
	if (file_fd > 0) {
		if (close(file_fd) != 0)
			std::cerr << "Close for file_fd in cleanExit failed" << std::endl;
	}
	exit(1);	
}

/*
	anything will work
*/
void	Bot::connectBot() {
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET, server.sin_port = htons(static_cast<uint16_t>(server_port));
	connect(bot_fd, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
}

void	Bot::runBot() {
	std::cout << "bot should be running" << std::endl;
	// while (g_shutdown == 0)
	// {}
}

Bot::~Bot() {
	if (bot_fd > 0) {
		if (close(bot_fd) != 0)
			std::cerr << "Close for bot_fd in destructor failed" << std::endl;
	}
	if (file_fd > 0) {
		if (close(file_fd) != 0)
			std::cerr << "Close for file_fd in destructor failed" << std::endl;
	}
}

void	Bot::checkEof() {
	if (g_shutdown == 0) {
		if (std::cin.eof()) {
			std::cin.clear();
			freopen("/dev/tty", "r", stdin);
		}
	}
}

void	Bot::printRequest(const std::string &rules) const {
	std::cout << "\033[33m\033[1m" << "Rules: "  << "\033[0m" << rules << std::endl;
	std::cout << "\033[33m\033[1m" << "Please fill out: " << "\033[0m" << std::flush;
}

void	Bot::printMistake(const std::string &mistake) const {
	std::cout << "\033[31m\033[1m" << mistake << ", try again: " << "\033[0m" << std::flush;
}
