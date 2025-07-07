#include "../inc/Channel.hpp"
#include <sys/socket.h> 

Channel::Channel(std::string name, unsigned int id) : id(id), name(name) {};

void Channel::addMember(unsigned int clientId, Server &server)
{
	if (inviteOnly && !_invites[clientId])
		return;
	if (_kicked[clientId])
		return;
	size_t channelSize = 0;
	for (const auto &member : _members)
	{
		if (member.second)
			channelSize++;
	}
	if (limit != 0 && channelSize >= limit)
		return;
	if (_members[clientId])
		return; // Already a member
	client->channelId = this->id;

	_members[clientId] = true;
	send_msg(server, clientId, "JOIN " + name + "\r\n");

	in add addMember
	just setting perminssion
	_members true
	_invites true i mean why not 
	_kicked false ignore?
	_operators[clientId] = false;
	std::map<unsigned int, bool> _members;
	std::map<unsigned int, bool> _kicked;
	std::map<unsigned int, bool> _operators;
	std::map<unsigned int, bool> _invites;
}

std::string Channel::removeMember(unsigned int clientId, Server &server)
{
	_members[clientId] = false;
}

std::string Channel::inviteMember(unsigned int clientId, Server &server)
{
	if (!clientId)
		return "Invalid client ID.";
	if (_members)
		return "Client is already a member.";
	if (_invites[clientId])
		return "Client is already invited.";
	_invites[clientId] = true;
	_kicked[clientId] = false;
}


void Channel::kick(unsigned int clientId, Server &server)
{
	removeMember(clientId, server);
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
	return _operators[clientId];
}

std::map<unsigned int, bool> & Channel::getMembers()
{
	return _members;
}

std::string Channel::getInfo()
{
	std::string info = "Channel Name: " + name + "\n";
	info += "Channel ID: " + std::to_string(id) + "\n";
	info += "Members: ";
	for (const auto &member : _members)
	{
		if (member.second)
			info += std::to_string(member.first) + " ";
	}
	info += "\nOperators: ";
	for (const auto &op : _operators)
	{
		if (op.second)
			info += std::to_string(op.first) + " ";
	}
	return info;
}


more server param_type

for the channal check
vektor or uniq pointer usaage


class Server {
	private:
		std::unordered_map<std::string, std::unique_ptr<Channel>> m_channels;
	
	public:
		Channel* createChannel(const std::string& name);
		void deleteChannel(const std::string& name);
	};



	Channel* Server::createChannel(const std::string& name) {
		auto chan = std::make_unique<Channel>(name, m_channels.size());
		Channel* chan_ptr = chan.get(); // Rückgabe als roher Pointer für Verwendung
		m_channels[name] = std::move(chan); // Ownership geht in map über
		return chan_ptr;
	}
	
	void Server::deleteChannel(const std::string& name) {
		m_channels.erase(name); // unique_ptr sorgt automatisch für delete
	}


	#include <vector>

class Server {
private:
    std::vector<Channel> _channels;

public:
    Channel& createChannel(const std::string& name);
    void deleteChannelByName(const std::string& name); // Achtung: langsamer
};

Channel& Server::createChannel(const std::string& name) {
    _channels.emplace_back(name, _channels.size());
    return _channels.back(); // Gib Referenz auf neu erstellten Channel zurück
}

void Server::deleteChannelByName(const std::string& name) {
    auto it = std::remove_if(_channels.begin(), _channels.end(),
        [&name](const Channel& c) {
            return c.getName() == name;
        });

    if (it != _channels.end()) {
        _channels.erase(it, _channels.end());
    }
}

void Channel::send_msg(std::string msg, Server &server, Client *client) {
	for (const auto& member : _members) {
		if (member.second && (!client || member.first != client->id)) {
			send(member.first, msg, server);
		}
	}
}


void Server::part(User* user, const std::string& channelName) {
    auto it = _channels.find(channelName);
    if (it != _channels.end()) {
        it->second->removeUser(user);
    }
}

void Server::quit(User* user) {
    for (auto& [name, chan] : _channels) {
        if (chan->hasUser(user)) {
            chan->removeUser(user);
        }
    }
}

void Server::kick(User* user, const std::string& channelName) {
    auto it = _channels.find(channelName);
    if (it != _channels.end() && it->second->hasUser(user)) {
        it->second->removeUser(user);
    }
}






udpate 


Parser::Parser()
{
	m_handler.emplace("INVITE", handleInvite)
	m_handler.emplace("JOIN", handleJoin);
	m_handler.emplace("MODE", handleMode);
	m_handler.emplace("KICK", handleKick);
    m_handler.emplace("QUIT", handleQuit);
}


void    invite(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };
}
void    join(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };
}
void    mode(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };
}
void    kick(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };
}
void    quit(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };

	const std::string&  error { user.errors.at(Errors::ERR_NONICKNAMEGIVEN) };
	user.respond(user.buildMsg(Errors::ERR_NONICKNAMEGIVEN, "NICK", error));// this line imporatant

	//also found this version:
	// user.handleErrors(Errors::ERR_REGISTERING, "PASS");
	//const std::string&  error { user.errors.at(Errors::ERR_REGISTERING) };
	//user.respond(user.buildMsg(Errors::ERR_REGISTERING, "PASS", error));
	return ;
}






Channel* getChannelByName(const std::string& name) {
	std::map<std::string, Channel>::iterator it = channels.find(name);
	if (it != channels.end())
		return &it->second;
	return NULL;
}

Channel& createChannel(const std::string& name, unsigned int clientId) {
	channels[name] = Channel(name, clientId);
	return channels[name];
}

std::map<std::string, Channel>& getAllChannels() {
	return channels;
}

void Channel::send_msg(std::string msg, Server& server, Client* sender) {
    for (std::map<unsigned int, bool>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (sender && it->first == sender->getId()) continue;
        Client* recipient = server.getClientById(it->first);
        if (recipient)
            recipient->sendMessage(msg); // oder .respond()
    }
}