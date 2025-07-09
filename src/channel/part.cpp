#include "../client/User.hpp"
#include "../channel/Channel.hpp"

void part(Client& client, int fd, std::vector<std::string> param)
{
	User& user = client[fd];

	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PART");

	std::string channelName = param[0];
	Channel* channel = server.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isMember(user.id))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);

	std::string msg = ":" + user.getSource() + " PART :" + channelName + "\r\n";
	channel->broadcast(msg, server, nullptr);
	user.respond(msg);

	channel->removeMember(user.id, server);
	if (channel->getMembers().empty())
		server.deleteChannel(channelName);
}
