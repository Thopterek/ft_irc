#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

//seem to need printing topic if pressent 
// JOIN #foo 123PW                 ; Command to join channels #foo and
// #bar.

// JOIN 0                          ; Leave all currently joined
// channels.
//also what about password?



void join(Client& client, int fd, const std::vector<std::string> &param)
{
	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end(); ++it)
		std::cout << "THERE WAS THIS MANY CHANNELS" << std::endl;
	std::cout << "\033[33m\033[1m" << "Command: JOIN started" << "\033[0m" << std::endl;
	User& user = client[fd];

	if (user.getStatus() != RegStatus::REGISTERED)
		return user.handleErrors(Errors::ERR_NOTREGISTERED, "JOIN");

	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "JOIN");

	std::string channelName = param[0];
	std::string password = (param.size() > 1) ? param[1] : "";
	std::cout << "we are before getting the channel pointer" << std::endl;
	Channel* channel = client.getChannelByName(channelName);///
	if (channel == NULL) {
		std::cout << "checkdouble channe; case" << std::endl;
		channel = client.createChannel(channelName, user.getFd());
		channel->addOperator(user.getFd());
	}
	std::cout << "checking if he is kicked" << std::endl;
	if (channel->isKicked(user.getFd())) {
		std::cout << "are we going into user.handle" << std::endl;
		return user.handleErrors(Errors::ERR_BANNEDFROMCHAN, channelName);
	}

	if (channel->isInviteOnly() && !channel->isInvited(user.getFd()))
		return user.handleErrors(Errors::ERR_INVITEONLYCHAN, channelName);
	std::cout << "next thing is checking the password" << std::endl;
	if (!channel->getPassword().empty() && channel->getPassword() != password)
		return user.handleErrors(Errors::ERR_BADCHANNELKEY, channelName);
	std::cout << "we gonna go for static-cast to check the limits" << std::endl;
	std::cout << "channel limit is: " << channel->getLimit() << std::endl;
	std::cout << "channel members size is: " << channel->getMembers().size() << std::endl;
	if ((!(static_cast<size_t>(channel->getLimit()) < channel->getMembers().size()) && !channel->isOperator(user.getFd())) || channel->getLimit() != 0)
		return user.handleErrors(Errors::ERR_CHANNELISFULL, channelName);
    std::cout << "is member check" << std::endl;
	if (channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_USERONCHANNEL, channelName);
	std::cout << "is addmember" << std::endl;
	channel->addMember(user.getFd());

	std::string msg = ":" + user.getSource() + " JOIN :" + channelName + "\r\n";
	channel->broadcast(msg, user);
	user.respond(msg);
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
