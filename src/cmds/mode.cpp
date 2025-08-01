#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

static std::string	getMemberNick(std::string TargetNick, Client& client) {
	auto it = client.getUsers().begin();
	while (it != client.getUsers().end()) {
		if (TargetNick == it->second->getNickName())
			return (TargetNick);
		++it;
	}
	return ("");
}

static int	getMemberFd(std::string TargetNick, Client& client) {
	auto it = client.getUsers().begin();
	while (it != client.getUsers().end()) {
		if (TargetNick == it->second->getNickName())
			return (it->first);
		++it;
	}
	return (0);
}

void mode(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];
    if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "");
        return ;
    }
	if (param.size() < 2)
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "MODE");

	const std::string& channelName = param[0];
	const std::string& modes = param[1];
	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isOperator(user.getFd()))
		return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);

	bool set = true;

	for (size_t i = 0; i < modes.size(); ++i) {
		char c = modes[i];
		if (c == '+') { set = true; continue; }
		if (c == '-') { set = false; continue; }
		if ((modes[i] == 'i' || modes[i] == 't' || modes[i] == 'k' || 
            modes[i] == 'l' || modes[i] == 'o') && modes.size() > 2)
			return user.handleErrors(Errors::ERR_UNKNOWNMODE, "MODE");
		switch (c) {
			case 'i': channel->setInviteOnly(set); break;
			case 't': channel->setTopicProtection(set); break;
			case 'k':
				if (set && param.size() > 2) channel->setPassword(param[2]);
				else if (!set) channel->setPassword("");
				break;
			case 'l':
				if (set && param.size() > 2) channel->setLimit(std::atoi(param[2].c_str()));
				else if (!set) channel->setLimit(0);
				break;
			case 'o':
				if(set && param.size() > 2 && getMemberNick(param[2], client) != "" && 
                    !channel->isKicked(getMemberFd(param[2], client)) &&
                        channel->isMember(getMemberFd(param[2], client)))
					channel->addOperator(getMemberFd(param[2], client));
				else if (!set && param.size() > 2 && getMemberNick(param[2], client) != "" && 
                        !channel->isKicked(getMemberFd(param[2], client)) && 
                            channel->isMember(getMemberFd(param[2], client))) 
					channel->removeOperator(getMemberFd(param[2], client));
				break;
			default:
				return user.handleErrors(Errors::ERR_UNKNOWNMODE, "MODE");
		}
	}
	std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + 
        "@" + user.getHostName() + " MODE " + channelName + " " + modes + "\r\n";
	channel->broadcast(msg, user);
	user.respond(msg);
}
