#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void part(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: PART started" << "\033[0m" << std::endl;
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
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
