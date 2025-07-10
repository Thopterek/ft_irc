#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void mode(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: MODE started" << "\033[0m" << std::endl;
	User& user = client[fd];

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
	
		switch (c) {
			// case 'i': channel->setInviteOnly(set); break;
			// case 't': channel->setTopicProtection(set); break;
			case 'k':
				if (set && param.size() > 2) channel->setPassword(param[2]);
				else if (!set) channel->setPassword(""); // Passwort löschen
				break;
			case 'l':
				if (set && param.size() > 2) channel->setLimit(std::atoi(param[2].c_str()));
				else if (!set) channel->setLimit(0); // Limit deaktivieren
				break;
			default:
				return user.handleErrors(Errors::ERR_UNKNOWNMODE, "MODE"/*std::string(1, c)*/);
		}
	}

	std::string msg = ":" + user.getSource() + " MODE " + channelName + " +" + modes + "\r\n";
	channel->broadcast(msg, user);
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
