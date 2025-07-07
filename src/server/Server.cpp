#include "../../inc/Server.hpp"

volatile std::sig_atomic_t g_shutdown = 0;

void	handler(int signum) {
	static_cast<void>(signum);
	g_shutdown = 1;
}

/*
	constructor and parser for arguments from main
	initializing the base structure to be used later
*/
Server::Server(int ac, char **av)
: server_fd(-1), polling(), fresh(), clients(), pars() {
	try {
		ac_check(ac);
		port_check(av);
		port = std::stoi(av[1]);
		password = av[2];
		server_fd = socket(PF_INET, SOCK_STREAM, 0);
		socket_check(server_fd);
		printDone();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

/*
	Base constructor with some safety checks
	as for the order of closing the connections
*/
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

/*	
	Self explained block of functions
	related to the things done already
*/
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

/*
	check the count of arguments
*/
void	Server::ac_check(int ac) const {
	if (ac != 3)
		throw errorAc();
}

/*
	check the validity of the port
	if it can be open for IRC
*/
void	Server::port_check(char **av) const {
	try {
		int tmp = std::stoi(av[1]);
		if (tmp <= 2 || tmp > 65535)
			throw errorPort();
	} catch (std::exception &e) {
		throw errorPort();
	}
}

/*
	checking of the automatic server socket
*/
void	Server::socket_check(int fd) const {
	if (fd == -1)
		throw errorSocket();
}

/*
	Welcome message after sever gets through constructor
*/
void	Server::printDone() const {
	std::cout << "\033[1m";
	std::cout << "       .d8888b. " << std::endl;
	std::cout << "      d88P  Y88b " << std::endl;
	std::cout << "      Y88b. " << std::endl;
	std::cout << "       Y888b.    .d88b.  888d888 888  888  .d88b.  888d888 " << std::endl;
	std::cout << "          Y88b. d8P  Y8b 888P   888  888 d8P  Y8b 888P " << std::endl;
	std::cout << "            888 88888888 888     Y88  88P 88888888 888 " << std::endl;
	std::cout << "     Y88b  d88P Y8b.     888      Y8bd8P  Y8b.     888 " << std::endl;
	std::cout << "         Y8888P   Y8888  888       Y88P    Y8888  888 " << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "d8b                                 888 " << std::endl;
	std::cout << "Y8P                                 888 " << std::endl;
	std::cout << "                                    888 " << std::endl;
	std::cout << "888 .d8888b       .d8888b   .d88b.  888888 888  888 88888b. " << std::endl;
	std::cout << "888 88K           88K      d8P  Y8b 888    888  888 888  88b " << std::endl;
	std::cout << "888  Y8888b.       Y8888b. 88888888 888    888  888 888  888 " << std::endl;
	std::cout << "888      X88           X88 Y8b.     Y88b.  Y88b 888 888 d88P " << std::endl;
	std::cout << "888  88888P'       88888P'   Y8888    Y888   Y88888 88888P " << std::endl;
	std::cout << "                                                    888 " << std::endl;
	std::cout << "                                                    888 " << std::endl;    
	std::cout << "                                                    888 " << "\033[0m" << std::endl;
	std::cout << "Runs on the port: " << "\033[33m\033[1m" << port << "\033[0m" << std::flush;
	std::cout << " and password: " << "\033[33m\033[1m" << password << "\033[0m" << std::endl;
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
		std::cout << "all of the above checks went through correctly, try to connect" << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		cleanExit();
	}
}

/*
	if the error would happen in:
	- Server constructor
	- setupServer member
*/
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

/*
	possible errors for used functions
*/
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

/*
	setting and dissabling some of the socket options
	as to be more intact with how socket should be used
*/
void	Server::socket_options() {
	int tmp = 1, check = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
	check = setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &tmp, sizeof(tmp));
	if (check == -1)
		throw errorOptions();
	std::cout << "socket " << "\033[33m\033[1m" << "REUSE" << "\033[0m" <<  " and " << std::flush; 
	std::cout << "\033[33m\033[1m" <<  "Nagle " << "\033[0m" << std::flush;
}

/*
	as per the subject requirments
	every socket used by server is non blocking
*/
void	Server::non_blocking() {
	int flag = fcntl(server_fd, F_SETFL, O_NONBLOCK);
	if (flag == -1)
		throw errorFcntl();
	std::cout << "fcntl " << "\033[33m\033[1m" << "NONBLOCK " << "\033[0m" << std::flush;
}

/*
	binding the address to be used for the server
	letting to conenction with the port from args
*/
void	Server::assign_address() {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(port));
	int check = bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
	if (check == -1)
		throw errorBind();
	std::cout << "bind " << "\033[33m\033[1m" << "IPV4 " << "\033[0m" << std::flush;
}

/*
	opening listen for the said port
	eventhough bind on bot from constructor
*/
void	Server::use_to_connect() {
	int check = listen(server_fd, SOMAXCONN);
	if (check == -1)
		throw errorListen();
	std::cout << "listen "<< "\033[33m\033[1m" << "MAXCONN " << "\033[0m" << std::endl;
}

/*
	now we are getting to part with running the server
	everything here is contained in the runServer ft
	run Error doesn't quit, server should be invincible
*/
void	Server::runError(const std::string &msg, const int &fd) const {
	if (fd == 0)
		std::cerr << "Error: 'runServer' " << msg << std::endl;
	else
		std::cerr << "Error: 'runServer' " << msg << " for '" << fd << "'" << std::endl;
}

/*
	basic sending of the message in IRC format
	to the particular user on said file descriptor
*/
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

/*
	accepting new connections and getting User information
	that will be used by the Client handler side of server
*/
void	Server::acceptingClient() {
	struct	sockaddr_in client_info;
	memset(&client_info, 0, sizeof(client_info));
	socklen_t	info_size = sizeof(client_info);
	int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_info), &info_size);
	if (client_fd == -1)
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
			/*
				pass in into .connect the server password
			*/
			clients.connect(client_fd, address, hostname);
			sendMsg("Welcome to our IRC server", client_fd);
		}
	}
}

/*
	debugging function, should be unused in finished server
*/
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

/*
	check if there is any other way than checking errno values
	after getting the result from the recv and etc.
	mainly checking if message should be ignored/buffered/used
	and possibly for disconnecting the users
*/
Server::iter	Server::receivingData(iter it) {
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
	if (buffer.size() > 2) {
		std::string::size_type line_feed = buffer.find("\r\n");
		if (line_feed == std::string::npos)
			clients[it->fd].buffer(buffer.data());
		else {
			int start = 0;
			int offset = 2;
			while (line_feed != std::string::npos) {
				std::string tmp = buffer.substr(start, line_feed - start);
				clients[it->fd].buffer(tmp.data());
				pars.parseAndDispatch(clients[it->fd]);
				start = line_feed + offset;
				line_feed = buffer.find("\r\n", start);
			}
			std::string leftover = buffer.substr(start, buffer.size() - start);
			if (leftover.empty() == false)
				clients[it->fd].buffer(leftover.data());
		}
	}
	buffer.clear();
	return (++it);
}

/*
	checking for the Errors from polling
	and removing unsafe connections
*/
Server::iter	Server::removeError(iter it) {
	std::cout << "Client with fd: '" << it->fd << "' had an error" << std::endl;
	if (close(it->fd) != 0)
		runError("Close on removeError failed", it->fd);
	return (polling.erase(it));
}

/*
	file descriptor failed and wasn't caught earlier
	most likely should never happen, just for poll check
*/
Server::iter	Server::removeInvalid(iter it) {
	std::cout << "File descriptor was never opened or already closed: " << it->fd << std::endl;
	return (polling.erase(it));
}

/*
	removal of disconnected users
	calling Client side handlers
*/
Server::iter	Server::removeDisconnected(iter it) {
	std::cout << "Client with: " << it->fd << " has disconnected" << std::endl;
	clients.disconnect(it->fd);
	if (close(it->fd) != 0)
		runError("Close on disconnection failed", it->fd);
	return (polling.erase(it));
}

/*
	main loop of the server with polling data
	the time out should never happen as -1 is set
	other than that calling proper helper members
*/
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