#include "../client/User.hpp"

void    pass(Client& client, int fd, std::vector<std::string> param)
{
    User&       user { client[fd] };

    if (user.getStatus() == RegStatus::REGISTERING)
    {
        user.handleErrors(Errors::ERR_REGISTERING, "PASS");
        //const std::string&  error { user.errors.at(Errors::ERR_REGISTERING) };
        //user.respond(user.buildMsg(Errors::ERR_REGISTERING, "PASS", error));
        return ;
    }
    else if (user.getStatus() == RegStatus::REGISTERED)
    {
        user.handleErrors(Errors::ERR_ALREADYREGISTERED, "PASS");
        //const std::string&  error { user.errors.at(Errors::ERR_ALREADYREGISTERED) };
        //user.respond(user.buildMsg(Errors::ERR_ALREADYREGISTERED, "PASS", error));
        return ;
    }
    if (param.empty() || param.size() != 1)
    {
        user.handleErrors(Errors::ERR_NEEDMOREPARAMS, "PASS");
        //const std::string&  error { user.errors.at(Errors::ERR_NEEDMOREPARAMS) };
        //user.respond(user.buildMsg(Errors::ERR_NEEDMOREPARAMS, "PASS", error));
        return ;
    }
    if (param[0] != user.getServerPwd())
    {
        user.handleErrors(Errors::ERR_PASSWDMISMATCH, "PASS");
       // const std::string&  error { user.errors.at(Errors::ERR_PASSWDMISMATCH) };
       // user.respond(user.buildMsg(Errors::ERR_PASSWDMISMATCH, "PASS", error));
        return ;
    }
    user.setStatus(RegStatus::REGISTERING);
}
