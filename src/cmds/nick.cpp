#include "../../inc/User.hpp"
#include "../../inc/Parser.hpp"

static Errors validate(Client& client, int fd, const std::string &nick)
{
    std::cout << "ARE WE SEGFAULTING HERE?" << std::endl;
    if (nick.empty())
        return (Errors::ERR_NONICKNAMEGIVEN);
    std::cout << "after the nick.empty" << std::endl;
    if (!std::isalpha(nick.front()) || nick.size() > 9)
        return (Errors::ERR_ERRONEUSNICKNAME);
    std::cout << "are we after alpha?" << std::endl;
    for (auto c : nick)
        if (c == ' ' || c == ',' || c == '*' || c == '?' || c == '!' || c == '@')
            return (Errors::ERR_ERRONEUSNICKNAME);
    std::cout << "we should never be here" << std::endl;
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
        std::cout << "DID WE GET HERE?" << std::endl;
        Errors err { validate(client, fd, param.front()) };
        std::cout << "are we passed the validation" << std::endl;
        if (err == Errors::ERR_NONE) {
            std::cout << "did we go to err_none" << std::endl;
            user.setStatus(RegStatus::REGISTERED);
            user.setNickName(param.front());
            const std::string&  welcome { user.getNickName() + " :Welcome to the NCS IRC network, " };
            user.respond(":" + user.getServerName() + " 001 " + welcome + user.getSource() + "\r\n");
            return ;
        }
        else {
            // user.setStatus(RegStatus::CONNECTED);
            std::cout << "ARE WE GOING INTO ERROR HANDLING?" << std::endl;
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
    std::cout << "This is what is in newNick-> " << newNick << std::endl; 
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
            std::cout << "User is a member of channel: " << std::endl;
            //std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " NICK :" + newNick + "\r\n";//he maybe not
            std::string msg = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " NICK :" + channel->name + " " + newNick + "\r\n";
            channel->broadcast(msg, user);
        }
    }
    std::string msg2 = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " NICK :" + newNick + "\r\n";
    user.respond(msg2);
    user.setOldNick(user.getNickName());
    user.setNickName(newNick);
    std::cout << "\033[32m" << "command went through succefully" << "\033[0m" << std::endl;
}
