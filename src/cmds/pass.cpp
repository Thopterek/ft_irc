#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

void    pass(Client& client, int fd, const std::vector<std::string> &param)
{
    std::cout << "\033[33m\033[1m" << "Command: PASS started" << "\033[0m" << std::endl;
    User&       user { client[fd] };

    if (user.getStatus() == RegStatus::REGISTERING)
    {
        user.handleErrors(Errors::ERR_REGISTERING, "");
        return ;
    }
    else if (user.getStatus() == RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "");
        return ;
    }
    if (param.empty() || param.size() != 1)
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PASS");
        return ;
    }
    if (param[0] != user.getServerPwd())
    {
        user.handleErrors(Errors::ERR_PASSWDMISMATCH, "");
        return ;
    }
    user.setStatus(RegStatus::REGISTERING);
    std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
