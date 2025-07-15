#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

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
    user.setStatus(RegStatus::REGISTERED);
    /*
        NEVER TOUCH THIS LINE
        PLEASE FOR THE LOVE OF GOD AND ALL THAT IS HOLLY
        STOP IT AND LEAVE IT
    */
    const std::string&  welcome { user.getNickName() + " :Welcome to the NCS IRC network, " };
    user.respond(":" + user.getServerName() + " 001 " + welcome + user.getSource() + "\r\n");
}
