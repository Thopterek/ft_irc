#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

static void pong(User&  user, const std::string& token)
{
    const std::string serverSource { ":" + user.getServerName() };
    const std::string   serverName { user.getServerName() };
    const std::string response { serverSource + " PONG " +
                                    serverName + " :" + token + "\r\n" };
    user.respond(response);
}

void    ping(Client& client, int fd, const std::vector<std::string>& param)
{
    User&   user { client[fd] };
    if (user.getStatus() != RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_NOTREGISTERED, "PING");
        return ;
    }
    if (param.empty() || param.at(0).empty())
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PING");
        return ;
    }
    pong(user, param.at(0));
}

