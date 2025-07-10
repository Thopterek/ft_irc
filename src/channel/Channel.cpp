#include "../../inc/Channel.hpp"

Channel::Channel(std::string name, unsigned int id) : id(id), name(name) {};

void Channel::addMember(unsigned int clientId)
{
	if (inviteOnly && isInvited(clientId) == false)
		return;
	if (isKicked(clientId) == true)
		return;
	size_t channelSize = 0;
	for (const auto &member : _members)
	{
		if (member.second)
			channelSize++;
	}
	if (limit != 0 && channelSize >= static_cast<size_t>(limit))
		return;
	if (isMember(clientId) == true)
		return; // Already a member
	// client->channelId = this->id;

	_members[clientId] = true;
	// send_msg(server, clientId, "JOIN " + name + "\r\n");
}

void	Channel::removeMember(unsigned int clientId)
{
	_members[clientId] = false;
}

std::string Channel::inviteMember(unsigned int clientId)
{
	if (!clientId)
		return "Invalid client ID.";
	if (_members[clientId])
		return "Client is already a member.";
	if (_invites[clientId])
		return "Client is already invited.";
	_invites[clientId] = true;
	_kicked[clientId] = false;
	return "You got invited";
}


void Channel::kick(unsigned int clientId)
{
	removeMember(clientId);
	_kicked[clientId] = true;
	_invites[clientId] = false;
	_operators[clientId] = false;
}
void Channel::unkick(unsigned int clientId)
{
	_kicked[clientId] = false;
}

void Channel::addOperator(unsigned int clientId)
{
	_operators[clientId] = true;
}
void Channel::removeOperator(unsigned int clientId)
{
	_operators[clientId] = false;
}
bool Channel::isOperator(unsigned int clientId)
{
	auto it = _operators.find(clientId);
	if (it == _operators.end())
		return (false);
	return (true);
}

bool Channel::isMember(unsigned int clientId)
{
	auto it = _members.find(clientId);
	if (it == _members.end())
		return (false);
	return (true);
}

std::map<unsigned int, bool> & Channel::getMembers()
{
	return _members;
}

bool Channel::isKicked(unsigned int client)
{
	auto it = _kicked.find(client);
	if (it == _kicked.end())
		return (false);
	return (true);
}

bool Channel::isInvited(unsigned int clientId)
{
	auto it = _invites.find(clientId);
	if (it == _invites.end())
		return (false);
	return (true);
}

// std::string Channel::getInfo()
// {
// 	std::string info = "Channel Name: " + name + "\n";
// 	info += "Channel ID: " + std::to_string(id) + "\n";
// 	info += "Members: ";
// 	for (const auto &member : _members)
// 	{
// 		if (member.second)
// 			info += std::to_string(member.first) + " ";
// 	}
// 	info += "\nOperators: ";
// 	for (const auto &op : _operators)
// 	{
// 		if (op.second)
// 			info += std::to_string(op.first) + " ";
// 	}
// 	return info;
// }


// more server param_type

// for the channal check
// vektor or uniq pointer usaage


// class Server {
// 	private:
// 		std::unordered_map<std::string, std::unique_ptr<Channel>> m_channels;
	
// 	public:
// 		Channel* createChannel(const std::string& name);
// 		void deleteChannel(const std::string& name);
// 	};



// 	Channel* Server::createChannel(const std::string& name) {
// 		auto chan = std::make_unique<Channel>(name, m_channels.size());
// 		Channel* chan_ptr = chan.get(); // Rückgabe als roher Pointer für Verwendung
// 		m_channels[name] = std::move(chan); // Ownership geht in map über
// 		return chan_ptr;
// 	}


// 	#include <vector>

// class Server {
// private:
//     std::vector<Channel> _channels;

// public:
//     Channel& createChannel(const std::string& name);
//     void deleteChannelByName(const std::string& name); // Achtung: langsamer
// };

// Channel& Server::createChannel(const std::string& name) {
//     _channels.emplace_back(name, _channels.size());
//     return _channels.back(); // Gib Referenz auf neu erstellten Channel zurück
// }

// void Server::deleteChannelByName(const std::string& name) {
//     auto it = std::remove_if(_channels.begin(), _channels.end(),
//         [&name](const Channel& c) {
//             return c.getName() == name;
//         });

//     if (it != _channels.end()) {
//         _channels.erase(it, _channels.end());
//     }
// }

// void Channel::send_msg(std::string msg, Server &server, Client *client) {
// 	for (const auto& member : _members) {
// 		if (member.second && (!client || member.first != client->id)) {
// 			send(member.first, msg, server);
// 		}
// 	}
// }


// void Server::part(User* user, const std::string& channelName) {
//     auto it = _channels.find(channelName);
//     if (it != _channels.end()) {
//         it->second->removeUser(user);
//     }
// }

// void Server::quit(User* user) {
//     for (auto& [name, chan] : _channels) {
//         if (chan->hasUser(user)) {
//             chan->removeUser(user);
//         }
//     }
// }

// void Server::kick(User* user, const std::string& channelName) {
//     auto it = _channels.find(channelName);
//     if (it != _channels.end() && it->second->hasUser(user)) {
//         it->second->removeUser(user);
//     }
// }



//if (channel->password != "" && (parts.size() < 3 || parts[2] != channel->password)
//		if (channelName[0] != '#')

// udpate 


// Parser::Parser()
// {
// 	m_handler.emplace("INVITE", handleInvite)
// 	m_handler.emplace("JOIN", handleJoin);
// 	m_handler.emplace("MODE", handleMode);
// 	m_handler.emplace("KICK", handleKick);
//     m_handler.emplace("QUIT", handleQuit);
// }


// void    invite(Client& client, int fd, std::vector<std::string> param)
// {
// 	User&       user { client[fd] };
// }
// void    join(Client& client, int fd, std::vector<std::string> param)
// {
// 	User&       user { client[fd] };
// }
// void    mode(Client& client, int fd, std::vector<std::string> param)
// {
// 	User&       user { client[fd] };
// }
// void    kick(Client& client, int fd, std::vector<std::string> param)
// {
// 	User&       user { client[fd] };
// }
// void    quit(Client& client, int fd, std::vector<std::string> param)
// {
// 	User&       user { client[fd] };

// 	const std::string&  error { user.errors.at(Errors::ERR_NONICKNAMEGIVEN) };
// 	user.respond(user.buildMsg(Errors::ERR_NONICKNAMEGIVEN, "NICK", error));// this line imporatant

// 	//also found this version:
// 	// user.handleErrors(Errors::ERR_REGISTERING, "PASS");
// 	//const std::string&  error { user.errors.at(Errors::ERR_REGISTERING) };
// 	//user.respond(user.buildMsg(Errors::ERR_REGISTERING, "PASS", error));
// 	return ;
// }

/*
	from this part we are taking fts
*/

// void Channel::send_msg(std::string msg, Server& server, Client* sender) {
//     for (std::map<unsigned int, bool>::iterator it = _members.begin(); it != _members.end(); ++it) {
//         if (sender && it->first == sender->getId()) continue;
//         Client* recipient = server.getClientById(it->first);
//         if (recipient)
//             recipient->sendMessage(msg); // oder .respond()
//     }
// }




// std::string Channel::removeMember(unsigned int clientId, Server &server)
// {
//     if (!_members.count(clientId) || !_members[clientId])
//         return "User not in channel.";

//     _members[clientId] = false;

//     // Optional: channelId im User zurücksetzen, wenn du das trackst
//     Client* client = server.getClientById(clientId);
//     if (client) {
//         client->channelId = std::nullopt;  // falls std::optional genutzt
//     }

//     return "";
// }

// std::string Channel::inviteMember(unsigned int clientId, Server &server)
// {
//     if (_members.count(clientId) && _members[clientId])
//         return "Client is already a member.";
//     if (_invites.count(clientId) && _invites[clientId])
//         return "Client is already invited.";

//     _invites[clientId] = true;
//     _kicked[clientId] = false;
//     return "";
// }

void Channel::broadcast(const std::string& msg, User &user) {
	std::string actual = msg + "\r\n";
	for (std::map<unsigned int, bool>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (!it->second)
			continue;
		if (it->first == static_cast<unsigned int>(user.getFd()))
			continue;
		if (send(it->first, actual.c_str(), actual.size(), MSG_DONTWAIT) < 0)
			std::cerr << "Error: send in broadcoast failed" << std::endl;
	}
}

std::string Channel::getPassword()
{
	return password;
}
void Channel::setPassword(std::string newPassword)
{
	password = newPassword;
}


void Channel::setTopic(std::string newTopic)
{
	topic = newTopic;
}
std::string Channel::getTopic()
{
	return topic;
}

void Channel::setLimit(int newLimit)
{
	if (newLimit < 0)
		return ;
	limit = newLimit;
}

int Channel::getLimit() const
{
	return limit;
}
