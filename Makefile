
# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

# Target name
NAME = webserv

# Directories
SRCDIR = src
INCDIR = include

# Source files
# Rules
SRC = $(SRCDIR)/main.cpp $(SRCDIR)/Config.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/Cgi.cpp $(SRCDIR)/Server_request.cpp $(SRCDIR)/Msg.cpp $(SRCDIR)/Request.cpp $(SRCDIR)/utils.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	rm -rf logs/*

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)
	rm -rf logs/*

re: fclean all

.PHONY: all clean fclean re
