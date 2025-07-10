#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


// void    join(Client& client, int fd, const std::vector<std::string> &param)
// {
// 	User&       user { client[fd] };
//     if (param.size() < 2)
//     {
//         user.sendCodeResponse(461, "Not enough parameters", "JOIN");
//         return;
//     }
//     if (!channel)
//     {

//         Channel& newChannel = server.createChannel(chanName, user.getId());
//         newChannel.addOperator(user.getId());
//         newChannel.addMember(user.getId(), server);

//         std::string msg = ":" + user.getFullPrefix() + " JOIN :" + chanName + "\r\n";
//         newChannel.send_msg(msg, server, NULL);
//         return;
//     }
//     std::string channelName = param[1];
//     Server& server = user.getServer();
//     Channel* channel = server.getChannelByName(channelName);
//     if (!channel)
//     {
//         user.sendCodeResponse(403, "No such channel", channelName);
//         return;
//     }
//     if (channel->inviteOnly && !channel->isOperator(user.id) && !channel->isInvited(user.id))
//     {
//         user.sendCodeResponse(473, "Cannot join channel (+i)", channelName);
//         return;
//     }
//     if (channel->isKicked(user.id))
//     {
//         user.sendCodeResponse(474, "Cannot join channel (+b)", channelName);
//         return;
//     }
//     if (channel->password != "" && (param.size() < 3 || param[2] != channel->password))
//     {
//         user.sendCodeResponse(475, "Cannot join channel (+k)", channelName);
//         return;
//     }
//     channel->addMember(user.id, server);
//     user.channelId = channel->id;
//     user.respond(331, "Channel joined", channelName); // RPL_NOTOPIC
//     channel->send_msg(":" + user.getNickname() + " JOIN " + channelName + "\r\n", server, &user);
// }



// void handleJoin(Client& client, const std::vector<std::string>& args, Server& server) {
//     if (!client.isAuthenticated) {
//         client.sendCodeResponse(451, "You have not registered", "JOIN");
//         return;
//     }

//     if (args.size() < 2) {
//         client.sendCodeResponse(461, "Not enough parameters", "JOIN");
//         return;
//     }

//     std::string channelName = args[1];
//     std::string password = (args.size() > 2) ? args[2] : "";

//     Channel* channel = server.getChannelByName(channelName);
//     if (!channel) {
//         channel = &server.createChannel(channelName, client.id);
//         channel->addOperator(client.id);
//     }

//     // CHECKS
//     if (channel->isKicked(client.id)) {
//         client.sendCodeResponse(474, "Banned from channel", channelName);
//         return;
//     }

//     if (channel->isInviteOnly() && !channel->isInvited(client.id)) {
//         client.sendCodeResponse(473, "Cannot join channel (+i)", channelName);
//         return;
//     }

//     if (!channel->getPassword().empty() && channel->getPassword() != password) {
//         client.sendCodeResponse(475, "Cannot join channel (+k)", channelName);
//         return;
//     }

//     if (channel->isFull() && !channel->isOperator(client.id)) {
//         client.sendCodeResponse(471, "Cannot join channel (+l)", channelName);
//         return;
//     }

//     if (channel->isMember(client.id)) {
//         client.sendCodeResponse(443, "You're already in channel", channelName);
//         return;
//     }

//     // JOIN PROZESS
//     channel->addMember(client.id, server);

//     std::string joinMsg = ":" + client.getFullPrefix() + " JOIN :" + channelName + "\r\n";
//     channel->broadcast(joinMsg, server, &client); // an andere

//     // an den Client selbst: JOIN-Bestätigung + ggf. TOPIC + NAMES
//     std::string response;

//     if (!channel->getTopic().empty())
//         response += ":" + server.getName() + " 332 " + client.nickname + " " + channelName + " :" + channel->getTopic() + "\r\n";

//     std::string names;
//     for (auto it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
//         if (it->second) {
//             Client* member = server.getClientById(it->first);
//             if (member) names += member->nickname + " ";
//         }
//     }

//     if (!names.empty()) names.pop_back(); // letztes Leerzeichen weg
//     response += ":" + server.getName() + " 353 " + client.nickname + " = " + channelName + " :" + names + "\r\n";
//     response += ":" + server.getName() + " 366 " + client.nickname + " " + channelName + " :End of /NAMES list\r\n";

//     client.sendMessage(joinMsg + response); // an sich selbst
// }


// void join(Client& client, int fd, std::vector<std::string> param)
// {
//     User& user = client[fd];
//     if (!user.isAuthenticated)
//         return user.sendCodeResponse(451, "You have not registered", "JOIN");
//     if (param.size() < 1)
//         return user.sendCodeResponse(461, "Not enough parameters", "JOIN");

//     std::string name = param[0];
//     std::string pass = (param.size() > 1) ? param[1] : "";

//     Channel* chan = server.getChannelByName(name);
//     if (!chan) {
//         chan = &server.createChannel(name, user.id);
//         chan->addOperator(user.id);
//     }

//     if (chan->isKicked(user.id))
//         return user.sendCodeResponse(474, "Banned from channel", name);
//     if (chan->isInviteOnly() && !chan->isInvited(user.id))
//         return user.sendCodeResponse(473, "Invite only", name);
//     if (!chan->getPassword().empty() && chan->getPassword() != pass)
//         return user.sendCodeResponse(475, "Wrong password", name);
//     if (chan->isFull() && !chan->isOperator(user.id))
//         return user.sendCodeResponse(471, "Channel full", name);
//     if (chan->isMember(user.id))
//         return user.sendCodeResponse(443, "Already in channel", name);

//     chan->addMember(user.id, server);

//     std::string msg = ":" + user.getFullPrefix() + " JOIN :" + name + "\r\n";
//     chan->broadcast(msg, server, &user);

//     user.sendMessage(msg);
// }




void join(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: JOIN started" << "\033[0m" << std::endl;
	User& user = client[fd];

	if (user.getStatus() != RegStatus::REGISTERED)
		return user.handleErrors(Errors::ERR_NOTREGISTERED, "JOIN");

	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "JOIN");

	std::string channelName = param[0];
	std::string password = (param.size() > 1) ? param[1] : "";
	std::cout << "we are before getting the channel pointer" << std::endl;
	Channel* channel = client.getChannelByName(channelName);
	if (channel == NULL) {
		channel = client.createChannel(channelName, user.getFd());
		channel->addOperator(user.getFd());
	}
	std::cout << "checking if he is kicked" << std::endl;
	if (channel->isKicked(user.getFd())) {
		std::cout << "are we going into user.handle" << std::endl;
		return user.handleErrors(Errors::ERR_BANNEDFROMCHAN, channelName);
	}

	// if (channel->isInviteOnly() && !channel->isInvited(user.getFd()))
	// 	return user.handleErrors(Errors::ERR_INVITEONLYCHAN, channelName);
	std::cout << "next thing is checking the password" << std::endl;
	if (!channel->getPassword().empty() && channel->getPassword() != password)
		return user.handleErrors(Errors::ERR_BADCHANNELKEY, channelName);
	std::cout << "we gonna go for static-cast to check the limits" << std::endl;
	if (static_cast<size_t>(channel->getLimit()) <= channel->getMembers().size() && !channel->isOperator(user.getFd()))
		return user.handleErrors(Errors::ERR_CHANNELISFULL, channelName);

	if (channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_USERONCHANNEL, channelName);

	channel->addMember(user.getFd());

	std::string msg = ":" + user.getSource() + " JOIN :" + channelName + "\r\n";
	channel->broadcast(msg, user);
	user.respond(msg);
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
