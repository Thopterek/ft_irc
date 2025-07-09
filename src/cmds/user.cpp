#include "../client/User.hpp"
#include "../client/Parser.hpp"

void    user(Client& client, int fd, const std::vector<std::string> &param)
{
    User&    user { client[fd] };
    
    if (user.getNickName().empty())
    {
        user.respond("ERROR :You must send NICK command before USER.\r\n");
        return ;
    }
    if (param.size() != 4 || param.front().empty())
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "USER");
        return ;
    }
   if (!user.getUserName().empty())
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "USER");
        return ;
    }
    user.setUserName(param[0]);
    user.setHostName(param.at(1));
    // user.setServerName(param.at(2));
    user.setRealName(param.at(3));
    user.setStatus(RegStatus::REGISTERED);
    user.respond(":server 001 " + user.getNickName() + " :Welcome to the network Network, " + user.getNickName() + "\r\n");
}
