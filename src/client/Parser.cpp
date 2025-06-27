/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndziadzi <ndziadzi@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:52:15 by sudaniel          #+#    #+#             */
/*   Updated: 2025/06/27 15:41:06 by ndziadzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser()
{
    // m_handler.emplace("NICK", handleNick);
    // m_handler.emplace("USER", handleUser);
    // m_handler.emplace("PASS", handlePwd);
    // m_handler.emplace("INVITE", handleInvite);
    // m_handler.emplace("CAP", handleCap);
    // m_handler.emplace("PRIVMSG", handlePrivMsg);
    // m_handler.emplace("JOIN", handleJoin);
    // m_handler.emplace("PART", handlePart);
    // m_handler.emplace("KICK", handleKick);
    // m_handler.emplace("QUIT", handleQuit);
    // m_handler.emplace("MODE", handleMode);
    // m_handler.emplace("TOPIC", handleTopic);
    // m_handler.emplace("WHO", handleWho);
    // m_handler.emplace("PING", handlePing);
    // m_handler.emplace("PONG", handlePong);
}

std::vector<std::string>    Parser::tokenize(std::string_view msg)
{
    std::string                 input(msg);
    std::istringstream          iss(input);
    std::string                 params;
    std::vector<std::string>    tokens;

    if (input.front() == ':')
        throw std::runtime_error("Client messages must not include a source");
    while (iss >> params)
    {
        //check for trailing prefix
        if (params.front() == ':')
        {
            std::string trailingPrefix = params.substr(1);
            std::string rest;
            std::getline(iss, rest);
            tokens.push_back(trailingPrefix + rest);
            break ;
        }
        tokens.push_back(params);
    }
    return (tokens);
}

void    
Parser::dispatchCommand(User& user, const std::vector<std::string>& argVec)
{
    if (argVec.empty())
        return ;

    std::string                 cmd{argVec.front()};
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string>    params{argVec.begin() + 1, argVec.end()};

    try
    {
        // m_handler.at(cmd)(user, params);
        std::cout << "DEBUG: it went into handler" << std::endl;
    }
    catch (const std::out_of_range&)
    {
        std::string msg;
        // msg = user.buildMsg(ERR_UNKNOWNCOMMAND, cmd, ": Unknown Command");
        std::cout << "DEBUG: ERR_UNKOWNCOMMAND is to be defined" << std::endl;
        user.respond(msg);
    }
}

void    Parser::parseAndDispatch(User& user)
{
    std::cout << user.getBuffer() << ": a whole buffer" << std::endl;
    try
    {   
        std::vector<std::string>    tokens{ tokenize(user.getBuffer()) };
        if (!tokens.empty())
            dispatchCommand(user, tokens);
    }
    catch (const std::runtime_error& e)
    {
        user.respond(e.what());
    }
    user.getBuffer().clear();
}
