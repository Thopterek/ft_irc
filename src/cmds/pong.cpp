#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

void pong(Client& client, int fd, const std::vector<std::string>& param)
{
    if (client[fd].getStatus() != RegStatus::REGISTERED)
    {
        client[fd].handleErrors(Errors::ERR_NOTREGISTERED, "");
        return ;
    }
    if (param.empty())
        client[fd].handleErrors(Errors::ERR_NEEDMOREPARAMS, "PONG");

    const std::string serverSource { ":" + client[fd].getServerName() };
    const std::string   serverName { client[fd].getServerName() };
    const std::string response { serverSource + " PONG " +
                                    serverName + " :" + param[0] + "\r\n" };
    client[fd].respond(response);
}

