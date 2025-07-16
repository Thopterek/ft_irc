#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

void topic(Client& client, int fd, const std::vector<std::string> &param)
{
	std::cout << "\033[33m\033[1m" << "Command: Topic started" << "\033[0m" << std::endl;
	User& user = client[fd];
    if (param.empty())
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "TOPIC");
        return;
    }
    std::string channelName = param[0];
    Channel* channel = client.getChannelByName(channelName);
    if (channel == NULL)
    {
        user.handleErrors(Errors::ERR_NOSUCHCHANNEL, channelName);
        return;
    }
    std::cout << "Channel found" << std::endl;
	if (channel->isMember(user.getFd()) == false)
		return user.handleErrors(Errors::ERR_USERONCHANNEL, channelName);
    std::cout << "is found in member" << std::endl;
    if (param.size() == 1)
    {
        std::cout << "in on param" << std::endl;
        std::string topic = channel->getTopic();
        if (topic.empty())
            user.handleErrors(Errors::ERR_NOTOPIC, channelName);
        else
        {
            std::string msg = ":" + user.getNickName() + " 332 " + user.getUserName() + " " + channelName + " :" + topic + "\r\n";
            user.respond(msg);
        }
        return;
    }
    if (!channel->isOperator(user.getFd()))
        return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);
    std::string newTopic = param[1];
    channel->setTopic(newTopic);
    std::string msg = ":" + user.getUserName() + " 332 " + user.getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n";
    channel->broadcast(msg, user);
    user.respond(msg);
    std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}