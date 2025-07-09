#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

void    pass(Client& client, int fd, const std::vector<std::string> &param)
{
    User&       user { client[fd] };

    if (user.getStatus() == RegStatus::REGISTERING)
    {
        user.handleErrors(Errors::ERR_REGISTERING, "PASS");
        return ;
    }
    else if (user.getStatus() == RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "PASS");
        return ;
    }
    if (param.empty() || param.size() != 1)
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PASS");
        return ;
    }
    if (param[0] != user.getServerPwd())
    {
        user.handleErrors(Errors::ERR_PASSWDMISMATCH, "PASS");
        return ;
    }
    user.setStatus(RegStatus::REGISTERING);
}
