#include "Client.hpp"

Client::Client() : m_users() {}

User&   Client::operator[](int socketFd)
{
    return (*(m_users.at(socketFd)));
}

const std::unordered_map<int, User*>&   Client::getUsers() const
{
    return (m_users);
}

void    
    Client::connect(int fd, std::string_view ip, std::string_view hostName)
{
    User    *user = new User(fd, ip, hostName);
    m_users.emplace(fd, user);
    ++m_userCount;
    std::cout << "Client with fd: " << fd << " added to client pool" << std::endl;
}

void    Client::disconnect(int socketFd)
{
    auto    it = m_users.find(socketFd);
    if (it == m_users.end())
        return ;
    delete it->second;
    m_users.erase(it);
    --m_userCount;
    std::cout << "Client with fd: " << socketFd << " removed from the pool" << std::endl;
}

const std::string  Client::ircCapitalize(const std::string& str)
{
    std::string capitalize;
    capitalize.reserve(str.size());

    for (auto c : str)
        capitalize += static_cast<char>(ircToupper(c));
    return (capitalize);
}

// IRC case-insensitive mapping
int Client::ircToupper(int c)
{
    if (c == '{')
        return ('[');
    else if (c == '}')
        return (']');
    else if (c == '|')
        return ('\\');
    else if (c == '^')
        return ('~');
    else
        return (std::toupper(c));
}
