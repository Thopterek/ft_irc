#include "../../inc/Server.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

Server::Server(int ac, char **av)
: server_fd(-1), polling(), clients() {
	try {
		ac_check(ac);
		port_check(av);
		port = std::stoi(av[1]);
		password = av[2];
		server_fd = socket(PF_INET, SOCK_STREAM, 0);
		socket_check(server_fd);
		std::cout << "Server with port: " << port << std::endl;
		std::cout << "and password: " << password << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

Server::~Server() {
	if (server_fd >= 0) {
		close(server_fd);
		server_fd = -1;
	}
	// const std::string goodbye = "";
	for (auto it = polling.begin(); it != polling.end(); ++it) {
		// int check = send(it->fd, )
		close(it->fd);
	}
	polling.clear();
}

const int	&Server::getPort() const {return (port);}

bool	Server::validPassword(const std::string &pass) const {
	if (pass == password)
		return (true);
	return (false);
}

const std::string	&Server::getServerName() const {return (server_name);}

const char *Server::errorAc::what() const throw() {
	return ("Error: 'constructor' usage ./ircserv <port> <password>");
}

const char *Server::errorPort::what() const throw() {
	return ("Error: 'constructor' port is outside of scope '1 <= 65535'");
}

const char *Server::errorSocket::what() const throw() {
	return ("Error: 'constructor' socket failed, server fd returned: '-1'");
}

void	Server::ac_check(int ac) const {
	if (ac != 3)
		throw errorAc();
}

void	Server::port_check(char **av) const {
	try {
		int tmp = std::stoi(av[1]);
		if (tmp <= 0 || tmp > 65535)
			throw errorPort();
	} catch (std::exception &e) {
		throw errorPort();
	}
}

void	Server::socket_check(int fd) const {
	if (fd == -1)
		throw errorSocket();
}

/*
	second part of getting through before running the server
	setting up the non blocking and other basic functionality
*/
void	Server::setupServer() {
	try {
		socket_options();
		non_blocking();
		assign_address();
		use_to_connect();
		std::cout << "Server setup done" << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		cleanExit();
	}
}

void	Server::cleanExit() {
	if (server_fd >= 0) {
		close(server_fd);
		server_fd = -1;
	}
	for (auto it = polling.begin(); it != polling.end(); ++it)
		close(it->fd);
	polling.clear();
	exit(EXIT_FAILURE);
}

const char *Server::errorOptions::what() const throw() {
	return ("Error: 'setup' setsockopt(2) failed, rtfm");
}

const char *Server::errorFcntl::what() const throw() {
	return ("Error: 'setup' fcntl(2) failed, rtfm");
}

const char *Server::errorBind::what() const throw() {
	return ("Error: 'setup' bind(2) failed, rtfm");
}

const char *Server::errorListen::what() const throw() {
	return ("Error: 'setup' listen(2) failed, rtfm");
}

void	Server::socket_options() {
	int tmp = 1;
	int check = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
	check = setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
}

void	Server::non_blocking() {
	int flag = fcntl(server_fd, F_SETFL, O_NONBLOCK);
	if (flag == -1)
		throw errorFcntl();
}

void	Server::assign_address() {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(port));
	int check = bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
	if (check == -1)
		throw errorBind();
}

void	Server::use_to_connect() {
	int check = listen(server_fd, SOMAXCONN);
	if (check == -1)
		throw errorListen();
}

/*
	now we are getting to part with running the server
	everything here is contained in the runServer ft
*/
void	Server::runError(const std::string &msg, const int &fd) const {
	if (fd == 0)
		std::cerr << "Error: 'runServer' " << msg << std::endl;
	else
		std::cerr << "Error: 'runServer' " << msg << " for '" << fd << "'" << std::endl;
}

void	Server::sendMsg(std::string msg, int fd) {
	if (fd < 0)
		return;
	std::string full = msg + "\r\n";
	ssize_t	check = send(fd, full.c_str(), full.size(), 0);
	if (check == -1)
		runError("MSG failed to be send", fd);
}

void	Server::acceptingClient() {
	// std::cout << "we are trying to accept the client" << std::endl;
	while (true) {
		// struct	sockaddr_in info;
		// socklen_t	info_size = sizeof(info);
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
			runError("accept failed, rtfm", 0);
			break ;
		}
		else if (client_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			break ;
		else {
			int flag = fcntl(client_fd, F_SETFL, O_NONBLOCK);
			if (flag == -1) {
				runError("fcntl call for new client failed", client_fd);
				close(client_fd);
				continue;
			}
			else {
				// sendMsg("Welcome to our IRC server", client_fd);
				struct pollfd new_client;
				new_client.fd = client_fd;
				new_client.events = POLLIN;
				// std::string address = inet_ntoa(info.sin_addr);
				/*
					adding a user to Client handler
					using his fd, address and hostname
				*/
				polling.push_back(new_client);
			}
		}
	}
}

int	Server::receivingData(const int &sockfd) {
	// std::cout << "we received the data" << std::endl;
	std::vector<char>	buffer;
	buffer.resize(1024);
	int check_receive = recv(sockfd, buffer.data(), buffer.size(), 0);
	if (check_receive == -1) {
		runError("recv didn't proccess the message", sockfd);
		close(sockfd);
		buffer.clear();
		return (EXIT_FAILURE);
	}
	else if (check_receive == 0) {
		std::cout << "Client with fd: '" << sockfd << "' disconnected" << std::endl;
		sendMsg("Goodbye and comeback soon", sockfd);
		close(sockfd);
		buffer.clear();
		return (EXIT_FAILURE);
	}
	for (auto it = buffer.begin(); it != buffer.end(); ++it)
		std::cout << *it << std::flush;
	buffer.clear();
	return (EXIT_SUCCESS);
}

void	Server::runServer() {
	struct pollfd polling_pool;
	polling_pool.fd = server_fd;
	polling_pool.events = POLLIN;
	polling.push_back(polling_pool);
	while (g_shutdown == 0) {
		int check_poll = poll(polling.data(), polling.size(), -1);
		if (check_poll == 0)
			runError("poll timed out before fd was ready", 0);
		else if (check_poll == -1 && g_shutdown == 0)
			runError("poll returned error, rtfm", 0);
		else {
			for (auto it = polling.begin(); it != polling.end(); ++it) {
				// std::cout << "we are in the for loop" << std::endl;
				if (it->fd == server_fd && (it->revents & POLLIN))
					acceptingClient();
				else if (it->fd != server_fd && (it->revents & POLLIN)) {
					int closed = receivingData(it->fd);
					if (closed == EXIT_FAILURE) {
						polling.erase(it);
						break;
					}
				}
				else if (it->fd != server_fd && (it->revents & POLLHUP)) {
					sendMsg("Another goodbye", it->fd);
					close(it->fd);
					polling.erase(it);
					break;
				}
			}
		}
	}
}