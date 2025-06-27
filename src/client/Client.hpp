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

class   Client
{
    int m_userCount {0};
    std::unordered_map<int, User*>              m_users;
  public:
    Client();
    explicit Client(int, std::string_view, std::string_view);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client() = default;
    User&   operator[](int);

    void    connect(int, std::string_view, std::string_view);
    void    disconnect(int);
};

#endif
