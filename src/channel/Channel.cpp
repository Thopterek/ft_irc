#include "../inc/Channel.hpp"

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
	

}

std::string Channel::removeMember(unsigned int clientId, Server &server)
{

}

std::string Channel::inviteMember(unsigned int clientId, Server &server)
{

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

}