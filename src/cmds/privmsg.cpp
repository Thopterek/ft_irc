#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

static bool isUser(const std::string& target)
{
    return (std::isalpha(target.at(0)));
}

static bool isChannel(const std::string& target)
{
    return (target.at(0) == '#');
}

static void    directMessage(const std::vector<std::string>& params
                             , int fd, Client& client)
{
    const std::string& target { params.at(0) };
    const std::string& message { params.at(1) };

    auto    users { client.getUsers() };
    for (const auto& user : users)
    {
        // Nicknames are unique in the server.
        if (client.ircCapitalize(target) == 
                client.ircCapitalize(user.second->getNickName()))
        {
            const std::string  targetMessage { target + " :" + message + "\r\n" };
            user.second->respond(client[fd].getSource() + " PRIVMSG " + targetMessage);
            return ;
        }
    }
}
//  TODO: Inform Chris about the channel Privmsg.
static void messageChannel(const std::vector<std::string>& params,
                           int fd, Client& client)
{
    User&   user { client[fd] };
    const std::string&  target { params.at(0) };
    auto    channels { user.getChannels() };
    auto    iter { channels.find(target) };
    if (iter == channels.end())
    {
        user.handleErrors(Errors::ERR_CANNOTSENDTOCHAN, "PRIVMSG");
        return ;
    }
    const std::string  targetMessage { target + " :" + params.at(1) + "\r\n" };
    const std::string   msg { user.getSource() + " PRIVMSG " + targetMessage };
    // iter->second->broadcast(message, &user);
}

void    privmsg(Client& client, int fd, const std::vector<std::string> &params)
{
    User&   user { client[fd] };

    if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "PRIVMSG");
        return ;
    }
    if (params.size() < 1 || params.at(0).empty())
    {
        user.handleErrors(Errors::ERR_NORECIPIENT, "PRIVMSG");
        return ;
    }
    if (params.size() < 2 || params.at(1).empty())
    {
        user.handleErrors(Errors::ERR_NOTEXTTOSEND, "PRIVMSG");
        return ;
    }
    if (isUser(params[0]))
        directMessage(params, fd, client);
    else if (isChannel(params[0]))
        messageChannel(params, fd, client);
    else
        user.handleErrors(Errors::ERR_NORECIPIENT, "PRIVMSG");
}
