#ifndef	CHANNEL_HPP
#define	CHANNEL_HPP

#include "Client.hpp"
#include <map>

class Server;

class Channel
{
	private:
		std::map<unsigned int, bool> _members;
		std::map<unsigned int, bool> _kicked;
		std::map<unsigned int, bool> _operators;
		std::map<unsigned int, bool> _invites;

	public:
		Channel() = delete;
		Channel(std::string name, unsigned int id);
		Channel(const Channel &other) = default;
		Channel(Channel &&other) = default;
		Channel & operator=(const Channel &other) = default;
		~Channel() = default;

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
		bool isMember(unsigned int clientId);
		std::string getPassword();
		void setPassword(std::string newPassword);
		void setTopic(std::string newTopic);
		std::string getTopic();
		void setLimit(int newLimit);
		int getLimit() const;



		std::string getInfoString();

		bool isKicked(unsigned int client);
};

#endif