#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>
#include <exception>
#include <string_view>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"


enum class RegStatus 
{ 
    CONNECTED,
    REGISTERING,
    REGISTERED
};

class Channel;

class   User
{
    const int           m_fd;
    // int                 m_channelCount { };
    RegStatus           m_status;
    const std::string   m_userIp, m_serverPwd;
    std::string         m_nickName, m_oldNick, m_userName, m_realName, m_hostName;
    std::string         m_userServerName;
    mutable std::string m_buffer;
    static const std::string                m_serverName;
    std::map<Errors, std::string>  m_errors;
  public:
    User() = delete;
    explicit User(int, std::string_view, std::string_view, std::string_view);
    User(const User&) = delete;
    User&   operator=(const User&) = delete;
    ~User() = default;
    
    void        setNickName(std::string_view);
    void        setOldNick(std::string_view);
    void        setUserName(std::string_view);
    void        setRealName(std::string_view);
    void        setHostName(std::string_view);
    void        setStatus(RegStatus);
    void        setUserServerName(std::string_view);
    static const std::string&  getServerName();
    std::string         getNickName() const;
    std::string         getOldNick() const;
    std::string         getUserName() const;
    std::string         getRealName() const;
    std::string         getUserIp() const;
    std::string         getHostName() const;
    const std::string&  getServerPwd() const;
    std::string&        getBuffer() const;
    int                 getFd() const;
    RegStatus           getStatus() const;
    std::string         getSource() const;

    void        buffer(std::string_view);
    void        handleErrors(Errors, const std::string&);
    void        respond(std::string_view);
    std::string buildMsg(Errors, const std::string&, const std::string&);
   };

#endif
