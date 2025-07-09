#include "../client/User.hpp"
#include "../channel/Channel.hpp"

void invite(Client& client, int fd, std::vector<std::string> param)
{
	User& user = client[fd];

	if (param.size() < 2)
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "INVITE");

	const std::string& targetNick = param[0];
	const std::string& channelName = param[1];

	User* target = server.getClientByNick(targetNick);
	if (!target)
		return user.handleErrors(Errors::ERR_NOSUCHNICK, targetNick);

	Channel* channel = server.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isMember(user.id))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);

	if (!channel->isOperator(user.id))
		return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);

	std::string inviteErr = channel->inviteMember(target->id, server);
	if (!inviteErr.empty()) {
		user.respond(user.buildMsg(Errors::ERR_UNKNOWNERROR, "INVITE", inviteErr));
		return;
	}

	std::string msg = ":" + user.getSource() + " INVITE " + targetNick + " :" + channelName + "\r\n";
	target->respond(msg);
	user.respond(msg);
}
