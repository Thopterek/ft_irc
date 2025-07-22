#include "../../inc/Client.hpp"
#include "../../inc/User.hpp"
#include "../../inc/Channel.hpp"

User&   Client::operator[](int socketFd)
{
    return (*(m_users.at(socketFd)));
}

const std::unordered_map<int, User*>&   Client::getUsers() const
{
    return (m_users);
}

void    Client::connect(int fd, std::string_view ip,
            std::string_view hostName, std::string_view serverPwd)
{
    User    *user = new User(fd, ip, hostName, serverPwd);
    m_users.emplace(fd, user);
}

void    Client::disconnect(int fd)
{
    auto    it = m_users.find(fd);
    if (it == m_users.end())
        return ;
    User&   user = *(it->second);
    const std::string msg = ":" + it->second->getSource() + " QUIT :Ping timeout: 120 seconds" + "\r\n";
    for (auto ref = getAllChannels().begin(); ref != getAllChannels().end();)
	{
		Channel* chan = ref->second.get();
		if (chan->isMember(fd))
		{
			chan->broadcast(msg, user);
			chan->removeMember(fd);
			if (chan->getMembers().empty()) {
				deleteChannel(chan->name);
				ref = getAllChannels().erase(ref);
				continue;
			}
		}
		++ref;
	}
    delete it->second;
    m_users.erase(it);
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

Channel* Client::getChannelByName(const std::string& name) {
	auto it = channels.find(name);
	if (it != channels.end())
		return it->second.get();
	return NULL;
}

Channel* Client::createChannel(const std::string& name, unsigned int clientId) {
	auto chan = std::make_unique<Channel>(name, clientId);
	Channel* chan_ptr = chan.get();
	channels[name] = std::move(chan);
	return chan_ptr;
}

void Client::deleteChannel(const std::string& name) {
    auto it = channels.find(name);
    if (it == channels.end()) {
        return;
    }
    channels.erase(name);
}

std::unordered_map<std::string, std::unique_ptr<Channel>>& Client::getAllChannels() {
	return channels;
}
