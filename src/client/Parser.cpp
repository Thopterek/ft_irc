#include "../../inc/Parser.hpp"

std::unordered_map<std::string, Parser::Cmd> Parser::m_cmds;

Parser::Parser()
{
    m_cmds.emplace("PASS", pass);
    m_cmds.emplace("NICK", nick);
    m_cmds.emplace("USER", user);
    m_cmds.emplace("PRIVMSG", privmsg);
    /*
        channel side of things
    */
    m_cmds.emplace("INVITE", invite);
    m_cmds.emplace("JOIN", join);
    m_cmds.emplace("MODE", mode);
    m_cmds.emplace("KICK", kick);
    m_cmds.emplace("QUIT", quit);
    m_cmds.emplace("PART", part);
    // m_cmds.emplace("CAP", handleCap);
    m_cmds.emplace("TOPIC", topic);
    // m_cmds.emplace("WHO", handleWho);
    m_cmds.emplace("PING", ping);
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
Parser::dispatchCommand(Client& client, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return ;

    std::string                 cmd{tokens.front()};
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string>    params{tokens.begin() + 1, tokens.end()};

    try
    {
        m_cmds.at(cmd)(client, fd, params);
    }
    catch (const std::out_of_range&)
    {
        throw std::out_of_range(cmd);
    }
}

void    Parser::parseAndDispatch(Client &clients, int fd)
{
    User&   user { clients[fd] };
    
    std::string_view buffer { user.getBuffer() };
    if (buffer.size() > 512)
    {
        user.handleErrors(Errors::ERR_INPUTTOOLONG, "");
        return ;
    }
    try
    {   
        std::vector<std::string>    tokens{ tokenize(user.getBuffer()) };
        if (!tokens.empty())
            dispatchCommand(clients, fd, tokens);
    }
    catch (const std::exception& e)
    {
        user.handleErrors(Errors::ERR_UNKNOWNCOMMAND, e.what());
    }
    user.getBuffer().clear();
}
