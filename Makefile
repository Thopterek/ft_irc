EXEC = ircserv
COMP = c++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -std=c++17
SRCS =	src/main.cpp src/protocol/Protocol.cpp src/server/Server.cpp \
		src/server/check_server.cpp src/client/Client.cpp src/util/Util.cpp
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