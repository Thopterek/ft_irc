#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void quit(Client& client, int fd, const std::vector<std::string> &param)
{
	User& user = client[fd];

	const std::string reason = param.empty() ? " " : param[0];
	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end();)
	{
		Channel* chan = it->second.get();
		if (chan->isMember(user.getFd()))
		{
			std::string msg = ":" + user.getNickName() + "!" + 
                user.getUserName() + "@" + user.getHostName() + 
                    " QUIT :" + it->first + reason + "\r\n";
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
	if (close(fd) != 0)
        std::cerr << "Error: close in quit failed" << std::endl;
}

