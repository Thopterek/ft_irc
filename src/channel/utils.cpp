#include "../inc/Channel.hpp"

void Commands::HandleCommand(const std::string &inCommand, unsigned int clientId, Server & server)
{
	Client *clientPtr = server.getClientById(clientId);
	if (!clientPtr)
		return ;
	Client &client = *clientPtr;

	Logger::Log(LogLevel::INFO, "Received command from " + client.getName() + ": " + inCommand);

	std::vector<std::string> parts = split(inCommand, ' ');
	if (parts.empty())
		return client.sendCodeResponse(421, "Unknown command");

	static const std::vector<CommandMapping> commandMappings = {
		{ "PASS", &CommandHandler::HandlePASS },
		{ "CAP", &CommandHandler::HandleCAP },
		{ "PING", &CommandHandler::HandlePING },
		{ "OPER", &CommandHandler::HandleOPER },
		{ "NICK", &CommandHandler::HandleNICK },
		{ "USER", &CommandHandler::HandleUSER },
		{ "JOIN", &CommandHandler::HandleJOIN },
		{ "QUIT", &CommandHandler::HandleQUIT },
		{ "PART", &CommandHandler::HandlePART },
		{ "PRIVMSG", &CommandHandler::HandlePRIVMSG },
		{ "KICK", &CommandHandler::HandleKICK },
		{ "INVITE", &CommandHandler::HandleINVITE },
		{ "TOPIC", &CommandHandler::HandleTOPIC },
		{ "MODE", &CommandHandler::HandleMODE },
		{ "DEBUG", &CommandHandler::HandleDEBUG }
	};

	for (const auto &mapping : commandMappings)
		if (parts[0] == mapping.command)
			return mapping.func(parts, client, server);

	client.sendCodeResponse(421, "Unknown command");
}

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
	ERR_NOTREGISTERED = 451,
	ERR_USERONCHANNEL = 442,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	ERR_BADCHANNELKEY = 476,
	ERR_BANNEDFROMCHAN = 474,
	ERR_BADCHANMASK = 475,
	ERR_CHANNELISFULL = 471,
	ERR_NOTOPLEVEL = 482,
	ERR_NOSUCHCHANNEL = 403,
	ERR_UNKNOWNMODE = 501,
    //RPL_AWAY = 301
};


User::User(int fd, std::string_view ip, std::string_view hostName, std::string_view serverPwd)
    :   m_fd{fd}, m_status{RegStatus::CONNECTED}, m_userIp{ip},
        m_hostName{hostName}, m_serverPwd{ serverPwd } 
{
    m_errors.emplace(Errors::ERR_NONE, "");
    m_errors.emplace(Errors::ERR_NICKMORETHAN9CHARS, ": Toomuch letters");
    m_errors.emplace(Errors::ERR_NONICKNAMEGIVEN,  ": No Nick given");
    m_errors.emplace(Errors::ERR_NICKNAMEINUSE, ": Nick in use");
    m_errors.emplace(Errors::ERR_ERRONEUSNICKNAME, ": Erroneous nickname");
    m_errors.emplace(Errors::ERR_NEEDMOREPARAMS, ": More parameters needed");
    m_errors.emplace(Errors::ERR_ALREADYREGISTERED, ": Already registered");
    m_errors.emplace(Errors::ERR_REGISTERING, ": Pass set, continue registration");
    m_errors.emplace(Errors::ERR_PASSWDMISMATCH, ": Wrong password entered");
    m_errors.emplace(Errors::ERR_NOSUCHNICK, ": No such nick");
    m_errors.emplace(Errors::ERR_CANNOTSENDTOCHAN, ": You cannot send message to channel");
    m_errors.emplace(Errors::ERR_TOOMANYTARGETS, ": Too many targets");
    m_errors.emplace(Errors::ERR_NORECIPIENT, ": You must provide a target for your message");
    m_errors.emplace(Errors::ERR_NOTEXTTOSEND, ": Message must not be empty");
	m_errors.emplace(Errors::ERR_USERNOTINCHANNEL, ": You're not in that channel");
    m_errors.emplace(Errors::ERR_UNKNOWNCOMMAND, ": Unknown command");
    m_errors.emplace(Errors::ERR_CHANOPRIVSNEEDED, ": You're not channel operator");
    m_errors.emplace(Errors::ERR_NOTONCHANNEL, ": You're not on that channel");
    m_errors.emplace(Errors::ERR_NOCHANNELNAMEGIVEN, ": No channel name given");
    m_errors.emplace(Errors::ERR_CHANNELDOESNOTEXIST, ": No such channel");

}

void    User::handleErrors(Errors errorCode, const std::string& cmd)
{
    const std::string&  error { m_errors.at(errorCode) };
    respond(buildMsg(errorCode, cmd, error));
}


void Commands::HandleJOIN(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered", "JOIN"); // ERR_NOTREGISTERED
	if (parts.size() < 2 || parts.size() > 3)
	{
		std::string response = ":irctic.com 461 JOIN :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (client.channelId && server.getChannelById(client.channelId.value()))
			response += ":irctic.com 442 " + server.getChannelById(client.channelId.value())->name + " :You are already on channel"; // ERR_USERONCHANNEL
		if (server.getChannels().size() > 0)
		{
			response += ":irctic.com 353 " + client.nickname + " = :"; // RPL_NAMREPLY
			for (auto &channel : server.getChannels())
				response += channel.name + " ";
			response += "\r\n";
			response += ":irctic.com 366 " + client.nickname + " :End of NAMES list"; // RPL_ENDOFNAMES
		}
		client.sendMessage(response);
		return ;
	}

	std::string channelName = parts[1];
	Channel *channel = server.getChannelByName(channelName);
	if (!channel)
	{
		if (channelName[0] != '#')
			return client.sendCodeResponse(476, "Invalid channel name", channelName); // ERR_BADCHANNELKEY
		server.createChannel(channelName);
		channel = server.getChannelByName(channelName);
	}

	if (channel->password != "" && (parts.size() < 3 || parts[2] != channel->password))
		return client.sendCodeResponse(475, "Cannot join channel (+k)", channelName); // ERR_BADCHANNELKEY
	
	if (channel->isKicked(client.id))
		return client.sendCodeResponse(474, "Cannot join channel (+b)", channelName); // ERR_BANNEDFROMCHAN

	channel->addMember(client.id, server);
	client.channelId = channel->id;
}

void Commands::HandleMODE(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 3 || parts.size() > 4)
		return client.sendCodeResponse(461, "Not enough parameters", "MODE");
	Channel *channel = server.getChannelByName(parts[1]);
	if (!client.isOperatorIn(channel))
		return client.sendCodeResponse(482, "You're not channel operator", parts[1]);
	if (!channel)
		return client.sendCodeResponse(403, "No such channel", parts[1]);

	std::string mode = parts[2];
	if (mode == "+i")
	{
		channel->inviteOnly = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " +i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-i")
	{
		channel->inviteOnly = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " -i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+t")
	{
		channel->anyoneCanChangeTopic = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " +t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-t")
	{
		channel->anyoneCanChangeTopic = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " -t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+k")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +k");
		channel->password = parts[3];
		channel->broadcast(":irctic.com MODE " + channel->name + " +k " + channel->password + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-k")
	{
		channel->password = "";
		channel->broadcast(":irctic.com MODE " + channel->name + " -k", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+l")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +l");
		channel->limit = std::stoi(parts[3]);
		channel->broadcast(":irctic.com MODE " + channel->name + " +l " + std::to_string(channel->limit) + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-l")
	{
		channel->limit = 0;
		channel->broadcast(":irctic.com MODE " + channel->name + " -l", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+o")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +o");
		unsigned int clientIdToOp = server.getClientIdByName(parts[3]);
		if (clientIdToOp > 0)
		{
			channel->addOperator(clientIdToOp);
			channel->broadcast(":irctic.com MODE " + channel->name + " +o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			client.sendCodeResponse(401, "No such nick/channel", parts[3]);
	}
	else if (mode == "-o")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE -o");
		unsigned int clientIdToDeop = server.getClientIdByName(parts[3]);
		if (clientIdToDeop > 0)
		{
			channel->removeOperator(clientIdToDeop);
			channel->broadcast(":irctic.com MODE " + channel->name + " -o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			return client.sendCodeResponse(401, "No such nick/channel", parts[3]);
	}
	else
		return client.sendCodeResponse(501, "Unknown MODE flag", mode);
}


User* Server::getClientByNick(const std::string& nickname)
{
    for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it) {
        if (it->second.nickname == nickname)
            return &it->second;
    }
    return NULL;
}


void Channel::setLimit(int newLimit)
{
	if (newLimit < 0)
		return ;
	limit = newLimit;
}

int Channel::getLimit() const
{
	return limit;
}