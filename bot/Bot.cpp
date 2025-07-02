#include "Bot.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

Bot::Bot(std::string name) : bot_name(name), file("ffy.txt", "simple text file"), file_type(FileType::TEXT), server_port(123), server_password("123") {
	setupSocket();
	std::cout << "\033[31m\033[1m" << "PREFILLED TEST CONSTRUCTOR USED, ONLY FOR DEBUGGING" << "\033[0m" << std::endl;
}

Bot::Bot() : bot_name(""), file("", ""), file_type(FileType::UNSET), server_port(0), server_password(""), bot_fd(-1) {
	setupName();
	setupFile();
	setupPort();
	setupPass();
	setupSocket();
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

void	Bot::setupSocket() {
	bot_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (bot_fd < 0) {
		std::cerr << "Failed to create a vaild socket" << std::endl;
		exit(1);
	}
	int flag = fcntl(bot_fd, F_SETFL, O_NONBLOCK);
	if (flag < 0) {
		std::cerr << "Failed to set non blocking socket" << std::endl;
		if (close(bot_fd) != 0)
			std::cerr << "Close in setupSocket failed" << std::endl;
		exit(1);
	}
}

/*
	anything will work
*/
void	Bot::connectBot() {
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET, server.sin_port = htons(static_cast<uint16_t>(server_port));
	connect(bot_fd, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
	runBot();
}

void	Bot::runBot() {
	std::cout << "bot should be running" << std::endl;
	// while (g_shutdown == 0)
	// {}
}

Bot::~Bot() {
	if (bot_fd > 0) {
		if (close(bot_fd) != 0)
			std::cerr << "Close in destructor failed" << std::endl;
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
