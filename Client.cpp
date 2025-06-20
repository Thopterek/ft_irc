/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sudaniel <sudaniel@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 16:50:39 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/20 17:56:55 by sudaniel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, std::string_view ip, std::string_view hostName)
{
    connect(fd, ip, hostName);
}

std::string_view  Client::getServerName()
{
    return m_serverName;
}

void    
    Client::connect(int fd, std::string_view ip, std::string_view hostName)
{
    User    *user = new User(fd, ip, hostName);
    m_users.emplace(fd, user);
}

void    Client::disconnect(int socketFd)
{
    auto    it = m_users.find(socketFd);
    if (it == m_users.end())
        return ;
    delete it->second;
    m_users.erase(it);
}
