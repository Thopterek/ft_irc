#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void kick(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];
	if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "");
        return ;
    }
	if (param.size() < 2)
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "KICK");
	const std::string& channelName = param[0];
	const std::string& targetNick = param[1];

	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isOperator(user.getFd()))
		return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);
	auto it = client.getUsers().begin();
	while (it != client.getUsers().end()) {
		if (targetNick == it->second->getNickName())
			break;
		++it;
	}
	if (it == client.getUsers().end())
		return user.handleErrors(Errors::ERR_NOSUCHNICK, targetNick);

	if (!channel->isMember(it->first))
		return user.handleErrors(Errors::ERR_USERNOTINCHANNEL, targetNick);

	std::string msg = ":" + user.getSource() + " KICK " + 
        channelName + " " + targetNick + " :Kicked\r\n";
	channel->broadcast(msg, user);
	channel->kick(it->first);
	if (channel->getMembers().empty())
		client.deleteChannel(channelName);
}

