void    join(Client& client, int fd, std::vector<std::string> param)
{
	User&       user { client[fd] };
    if (param.size() < 2)
    {
        user.sendCodeResponse(461, "Not enough parameters", "JOIN");
        return;
    }
    if (!channel)
    {

        Channel& newChannel = server.createChannel(chanName, user.getId());
        newChannel.addOperator(user.getId());
        newChannel.addMember(user.getId(), server);

        std::string msg = ":" + user.getFullPrefix() + " JOIN :" + chanName + "\r\n";
        newChannel.send_msg(msg, server, NULL);
        return;
    }
    std::string channelName = param[1];
    Server& server = user.getServer();
    Channel* channel = server.getChannelByName(channelName);
    if (!channel)
    {
        user.sendCodeResponse(403, "No such channel", channelName);
        return;
    }
    if (channel->inviteOnly && !channel->isOperator(user.id) && !channel->isInvited(user.id))
    {
        user.sendCodeResponse(473, "Cannot join channel (+i)", channelName);
        return;
    }
    if (channel->isKicked(user.id))
    {
        user.sendCodeResponse(474, "Cannot join channel (+b)", channelName);
        return;
    }
    if (channel->password != "" && (param.size() < 3 || param[2] != channel->password))
    {
        user.sendCodeResponse(475, "Cannot join channel (+k)", channelName);
        return;
    }
    channel->addMember(user.id, server);
    user.channelId = channel->id;
    user.respond(331, "Channel joined", channelName); // RPL_NOTOPIC
    channel->send_msg(":" + user.getNickname() + " JOIN " + channelName + "\r\n", server, &user);
}