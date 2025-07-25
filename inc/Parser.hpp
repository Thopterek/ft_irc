#ifndef PARSER_HPP
#define PARSER_HPP

#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include "User.hpp"

class   Parser
{
  public:
    using Cmd = void (*)(Client&, int, const std::vector<std::string>&);
    Parser();
    ~Parser() = default;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    static void parseAndDispatch(Client&, int);
  private:
    static std::unordered_map<std::string, Cmd> m_cmds;
    static std::vector<std::string> tokenize(std::string_view);
    static  void    dispatchCommand(Client&, int fd, const std::vector<std::string>&);
};

/*
  basic commands
*/
void    pass(Client&, int, const std::vector<std::string>&);
void    nick(Client&, int, const std::vector<std::string>&);
void    user(Client&, int, const std::vector<std::string>&);
void    privmsg(Client&, int, const std::vector<std::string>&);
void    ping(Client&, int, const std::vector<std::string>&);
void    pong(Client&, int, const std::vector<std::string>&);

/*
  channel side commands
*/
void invite(Client& client, int fd, const std::vector<std::string> &param);
void join(Client& client, int fd, const std::vector<std::string> &param);
void kick(Client& client, int fd, const std::vector<std::string> &param);
void mode(Client& client, int fd, const std::vector<std::string> &param);
void part(Client& client, int fd, const std::vector<std::string> &param);
void quit(Client& client, int fd, const std::vector<std::string> &param);
void topic(Client& client, int fd, const std::vector<std::string> &param);

#endif
