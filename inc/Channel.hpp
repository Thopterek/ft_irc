#ifndef	CHANNEL_HPP
#define	CHANNEL_HPP

#include "Client.hpp"

class Server;

class Channel
{
	private:
		std::map<unsigned int, bool> _members;
		std::map<unsigned int, bool> _kicked;
		std::map<unsigned int, bool> _operators;
		std::map<unsigned int, bool> _invites;

	public:
		Channel(std::string name, unsigned int id);

		unsigned int id;

		std::string name = "";
		std::string topic = "";
		std::string password = "";
		bool inviteOnly = false;
		bool anyoneCanChangeTopic = true;
		int limit = 0;

		void send_msg(std::string msg, Server &server, Client * client = nullptr);

		void addMember(unsigned int client, Server &server);
		std::string removeMember(unsigned int clientId, Server &server);
		std::string inviteMember(unsigned int clientId, Server &server);
		std::map<unsigned int, bool> & getMembers();
	
		void kick(unsigned int clientId, Server &server);
		void unkick(unsigned int clientId);

		void addOperator(unsigned int clientId);
		void removeOperator(unsigned int clientId);
		bool isOperator(unsigned int clientId);

		std::string getInfoString();

		bool isKicked(unsigned int client) { return _kicked[client]; }
};

#endif