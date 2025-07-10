#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

static Errors validate(Client& client, int fd, const std::string &nick)
{
    std::cout << "\033[33m\033[1m" << "Command: NICK started" << "\033[0m" << std::endl;
    if (nick.empty())
        return (Errors::ERR_NONICKNAMEGIVEN);
    if (!std::isalpha(nick.front()) || nick.size() > 9)
        return (Errors::ERR_ERRONEUSNICKNAME);
    for (auto c : nick)
        if (c == ' ' || c == ',' || c == '*' || c == '?' || c == '!' || c == '@')
            return (Errors::ERR_ERRONEUSNICKNAME);
    const std::string capitalized { client.ircCapitalize(nick) };
    const std::unordered_map<int, User*> users { client.getUsers() };
    for (auto user : users)
        if (user.second->getFd() != client[fd].getFd())
            if (capitalized == client.ircCapitalize(user.second->getNickName()))
                return (Errors::ERR_NICKNAMEINUSE);
    return (Errors::ERR_NONE);
}

void    nick(Client& client, int fd, const std::vector<std::string> &param)
{
    User&       user { client[fd] };

    if (user.getStatus() == RegStatus::CONNECTED)
    {
        user.respond("ERROR :You must send PASS command before registering.");
        return ;
    }
    if (param.empty())
    {
        user.handleErrors(Errors::ERR_NONICKNAMEGIVEN, "NICK");
        return ;
    }

    const std::string&  newNick { param.front() };
    Errors err { validate(client, fd, newNick) };

    if (err != Errors::ERR_NONE)
    {
        user.handleErrors(err, "NICK");
        return ;
    }
    if (user.getNickName().empty())
    {
        user.setNickName(newNick);
        std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
        return ;
    }
    if (client.ircCapitalize(user.getNickName()) == client.ircCapitalize(newNick))
        return ;
    /*
        after merging with channel handling uncomment and test
    */
    // for (auto member : getChannels())
    //     member->broadcast(user.getSource() + " NICK :" + newNick, user);
    user.setOldNick(user.getNickName());
    user.setNickName(newNick);
    std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
