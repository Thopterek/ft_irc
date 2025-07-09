#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void quit(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];

	const std::string reason = param.empty() ? "Client Quit" : param[0];
	const std::string msg = ":" + user.getSource() + " QUIT :" + reason + "\r\n";

	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end(); ++it)
	{
		Channel* chan = it->second;

		if (chan->isMember(user.getFd()))
		{
			chan->broadcast(msg, user);
			chan->removeMember(user.getFd());
			if (chan->getMembers().empty())
				client.deleteChannel(chan->name);
		}
	}

	// user.respond(msg); // optional: send to self
}
