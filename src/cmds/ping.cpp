#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

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
    pong(client, fd, param);
}

