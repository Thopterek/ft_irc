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

class User;

/*
  fetching of particular message
  on non command and not registered
*/
enum class Errors
{
    ERR_NONE = 0,
    ERR_NICKMORETHAN9CHARS = 1,
    ERR_REGISTERING = 2,
    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_NICKCOLLISION = 436,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_NOSUCHNICK = 401,
    ERR_CANNOTSENDTOCHAN = 404,
    ERR_TOOMANYTARGETS = 407,
    ERR_NORECIPIENT = 411,
    ERR_NOTEXTTOSEND = 412,
    ERR_UNKNOWNCOMMAND = 421,
    ERR_NOTREGISTERED = 451
    //RPL_AWAY = 301
};

class   Client
{
    int                             m_userCount {};
    std::unordered_map<int, User*>  m_users;
    static int     ircToupper(int c);
  public:
    Client() = default;
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client() = default;
    User&   operator[](int);

    const std::unordered_map<int, User*>&   getUsers() const;
    void    connect(int, std::string_view, 
                    std::string_view, std::string_view);
    void    disconnect(int);
    static const std::string   ircCapitalize(const std::string& str);
};

#endif
