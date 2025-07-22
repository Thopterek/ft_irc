#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

void join(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];
	if (user.getStatus() != RegStatus::REGISTERED)
		return user.handleErrors(Errors::ERR_NOTREGISTERED, "JOIN");
	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "JOIN");
	std::string channelName = param[0];
	std::string password = (param.size() > 1) ? param[1] : "";
	Channel* channel = client.getChannelByName(channelName);
	if (channel == NULL) {
		channel = client.createChannel(channelName, user.getFd());
		channel->addOperator(user.getFd());
	}
	if (channel->isKicked(user.getFd())) {
		return user.handleErrors(Errors::ERR_BANNEDFROMCHAN, channelName);
	}
	if (channel->isInviteOnly() && !channel->isInvited(user.getFd()))
		return user.handleErrors(Errors::ERR_INVITEONLYCHAN, channelName);
	if (!channel->getPassword().empty() && channel->getPassword() != password)
		return user.handleErrors(Errors::ERR_BADCHANNELKEY, channelName);
	if ((!(static_cast<size_t>(channel->getLimit()) < channel->getMembers().size()) && 
        !channel->isOperator(user.getFd())) || channel->getLimit() != 0)
		return user.handleErrors(Errors::ERR_CHANNELISFULL, channelName);
	if (channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_USERONCHANNEL, channelName);
	channel->addMember(user.getFd());
	std::string topic = channel->getTopic();
	if (topic.empty())
		channel->setTopic(": No topic set");
	std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + 
        "@" + user.getHostName() + " JOIN :" + channelName + "\r\n";
	channel->broadcast(msg, user);
	std::string msg2 = ":" + user.getUserName() + " 332 " + user.getNickName() + 
        " " + channelName + " :" + channel->getTopic() + "\r\n";
	user.respond(msg);
	user.respond(msg2);
}
