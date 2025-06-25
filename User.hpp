/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sudaniel <sudaniel@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 14:36:45 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/20 17:08:27 by sudaniel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>
#include <exception>
#include <string_view>
#include "Client.hpp"

    
enum class RegStatus 
{ 
    CONNECTED,
    REGISTERING,
    REGISTERED
};


class   User
{
    const int           m_socketFd;
    int                 m_channelCount {0};
    RegStatus           m_status;
    const std::string   m_userIp, m_hostName;
    std::string         m_nickName, m_userName, m_realName, m_buffer; 
    static const std::string        m_serverName;
  public:
    User() = delete;
    explicit User(int, std::string_view, std::string_view);
    User(const User&) = delete;
    User&   operator=(const User&) = delete;
    ~User() = default;
    
    void        setNickName(std::string_view);
    void        setUserName(std::string_view);
    void        setRealName(std::string_view);
    void        setStatus(RegStatus);
    static  const std::string getServerName();
    std::string         getNickName() const;
    std::string         getUserName() const;
    std::string         getRealName() const;
    std::string         getUserIp() const;
    std::string         getHostName() const;
    std::string         getBuffer() const;
    int                 getSocketFd() const;
    RegStatus           getStatus() const;
    int                 getChannelCount() const;

    std::string getPrefix() const;
    void        buffer(std::string_view);
    void        incrementChannelCount();
    void        decrementChannelCount();
    std::string buildMsg(int, const std::string&, const std::string&);
    void                respond(std::string_view);
   };

#endif
