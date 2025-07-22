#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

static Errors validate(Client& client, int fd, const std::string &nick)
{
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

void    nick(Client& client, int fd, const std::vector<std::string>& param)
{
    User&       user { client[fd] };
    if (param.empty())
    {
        user.handleErrors(Errors::ERR_NONICKNAMEGIVEN, "");
        return ;
    }
    if (user.getStatus() == RegStatus::ATTEMPTED)
    {
        Errors err { validate(client, fd, param.front()) };
        if (err == Errors::ERR_NONE) {
            user.setStatus(RegStatus::REGISTERED);
            user.setNickName(param.front());
            const std::string&  welcome { user.getNickName() + " :Welcome to the NCS IRC network, " };
            user.respond(":" + user.getServerName() + " 001 " + welcome + user.getSource() + "\r\n");
            return ;
        }
        else {
            if (err == Errors::ERR_NONICKNAMEGIVEN)
                user.handleErrors(err, "");
            else
                user.handleErrors(err, param.front());
        }
    }
    if (user.getStatus() == RegStatus::CONNECTED)
    {
        user.respond("ERROR :You must send PASS command before registering.");
        return ;
    }

    const std::string&  newNick { param.front() };
    Errors err { validate(client, fd, newNick) };
    if (err != Errors::ERR_NONE)
    {
        if (err == Errors::ERR_NONICKNAMEGIVEN)
            user.handleErrors(err, "NICK");
        else
            user.handleErrors(err, newNick);

        return ;
    }
    if (user.getNickName().empty())
    {
        user.setNickName(newNick);
        return ;
    }
    if (client.ircCapitalize(user.getNickName()) == client.ircCapitalize(newNick))
        return ;
        
    auto& channels { client.getAllChannels() };
    for (auto& member : channels)
    {
        Channel*    channel { member.second.get() };
        if (channel->isMember(user.getFd()))
        {
            std::string msg = ":" + user.getNickName() + "!" + user.getUserName()+
                "@" + user.getHostName() + " NICK :" + newNick + "\r\n";
            channel->broadcast(msg, user);
        }
    }
    std::string msg2 = ":" + user.getNickName() + "!" + user.getUserName() +
        "@" + user.getHostName() + " NICK :" + newNick + "\r\n";
    user.respond(msg2);
    user.setOldNick(user.getNickName());
    user.setNickName(newNick);
}
