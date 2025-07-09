#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void part(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];

	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PART");

	std::string channelName = param[0];
	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);

	std::string msg = ":" + user.getSource() + " PART :" + channelName + "\r\n";
	channel->broadcast(msg, user);
	// user.respond(msg);

	channel->removeMember(user.getFd());
	if (channel->getMembers().empty())
		client.deleteChannel(channelName);
}
