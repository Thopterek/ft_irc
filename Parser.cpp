/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sudaniel <sudaniel@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:52:15 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/24 18:04:19 by sudaniel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser()
{
    m_handler.emplace("NICK", handleNick);
    m_handler.emplace("USER", handleUser);
    m_handler.emplace("PASS", handlePwd);
    m_handler.emplace("INVITE", handleInvite);
    m_handler.emplace("CAP", handleCap);
    m_handler.emplace("PRIVMSG", handlePrivMsg);
    m_handler.emplace("JOIN", handleJoin);
    m_handler.emplace("PART", handlePart);
    m_handler.emplace("KICK", handleKick);
    m_handler.emplace("QUIT", handleQuit);
    m_handler.emplace("MODE", handleMode);
    m_handler.emplace("TOPIC", handleTopic);
    m_handler.emplace("WHO", handleWho);
    m_handler.emplace("PING", handlePing);
    m_handler.emplace("PONG", handlePong);
}
