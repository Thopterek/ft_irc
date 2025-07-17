#ifndef	CHANNEL_HPP
#define	CHANNEL_HPP

#include "User.hpp"

class Channel
{
	private:
		std::map<unsigned int, bool> _members;
		std::map<unsigned int, bool> _kicked;
		std::map<unsigned int, bool> _operators;
		std::map<unsigned int, bool> _invites;

	public:
		Channel() = delete;
		Channel(std::string set_name, unsigned int set_id);
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

		void broadcast(const std::string &msg, User &user);

		void addMember(unsigned int client);
		void removeMember(unsigned int clientId);
		//std::string inviteMember(unsigned int clientId);
		void inviteMember(unsigned int clientId);
		std::map<unsigned int, bool> & getMembers();
	
		void kick(unsigned int clientId);
		void unkick(unsigned int clientId);

		void addOperator(unsigned int clientId);
		void removeOperator(unsigned int clientId);
		/*
			is someone somone checks
		*/
		bool setInviteOnly(bool newinviteOnly);
		bool isInviteOnly() const;
		bool isTopicProtected() const;
		void setTopicProtection(bool newtopic);
		bool isOperator(unsigned int clientId);
		bool isMember(unsigned int clientId);
		bool isInvited(unsigned int clientId);
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