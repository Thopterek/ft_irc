#include "Client.hpp"

static ErrorValues validate(Client& client, int fd, std::string_view nick)
{
    if (nick.empty())
        return (Errors::ERR_NONICKNAMEGIVEN);
    if (!std::isalpha(nick.front()) || nick.size() > 9)
        return (Errors::ERR_ERRONEUSNICKNAME);
    for (auto c : nick)
        if (c == ' ' || c == ',' || c == '*' || c == '?' || c == '!' || c == '@')
            return (Errors::ERR_ERRONEUSNICKNAME);
    const std::string capitalized { client.ircCapitalize(nick) };
    for (auto user : client.m_clients)
        if (user->m_fd != client[fd].m_fd)
            if (capitalized == ircCapitalize(user->getNickName()))
                return (Errors::ERR_NICKNAMEINUSE);
    return (Errors::ERR_NONE);
}

void    nick(Client& client, int fd, std::vector<std::string> param)
{
    User&       user { client[fd] };

    if (param.empty())
    {
        const std::string&  error { user.errors.at(Errors::ERR_NONICKNAMEGIVEN) };
        user.respond(user.buildMsg(Errors::ERR_NONICKNAMEGIVEN, "NICK", error));
        return ;
    }

    const std::string&  newNick { param.front() };
    ErrorValues err { validate(client, fd, newNick) };

    if (err != Errors::ERR_NONE)
    {
        user.respond(user.buildMsg(err, "NICK", user.errors.at(err)));
        return ;
    }
    if (user.getNickName().empty())
    {
        user.setNickName(newNick);
        user.setStatus(RegStatus::REGISTERING);
        return ;
    }
    if (ircCapitalize(user.getNickName()) == ircCapitalize(newNick))
        return ;
    for (auto member : getChannels())
        member->broadcast(user.getSource() + " NICK :" + newNick);
    user.setOldNick(user.getNickName());
    user.setNickName(newNick);
}
