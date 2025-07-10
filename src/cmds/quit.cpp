#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void quit(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: QUIT started" << "\033[0m" << std::endl;
	User& user = client[fd];

	const std::string reason = param.empty() ? "Client Quit" : param[0];
	const std::string msg = ":" + user.getSource() + " QUIT :" + reason + "\r\n";

	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end();)
	{
		Channel* chan = it->second.get();
		if (chan->isMember(user.getFd()))
		{
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
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
	// user.respond(msg); // optional: send to self
}
