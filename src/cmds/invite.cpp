#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

void invite(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];
    if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "");
        return ;
    }
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

	if (channel->isMember(it->first))
		return user.handleErrors(Errors::ERR_USERONCHANNEL, targetNick);

	if (channel->isKicked(it->first))
		return user.handleErrors(Errors::ERR_BANNEDFROMCHAN, channelName);

	channel->inviteMember(it->first);

	std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + "@" + 
        user.getHostName() + " INVITE " + targetNick + " :" + channelName + "\r\n";
	it->second->respond(msg);
	std::string replyToInviter = ":" + user.getServerName() + " 341 " + 
        user.getNickName() + " " + targetNick + " " + channelName + "\r\n";
	user.respond(replyToInviter);
}

