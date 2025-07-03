#include "../client/User.hpp"

void    directMessage(const std::string& target,
                      const std::string& message,
                      int fd, Client& client)
{
    std::unordered_map<int, Users*> users { client.getUsers };
    for (auto user : users)
    {
        if (client.ircCapitalize(target) == client.ircCapitalize(user->getNickName()))
        {
            user->respond(client[fd]->getSource() +
                            " PRIVMSG " + target + " :" + message);
            return ;
        }
    }
}

void    privmsg(Client& client, int fd, std::vector<std::string> params)
{
    User&   user { client[fd] };

    if (user.getStatus != RegStatus::REGISTERED)
    {
        handleError(Errors::ERR_NOTREGISTERED, "PRIVMSG");
        return ;
    }
    if (params.size() != 2 || params.at(0).empty())
    {
        handleError(Errors::ERR_NORECIPIENT, "PRIVMSG");
        return ;
    }
    if (params.at(1).empty())
    {
        handleError(Errors::ERR_NOTEXTTOSEND, "PRIVMSG");
        return ;
    }
    if (std::isalpha(params[0].[0]))
    {
        const std::string&  target { params[0] };
        directMessage(target, params[1], fd, client);
    }
    else
        messageChannel(params);
}
