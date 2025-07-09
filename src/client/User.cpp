#include "User.hpp"
// #include "../../inc/Channel.hpp"

const std::string        User::m_serverName = "ncs-irc.local";

User::User(int fd, std::string_view ip, std::string_view hostName, std::string_view serverPwd)
    :   m_fd{fd}, m_status{RegStatus::CONNECTED}, m_userIp{ip}, m_serverPwd{ serverPwd },
    m_hostName{hostName}
{
    m_errors.emplace(Errors::ERR_NONE, "");
    m_errors.emplace(Errors::ERR_NICKMORETHAN9CHARS, ": Toomuch letters");
    m_errors.emplace(Errors::ERR_NONICKNAMEGIVEN,  ": No Nick given");
    m_errors.emplace(Errors::ERR_NICKNAMEINUSE, ": Nick in use");
    m_errors.emplace(Errors::ERR_ERRONEUSNICKNAME, ": Erroneous nickname");
    m_errors.emplace(Errors::ERR_NEEDMOREPARAMS, ": More parameters needed");
    m_errors.emplace(Errors::ERR_ALREADYREGISTERED, ": Already registered");
    m_errors.emplace(Errors::ERR_REGISTERING, ": Pass set, continue registration");
    m_errors.emplace(Errors::ERR_PASSWDMISMATCH, ": Wrong password entered");
    m_errors.emplace(Errors::ERR_NOSUCHNICK, ": No such nick");
    m_errors.emplace(Errors::ERR_CANNOTSENDTOCHAN, ": You cannot send message to channel");
    m_errors.emplace(Errors::ERR_TOOMANYTARGETS, ": Too many targets");
    m_errors.emplace(Errors::ERR_NORECIPIENT, ": You must provide a target for your message");
    m_errors.emplace(Errors::ERR_NOTEXTTOSEND, ": Message must not be empty");
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
    std::string source {m_nickName.empty() ? "*" : ":" + m_nickName};

    if (!m_userName.empty())
        source += ("!" + m_userName);
    if (!m_hostName.empty())
        source += ("@" + m_hostName);
    return (source);
}

void    User::buffer(std::string_view input)
{
    std::cout << input << ": got added to the buffer" << std::endl;
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
    
    serverPrefix += (" " + errorCode + " " + getNickName());
    return (serverPrefix + " " + cmd + " " + errMsg + " \r\n");
}

void    User::respond(std::string_view msg)
{
    if (send(m_fd, msg.data(), msg.length(), 0) == -1)
        throw   std::runtime_error("Unable to send message");
}

const std::map<const std::string, Channel*>&   User::getChannels() const
{
    return (m_channels);
}

int User::getChannelCount() const
{
    return (m_channelCount);
}

RegStatus User::getStatus() const
{
    return (m_status);
}

// void    User::addToChannel(Channel* channel)
// {
//     m_channels.emplace(channel->getChannelName(), channel);
// }

// void    User::removeFromChannels(Channel* channel)
// {
//     auto  iter { m_channels.find(channel->getChannelName()) };
//     if (iter != end(m_channels))
//         m_channels.erase(iter);
// }

void    User::incrementChannelCount()
{
    ++m_channelCount;
}

void    User::decrementChannelCount()
{
    if (m_channelCount > 0)
        --m_channelCount;
}
