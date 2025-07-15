EXEC = ircserv
EXBOT = ircbot
COMP = c++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -std=c++17 -g -Wfatal-errors
SRCS =	src/main.cpp src/channel/Channel.cpp src/server/Server.cpp \
		src/client/Client.cpp src/client/User.cpp \
		src/client/Parser.cpp src/cmds/nick.cpp src/cmds/pass.cpp \
		src/cmds/user.cpp src/cmds/privmsg.cpp \
		src/cmds/invite.cpp src/cmds/join.cpp src/cmds/kick.cpp src/cmds/ping.cpp \
		src/cmds/mode.cpp src/cmds/part.cpp src/cmds/quit.cpp src/cmds/topic.cpp


BOTSRC = bot/botSetup.cpp bot/Bot.cpp

OBJS = $(SRCS:.cpp=.o)

BOTOBJ = $(BOTSRC:.cpp=.o)


all: $(EXEC)

$(EXEC): $(OBJS)
	$(COMP) $(CXXFLAGS) -o $(EXEC) $(OBJS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(EXEC)

re: fclean all

bot: $(EXBOT)

$(EXBOT): $(BOTOBJ)
	$(COMP) $(CXXFLAGS) -o $(EXBOT) $(BOTOBJ)


irssi-docker:
	@docker ps > /dev/null 2>&1 || ( \
		echo "🚀 Docker Desktop is not running. Trying to start Docker Desktop..." && \
		open -a Docker && \
		printf "⏳ Waiting for Docker Desktop to start " && \
		while ! docker ps > /dev/null 2>&1; do \
			printf "."; \
			sleep 1; \
		done; \
		echo "\n✅ Docker Desktop is ready!" \
	)
	@docker run -it --rm --name irssiclient ubuntu bash -c "\
		apt update && \
		apt install -y irssi && \
		bash"

cleanbot:
	@rm -f $(BOTOBJ)

fcleanbot: cleanbot
	@rm -f $(EXBOT)

rebot: fcleanbot bot

.PHONY: all re clean fclean bot rebot cleanbot fcleanbot irssi-docker