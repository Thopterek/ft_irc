#include "../../inc/User.hpp"

const std::string        User::m_serverName = "ncs-irc.local";

User::User(int fd, std::string_view ip, std::string_view hostName, std::string_view serverPwd)
    :   m_fd{fd}, m_status{RegStatus::CONNECTED}, m_userIp{ip}, m_serverPwd{ serverPwd },
    m_hostName{hostName}
{
    m_errors.emplace(Errors::ERR_NONE, "");
    m_errors.emplace(Errors::ERR_NICKMORETHAN9CHARS, ":Toomuch letters");
    m_errors.emplace(Errors::ERR_NONICKNAMEGIVEN,  ":No Nick given");
    m_errors.emplace(Errors::ERR_NICKNAMEINUSE, ":Nick is already in use");
    m_errors.emplace(Errors::ERR_ERRONEUSNICKNAME, ":Erroneous nickname");
    m_errors.emplace(Errors::ERR_NEEDMOREPARAMS, ":Not enough parameters");
    m_errors.emplace(Errors::ERR_NOTREGISTERED, ":You have not registered");
    m_errors.emplace(Errors::ERR_ALREADYREGISTERED, ":You may not reregister");
    m_errors.emplace(Errors::ERR_REGISTERING, ":Pass set, continue registration");
    m_errors.emplace(Errors::ERR_PASSWDMISMATCH, ":Password incorrect");
    m_errors.emplace(Errors::ERR_NOSUCHNICK, ":No such nick");
    m_errors.emplace(Errors::ERR_CANNOTSENDTOCHAN, ":Cannot send to channel");
    m_errors.emplace(Errors::ERR_TOOMANYTARGETS, ": Too many targets");
    m_errors.emplace(Errors::ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
    m_errors.emplace(Errors::ERR_NOTEXTTOSEND, ":No text to send");
    m_errors.emplace(Errors::ERR_UNKNOWNMODE, ":is unknown mode char to me");
    m_errors.emplace(Errors::ERR_NOSUCHCHANNEL, ":No such channel");
    m_errors.emplace(Errors::ERR_NOTONCHANNEL, ":You're not on that channel");
    m_errors.emplace(Errors::ERR_CHANOPRIVSNEEDED, ":You're not channel operator");
    m_errors.emplace(Errors::ERR_UNKNOWNCOMMAND, ":Unknown command");
    m_errors.emplace(Errors::ERR_INPUTTOOLONG, ":Input line wass too long");
    // newly added
    m_errors.emplace(Errors::ERR_BADCHANNELKEY, ":Cannot join channel (+k)");
    m_errors.emplace(Errors::ERR_BANNEDFROMCHAN, ":Cannot join channel (+b)");
    m_errors.emplace(Errors::ERR_BADCHANMASK, ":Bad Channel Mask");
    m_errors.emplace(Errors::ERR_CHANNELISFULL, ":Cannot join channel (+l)");
    m_errors.emplace(Errors::ERR_NOTOPLEVEL, ": CHANN PRIVS SOMETHING");
    m_errors.emplace(Errors::ERR_USERONCHANNEL, ":is already on channel");
    m_errors.emplace(Errors::ERR_USERNOTINCHANNEL, ":They aren't on that channel");
    m_errors.emplace(Errors::ERR_NOTOPIC, ":No topic is set");
    m_errors.emplace(Errors::ERR_INVITEONLYCHAN, ":Cannot join channel (+i)");
}

void    User::setNickName(std::string_view nickName)
{
    m_nickName = nickName;
}

void    User::setOldNick(std::string_view oldNick)
{
    m_oldNick = oldNick;
}

void    User::setUserName(std::string_view userName)
{
    m_userName = userName;
}

void    User::setRealName(std::string_view realName)
{
    m_realName = realName;
}

void    User::setUserServerName(std::string_view userServerName)
{
    m_userServerName = userServerName;
}

void    User::setStatus(RegStatus status)
{
    m_status = status;
}

void    User::setHostName(std::string_view host) {
    m_hostName = host;
}

const std::string&  User::getServerName()
{
    return (m_serverName);
}

std::string User::getNickName() const
{
    return (m_nickName);
}

std::string User::getOldNick() const
{
    return (m_oldNick);
}

const std::string&  User::getServerPwd() const
{
    return (m_serverPwd);
}

std::string User::getUserName() const
{
    return (m_userName);
}

std::string User::getRealName() const
{
    return (m_realName);
}

std::string User::getUserIp() const
{
    return (m_userIp);
}

std::string User::getHostName() const
{
    return (m_hostName);
}

std::string& User::getBuffer() const
{
    return (m_buffer);
}

int User::getFd() const
{
    return (m_fd);
}

std::string User::getSource() const
{
    std::string source { m_nickName.empty() ? "*" : ":" + m_nickName };

    if (!m_userName.empty())
        source += ("!" + m_userName);
    if (!m_hostName.empty())
        source += ("@" + m_hostName);
    return (source);
}

void    User::buffer(std::string_view input)
{
    m_buffer += input;
}

void    User::handleErrors(Errors errorCode, const std::string& cmd)
{
    const std::string&  error { m_errors.at(errorCode) };
    respond(buildMsg(errorCode, cmd, error));
}

std::string    
User::buildMsg(Errors errCode, const std::string& cmd, const std::string& errMsg)
{
    std::string serverPrefix { ":" + getServerName()};
    std::string errorCode {std::to_string(static_cast<int>(errCode))};
    const std::string&  temp { cmd.empty() ? " " : " " + cmd + " " };

    serverPrefix += (" " + errorCode + " " + getNickName());
    return (serverPrefix + temp + errMsg + "\r\n");
}

void    User::respond(std::string_view msg)
{
    if (send(m_fd, msg.data(), msg.length(), 0) == -1)
        throw   std::runtime_error("Unable to send message");
}

RegStatus User::getStatus() const
{
    return (m_status);
}

