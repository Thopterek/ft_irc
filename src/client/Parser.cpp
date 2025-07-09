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
    m_cmds.emplace("PASS", pass);
    m_cmds.emplace("NICK", nick);
    m_cmds.emplace("USER", user);
    m_cmds.emplace("PRIVMSG", privMsg);
    // m_cmds.emplace("INVITE", handleInvite);
    // m_cmds.emplace("CAP", handleCap);
    // m_cmds.emplace("JOIN", handleJoin);
    // m_cmds.emplace("PART", handlePart);
    // m_cmds.emplace("KICK", handleKick);
    // m_cmds.emplace("QUIT", handleQuit);
    // m_cmds.emplace("MODE", handleMode);
    // m_cmds.emplace("TOPIC", handleTopic);
    // m_cmds.emplace("WHO", handleWho);
    // m_cmds.emplace("PING", handlePing);
    // m_cmds.emplace("PONG", handlePong);
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
Parser::dispatchCommand(User& user, const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return ;

    std::string                 cmd{tokens.front()};
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string>    params{tokens.begin() + 1, tokens.end()};

    try
    {
        m_cmds.at(cmd)(user, params);
        std::cout << "DEBUG: it went into handler" << std::endl;
    }
    catch (const std::out_of_range&)
    {
        user.handleError(Error::ERR_UNKNOWNCOMMAND, cmd);
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
