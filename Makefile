EXEC = ircserv
COMP = c++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -std=c++17
SRCS =	src/main.cpp src/channel/Channel.cpp src/server/Server.cpp \
		src/client/Client.cpp src/client/User.cpp src/util/Util.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(COMP) $(CXXFLAGS) -o $(EXEC) $(OBJS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(EXEC)

re: fclean all

.PHONY: all re clean fclean