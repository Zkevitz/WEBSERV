
# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address

# Target name
NAME = webserv

# Directories
SRCDIR = src
INCDIR = include

# Source files
SRC = $(SRCDIR)/main.cpp $(SRCDIR)/Config.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/Cgi.cpp $(SRCDIR)/Msg.cpp $(SRCDIR)/Request.cpp $(SRCDIR)/utils.cpp

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

re: fclean all

.PHONY: all clean fclean re
