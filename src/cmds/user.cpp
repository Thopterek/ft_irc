#include "../client/User.hpp"

void    user(Client& client, int fd, std::vector<std::string> param)
{
    User&    user { client[fd] };
    
    if (user.getNickName().empty())
    {
        user.respond("ERROR :You must send NICK command before USER.");
        return ;
    }
    if (param.size() != 4 || param.front().empty())
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "USER");
        //const std::string&  error { user.errors.at(Errors::ERR_NEEDMOREPARAMS) };
        //user.respond(user.buildMsg(Errors::ERR_NEEDMOREPARAMS, "USER", error));
        return ;
    }
   if (!user.getUserName().empty())
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "USER");
        //const std::string&  error { user.errors.at(Errors::ERR_ALREADYREGISTERED) };
        //user.respond(user.buildMsg(Errors::ERR_ALREADYREGISTERED, "USER", error));
        return ;
    }
    user.setUserName(param[0]);
    user.setHostName(param.at(1));
    user.setServerName(param.at(2));
    user.setRealName(param.at(3));
    user.setStatus(RegStatus::REGISTERED);
}
