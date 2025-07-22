#include "../../inc/Channel.hpp"

Channel::Channel(std::string set_name, unsigned int set_id) : id(set_id), name(set_name) {};

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
		return;
	_members[clientId] = true;
}

void	Channel::removeMember(unsigned int clientId)
{
	auto check_member = _members.find(clientId);
	if (check_member == _members.end())
		return ;
	_members[clientId] = false;
	_invites[clientId] = false;
}

void	Channel::inviteMember(unsigned int clientId)
{
	_invites[clientId] = true;
}


void Channel::kick(unsigned int clientId)
{
	removeMember(clientId);
	_kicked[clientId] = true;
	auto check_invite = _invites.find(clientId);
	if (check_invite == _invites.end())
		return ;
	_invites[clientId] = false;
	auto check_op = _operators.find(clientId);
	if (check_op == _operators.end())
		return ;
	_operators[clientId] = false;
}
void Channel::unkick(unsigned int clientId)
{
	auto check_kick = _kicked.find(clientId);
	if (check_kick == _kicked.end())
		return ;
	_kicked[clientId] = false;
}

void Channel::addOperator(unsigned int clientId)
{
	_operators[clientId] = true;
}
void Channel::removeOperator(unsigned int clientId)
{
	auto check_op = _operators.find(clientId);
	if (check_op == _operators.end()) {
		return;
	}
	_operators[clientId] = false;
}

bool Channel::isOperator(unsigned int clientId)
{
	auto it = _operators.find(clientId);
	if (it == _operators.end())
		return (false);
	if (it->second == false)
		return (false);
	return (true);
}

bool Channel::isMember(unsigned int clientId)
{
	auto it = _members.find(clientId);
	if (it == _members.end())
		return (false);
	if (it->second == false)
		return (false);
	return (true);
}

bool	Channel::isEmpty() {
	auto check_member = _members.begin();
	for (; check_member != _members.end(); ++check_member) {
		if (check_member->second == true)
			return (false);
	}
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
	if (it->second == false)
		return (false);
	return (true);
}

bool Channel::isInvited(unsigned int clientId)
{
	auto it = _invites.find(clientId);
	if (it == _invites.end())
		return (false);
	if (inviteOnly == true && it->second == false)
		return (false);
	return (true);
}

bool Channel::isTopicProtected() const
{
	return anyoneCanChangeTopic;
}
void Channel::setTopicProtection(bool newTopic)
{
	this->anyoneCanChangeTopic = newTopic;
}
bool Channel::setInviteOnly(bool newinviteOnly) {
	this->inviteOnly = newinviteOnly;
	return this->inviteOnly;
}
bool Channel::isInviteOnly() const {
	return inviteOnly;
}

//right now no protection case for use with privmsg
void Channel::broadcast(const std::string& msg, User &user) {
	std::string actual = msg + "\r\n";
	for (std::map<unsigned int, bool>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (!it->second)
			continue;
		if (it->first == static_cast<unsigned int>(user.getFd()))
			continue;
		if (send(it->first, actual.c_str(), actual.size(), MSG_DONTWAIT) < 0)
		{
			std::cerr << "Error: send in broadcoast failed" << std::endl;
			perror("send");
		}
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
