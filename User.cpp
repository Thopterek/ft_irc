/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sudaniel <sudaniel@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 15:45:43 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/20 17:08:31 by sudaniel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd, std::string_view ip, std::string_view hostName)
    :   m_socketFd{fd}, m_userIp{ip},
        m_hostName{hostName}, m_status{RegStatus::CONNECTED}
{}

void    User::setNickName(std::string_view nickName)
{
    m_nickName = nickName;
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

std::string User::getNickName() const
{
    return (m_nickName);
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

int User::getSocketFd() const
{
    return (m_socketFd);
}

int User::getChannelCount() const
{
    return (m_channelCount);
}

RegStatus User::getStatus() const
{
    return (m_status);
}

void    User::incrementChannelCount()
{
    ++m_channelCount;
}

void    User::decrementChannelCount()
{
    --m_channelCount;
}
