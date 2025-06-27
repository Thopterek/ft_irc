#include "Client.hpp"

Client::Client(int fd, std::string_view ip, std::string_view hostName)
{
    connect(fd, ip, hostName);
}

User&   Client::operator[](int socketFd)
{
    return (*(m_users.at(socketFd)));
}

void    
    Client::connect(int fd, std::string_view ip, std::string_view hostName)
{
    User    *user = new User(fd, ip, hostName);
    m_users.emplace(fd, user);
    ++m_userCount;
}

void    Client::disconnect(int socketFd)
{
    auto    it = m_users.find(socketFd);
    if (it == m_users.end())
        return ;
    delete it->second;
    m_users.erase(it);
    --m_userCount;
}
