#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void part(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];
    if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "");
        return ;
    }
	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PART");

	std::string channelName = param[0];
	std::string leavemsg = param.size() > 1 ? param[1] : user.getNickName();;
	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);
	if (!channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);
	std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + 
        "@" + user.getHostName() + " PART " + channelName + " :" + leavemsg + "\r\n";
	channel->removeMember(user.getFd());
	channel->broadcast(msg, user);
	user.respond(msg);
	if (channel->isEmpty()) {
		client.deleteChannel(channelName);
	}
}
