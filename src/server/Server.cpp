#include "../../inc/Server.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

Server::Server(int ac, char **av)
: server_fd(-1), polling(), fresh(), clients(), pars() {
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
	for (auto it = polling.begin(); it != polling.end(); ++it) {
		if (it->fd != server_fd) {
			sendMsg("Server is closing, see you later", it->fd);
			if (close(it->fd) != 0)
				runError("close failed for polling at destructor", it->fd);
		}
	}
	for (auto it = fresh.begin(); it != fresh.end(); ++it) {
		if (close(it->fd) != 0)
				runError("close failed for polling at destructor", it->fd);	
	}
	fresh.clear();
	if (server_fd >= 0) {
		if (close(server_fd) != 0)
			runError("close failed for server_fd at destructor", server_fd);
		server_fd = -1;
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
	for (auto it = polling.begin(); it != polling.end(); ++it) {
		if (it->fd != server_fd) {
			if (close(it->fd) != 0)
				runError("close in cleanExit for polling", it->fd);
		}
	}
	if (server_fd >= 0) {
		if (close(server_fd) != 0)
			runError("close in cleanExit for server_fd", server_fd);
		else
			server_fd = -1;
	}
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
	ssize_t	check = send(fd, full.c_str(), full.size(), MSG_DONTWAIT);
	if (check == -1) {
		recvErrno();
		runError("MSG failed to be send", fd);
	}
}

void	Server::acceptingClient() {
	struct	sockaddr_in client_info;
	memset(&client_info, 0, sizeof(client_info));
	socklen_t	info_size = sizeof(client_info);
	int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_info), &info_size);
	if (client_fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
		runError("accept failed, rtfm", 0);
		return ;
	}
	else if (client_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return ;
	else {
		int flag = fcntl(client_fd, F_SETFL, O_NONBLOCK);
		if (flag == -1) {
			runError("fcntl call for new client failed", client_fd);
			if (close(client_fd) != 0)
				runError("close in acceptingClient", client_fd);
			return ;
		}
		else {
			struct pollfd new_client;
			memset(&new_client, 0, sizeof(new_client));
			new_client.fd = client_fd;
			new_client.events = POLLIN;
			std::string address = inet_ntoa(client_info.sin_addr);
			struct hostent *client_host = gethostbyname(address.c_str());
			std::string hostname = client_host->h_name;
			std::cout << "new client at: '" << address << "'" << std::endl;
			std::cout << "and with fd: '" << client_fd << "' got accepted" << std::endl;
			std::cout << "his hostname is '" << hostname << "'" << std::endl;
			fresh.push_back(new_client);
			clients.connect(client_fd, address, hostname);
			sendMsg("Welcome to our IRC server", client_fd);
		}
	}
}

void	Server::recvErrno() {
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		std::cerr << "it was EAGAIN" << std::endl;
	else if (errno == EBADF)
		std::cerr << "it was EBADF: sockfd is an invalid fd" << std::endl;
	else if (errno == ECONNREFUSED)
		std::cerr << "it was ECONNREFUSED" << std::endl;
	else if (errno == EFAULT)
		std::cerr << "it was EFAULT" << std::endl;
	else if (errno == EINTR)
		std::cerr << "it was EINTR" << std::endl;
	else if (errno == EINVAL)
		std::cerr << "it was EINVAL" << std::endl;
	else if (errno == ENOTCONN)
		std::cerr << "it was ENOTCONN: The socket is not connected" << std::endl;
	else if (errno == ENOTSOCK)
		std::cerr << "it was ENOTSOCK" << std::endl;
} 

Server::iter	Server::receivingData(iter it) {
	// std::vector<char>	buffer;
	std::string	buffer;
	buffer.resize(512);
	int check_receive = recv(it->fd, buffer.data(), buffer.size(), MSG_DONTWAIT);
	if (check_receive == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
		std::cout << "double check it was: " << it->fd << std::endl;
		runError("recv didn't proccess the message", it->fd);
		recvErrno();
		if (close(it->fd) != 0)
			runError("close in receivingData", it->fd);
		buffer.clear();
		it = polling.erase(it);
		return (it);
	}
	else if (check_receive == 0) {
		std::cout << "Client with fd: '" << it->fd << "' disconnected" << std::endl;
		sendMsg("Goodbye and comeback soon", it->fd);
		if (close(it->fd) != 0)
			runError("close in receivingData", it->fd);
		buffer.clear();
		it = polling.erase(it);
		return (it);
	}
	buffer.resize(check_receive);
	std::cout << it->fd << " has sent: " << std::flush;
	for (auto print = buffer.begin(); print != buffer.end(); ++print)
		std::cout << *print << std::flush;
	if (buffer.size() > 2) {
		auto slash_n = std::prev(buffer.end(), 1);
		auto carnage = std::prev(buffer.end(), 2);
		if (*slash_n == '\n' && *carnage == '\r') {
			std::cout << "message ended with carnage and newline" << std::endl;
			clients[it->fd].buffer(buffer.data());
			pars.parseAndDispatch(clients[it->fd]);
		}
		else {
			std::cout << "message is going to be buffered" << std::endl;
			clients[it->fd].buffer(buffer.data());	
		}
	}
	buffer.clear();
	return (++it);
}

Server::iter	Server::removeError(iter it) {
	std::cout << "Client with fd: '" << it->fd << "' had an error" << std::endl;
	if (close(it->fd) != 0)
		runError("Close on removeError failed", it->fd);
	return (polling.erase(it));
}

Server::iter	Server::removeInvalid(iter it) {
	std::cout << "File descriptor was never opened or already closed: " << it->fd << std::endl;
	return (polling.erase(it));
}

Server::iter	Server::removeDisconnected(iter it) {
	std::cout << "Client with: " << it->fd << " has disconnected" << std::endl;
	clients.disconnect(it->fd);
	if (close(it->fd) != 0)
		runError("Close on disconnection failed", it->fd);
	return (polling.erase(it));
}

void	Server::runServer() {
	struct pollfd polling_pool;
	memset(&polling_pool, 0, sizeof(polling_pool));
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
			for (iter it = polling.begin(); it != polling.end();) {
				if (it->fd == server_fd && (it->revents & POLLIN) && g_shutdown == 0) {
					acceptingClient();
					++it;
				}
				else if (it->fd != server_fd && it->fd > 2 && (it->revents & POLLNVAL) && g_shutdown == 0)
					it = removeInvalid(it);
				else if (it->fd != server_fd && it->fd > 2 && (it->revents & POLLHUP) && g_shutdown == 0)
					it = removeDisconnected(it);
				else if (it->fd != server_fd && it->fd > 2 && (it->revents & POLLERR) && g_shutdown == 0)
					it = removeError(it);
				else if (it->fd != server_fd && it->fd > 2 && (it->revents & POLLIN) && g_shutdown == 0)
					it = receivingData(it);
				else
					++it;
			}
			for (const struct pollfd &saved : fresh)
				polling.push_back(saved);
			fresh.clear();
		}
	}
}