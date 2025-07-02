#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>
#include <exception>
#include <string_view>
#include "Client.hpp"
#include "Channel.hpp"


enum class RegStatus 
{ 
    CONNECTED,
    REGISTERING,
    REGISTERED
};


class   User
{
    const int           m_fd;
    int                 m_channelCount {0};
    RegStatus           m_status;
    const std::string   m_userIp, m_hostName, m_serverPwd;
    std::string         m_nickName, m_oldNick, m_userName, m_realName, m_buffer; 
    static const std::string            m_serverName;
    std::set<Channel*>                  m_channels;
    std::map<ErrorValues, std::string>  m_errors;
    std::string buildMsg(int, const std::string&, const std::string&);
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
    void        setStatus(RegStatus);
    static const std::string&  getServerName();
    std::string         getNickName() const;
    std::string         getOldNick() const;
    std::string         getUserName() const;
    std::string         getRealName() const;
    std::string         getUserIp() const;
    std::string         getHostName() const;
    const std::string&  getServerPwd() const;
    std::string         getBuffer() const;
    int                 getFd() const;
    RegStatus           getStatus() const;
    const std::set<Channel*>&  getChannels() const;
    int                 getChannelCount() const;
    std::string         getSource() const;

    void        addToChannel(Channel*);
    void        removeFromChannels(Channel*);
    void        buffer(std::string_view);
    void        incrementChannelCount();
    void        decrementChannelCount();
    void        handleErrors(Errors, const std::string&);
    void        respond(std::string_view);
   };

#endif
