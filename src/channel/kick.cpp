#include "../client/User.hpp"
#include "../channel/Channel.hpp"

void kick(Client& client, int fd, std::vector<std::string> param)
{
	User& user = client[fd];

	if (param.size() < 2)
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "KICK");

	const std::string& channelName = param[0];
	const std::string& targetNick = param[1];

	Channel* channel = server.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isOperator(user.id))
		return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);

	User* target = server.getClientByNick(targetNick);
	if (!target)
		return user.handleErrors(Errors::ERR_NOSUCHNICK, targetNick);

	if (!channel->isMember(target->id))
		return user.handleErrors(Errors::ERR_USERNOTINCHANNEL, targetNick);

	std::string msg = ":" + user.getSource() + " KICK " + channelName + " " + targetNick + " :Kicked\r\n";
	channel->broadcast(msg, server, nullptr);
	target->respond(msg);

	channel->kick(target->id, server);
	if (channel->getMembers().empty())
		server.deleteChannel(channelName);
}
