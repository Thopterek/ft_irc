/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sudaniel <sudaniel@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:18:57 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/20 17:56:59 by sudaniel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include "User.hpp"

class User;

class   Client
{
    std::unordered_map<int, User*>      m_users;
    static inline constexpr std::string_view   m_serverName = "ncs-irc.local";
  public:
    Client() = delete;
    explicit Client(int, std::string_view, std::string_view);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client() = default;

    static  std::string_view  getServerName();
    void    connect(int, std::string_view, std::string_view);
    void    disconnect(int);
};

#endif
