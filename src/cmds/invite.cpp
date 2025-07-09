#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

void invite(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];

	if (param.size() < 2)
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "INVITE");

	const std::string& targetNick = param[0];
	const std::string& channelName = param[1];

	auto it = client.getUsers().begin();
	while (it != client.getUsers().end()) {
		if (targetNick == it->second->getNickName())
			break;
		++it;
	}
	if (it == client.getUsers().end())
		return user.handleErrors(Errors::ERR_NOSUCHNICK, targetNick);

	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);

	if (!channel->isOperator(user.getFd()))
		return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);

	std::string inviteErr = channel->inviteMember(it->first);
	if (!inviteErr.empty()) {
		user.handleErrors(Errors::ERR_UNKNOWNERROR, "INVITE");
		return;
	}

	std::string msg = ":" + user.getSource() + " INVITE " + targetNick + " :" + channelName + "\r\n";
	it->second->respond(msg);
}
