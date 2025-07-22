#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void quit(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: QUIT started" << "\033[0m" << std::endl;
	User& user = client[fd];

	const std::string reason = param.empty() ? " " : param[0];
	std::cout << reason << std::endl;
	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end();)
	{
		Channel* chan = it->second.get();
		std::cout << "Checking channel: " << chan->name << std::endl;
		if (chan->isMember(user.getFd()))
		{
			std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " QUIT :" + it->first + reason + "\r\n";
			chan->broadcast(msg, user);
			chan->removeMember(user.getFd());
			if (chan->getMembers().empty()) {
				client.deleteChannel(chan->name);
				it = client.getAllChannels().erase(it);
				continue;
			}
		}
		++it;
	}
	close(fd);
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
