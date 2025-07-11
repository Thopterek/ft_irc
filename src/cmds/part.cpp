#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"


void part(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: PART started" << "\033[0m" << std::endl;
	User& user = client[fd];

	if (param.empty())
		return user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PART");

	std::string channelName = param[0];
	Channel* channel = client.getChannelByName(channelName);
	if (!channel)
		return user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);

	if (!channel->isMember(user.getFd()))
		return user.handleErrors(Errors::ERR_NOTONCHANNEL, channelName);

	std::string msg = ":" + user.getSource() + " PART :" + channelName + "\r\n";
	channel->broadcast(msg, user);
	// user.respond(msg);

	channel->removeMember(user.getFd());
	if (channel->getMembers().empty())
		client.deleteChannel(channelName);
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}

// If a "Part Message" is given, this will be sent
//    instead of the default message, the nickname.  This request is always
//    granted by the server.


//    PART #twilight_zone             ; Command to leave channel
//                                    "#twilight_zone"

//    PART #twilight_zone : bye            ; Command to leave channel with message i gues.
//                                          "#twilight_zone"

// 	boradcast									 :WiZ!jto@tolsun.oulu.fi PART #playzone :I lost
// 												 ; User WiZ leaving channel
// 												 "#playzone" with the message "I
// 												 lost".