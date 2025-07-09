#include "../client/User.hpp"
#include "../channel/Channel.hpp"

void quit(Client& client, int fd, std::vector<std::string> param)
{
	User& user = client[fd];

	const std::string reason = param.empty() ? "Client Quit" : param[0];
	const std::string msg = ":" + user.getSource() + " QUIT :" + reason + "\r\n";

	for (std::map<std::string, Channel>::iterator it = server.getChannels().begin(); it != server.getChannels().end(); ++it)
	{
		Channel& chan = it->second;

		if (chan.isMember(user.id))
		{
			chan.broadcast(msg, server, &user);
			chan.removeMember(user.id, server);
			if (chan.getMembers().empty())
				server.deleteChannel(chan.name);
		}
	}

	user.respond(msg); // optional: send to self
}
