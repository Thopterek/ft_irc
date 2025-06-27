#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "User.hpp"

class User;

//Todo: Write an ircCapitalize function.
enum class Errors
{
    ERR_NONE = 0,
    ERR_NICKMORETHAN9CHARS = 1,
    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_NICKCOLLISION = 436,
};

class   Client
{
    int                             m_userCount {0};
    std::unordered_map<int, User*>  m_users;
  public:
    Client() = default;
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client() = default;
    User&   operator[](int);

    void    connect(int, std::string_view, std::string_view);
    void    disconnect(int);
};

#endif
