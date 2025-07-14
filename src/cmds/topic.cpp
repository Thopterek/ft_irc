#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

void topic(Client& client, int fd, const std::vector<std::string> &param)
{
	for (auto it = client.getAllChannels().begin(); it != client.getAllChannels().end(); ++it)
		std::cout << "THERE WAS THIS MANY CHANNELS" << std::endl;
	std::cout << "\033[33m\033[1m" << "Command: Topic started" << "\033[0m" << std::endl;
	User& user = client[fd];
    if (param.size() < 1)
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
    if (param.size() == 1)
    {
        std::string topic = channel->getTopic();
        if (topic.empty())
        user.handleErrors(Errors::ERR_NOTOPIC, channelName);
        else
        {
            std::string msg = ":" + user.getSource() + " TOPIC " + channelName + " :" + topic + "\r\n";
            user.respond(msg);
        }
        return;
    }
    if (!channel->isOperator(user.getFd()))
        return user.handleErrors(Errors::ERR_CHANOPRIVSNEEDED, channelName);
    std::string newTopic = param[1];
    channel->setTopic(newTopic);
    std::string msg = ":" + user.getSource() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    channel->broadcast(msg, user);
    std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}