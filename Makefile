EXEC = ircserv
EXBOT = ircbot
COMP = c++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -std=c++17 -g
SRCS =	src/main.cpp src/channel/Channel.cpp src/server/Server.cpp \
		src/client/Client.cpp src/client/User.cpp src/util/Util.cpp \
		src/client/Parser.cpp src/client/nick.cpp

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

cleanbot:
	@rm -f $(BOTOBJ)

fcleanbot: cleanbot
	@rm -f $(EXBOT)

rebot: fcleanbot bot

.PHONY: all re clean fclean bot rebot cleanbot fcleanbot