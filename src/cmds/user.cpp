#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

void    user(Client& client, int fd, const std::vector<std::string> &param)
{
    User&    user { client[fd] };
    
    if (user.getNickName().empty())
        user.setStatus(RegStatus::ATTEMPTED);
    if (param.size() != 4 || param.front().empty())
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "USER");
        return ;
    }
    if (param.at(3).size() < 2 || param.at(3).front() != ':')
    {
        user.respond("ERROR :No realname entered");
        return ;
    }
    if (!user.getUserName().empty())
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "USER");
        return ;
    }
    user.setUserName(param[0]);
    user.setHostName(param.at(1));
    user.setUserServerName(param.at(2));
    user.setRealName(param.at(3));
    if (user.getStatus() != RegStatus::ATTEMPTED)
    {
        user.setStatus(RegStatus::REGISTERED);
        const std::string&  welcome { user.getNickName() +
                        " :Welcome to the NCS IRC network, " };
        user.respond(":" + user.getServerName() + " 001 " +
                        welcome + user.getSource() + "\r\n");
    }
}
