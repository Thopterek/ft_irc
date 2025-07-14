#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <utility>

class Channel;

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
    ERR_NOTOPIC = 331, //check on this
    ERR_INVITEONLYCHAN = 473, //check on this
    ERR_UNKNOWNERROR = 400,
    ERR_NOSUCHNICK = 401,
    ERR_NOSUCHCHANNEL = 403, // CHECK ON THIS
    ERR_CANNOTSENDTOCHAN = 404,
    ERR_TOOMANYTARGETS = 407,
    ERR_NORECIPIENT = 411,
    ERR_NOTEXTTOSEND = 412,
    ERR_INPUTTOOLONG = 417,
    ERR_UNKNOWNCOMMAND = 421,
    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_NICKCOLLISION = 436,
    ERR_USERNOTINCHANNEL = 441, // CHECK ON THIS
    ERR_NOTONCHANNEL = 442, // CHECK ON THIS
    ERR_USERONCHANNEL = 443, // CHECK ON THIS
    ERR_NOTREGISTERED = 451,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_CHANOPRIVSNEEDED = 482, // CHECK ON THIS
    ERR_UNKNOWNMODE = 501,
    // here are added
    ERR_BADCHANNELKEY = 476,
    ERR_BANNEDFROMCHAN = 474,
    ERR_BADCHANMASK = 475,
    ERR_CHANNELISFULL = 471,
    ERR_NOTOPLEVEL = 413
    // ths is stop
    //RPL_AWAY = 301
};

class   Client
{
    int                             m_userCount {};
    std::unordered_map<int, User*>  m_users;
    static int     ircToupper(int c);
    /*
      channel handling unorderd map
      keeping track of all of them
    */
    std::unordered_map<std::string, std::unique_ptr<Channel>> channels;
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
    /*
      channel handling operations
    */
    Channel* getChannelByName(const std::string& name);
    Channel* createChannel(const std::string& name, unsigned int clientId);
    void deleteChannel(const std::string& name);
    std::unordered_map<std::string, std::unique_ptr<Channel>>& getAllChannels();
};

#endif
