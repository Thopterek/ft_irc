#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

void    user(Client& client, int fd, const std::vector<std::string> &param)
{
    std::cout << "\033[33m\033[1m" << "Command: USER started" << "\033[0m" << std::endl;
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
    user.setUserServerName(param.at(2));
    user.setRealName(param.at(3));
    user.setStatus(RegStatus::REGISTERED);
    const std::string&  welcome { user.getNickName() + " :Welcome to the NCS IRC network, " };
    user.respond(user.getServerName() + " 001 " + welcome + user.getSource() + "\r\n");
	std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
