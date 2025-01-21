NAME		=	bsq	

SRCS_NAME	=	main.c \
				ft_split.c \
				ft_is_all_valid.c \
				ft_is_valid_map.c \
				read_map.c \
				util_functions.c \
				util_functions2.c \
				display_map.c \
				get_line_support.c \
				header_check.c \
				solve.c \

INC_NAME	=	bsq.h

SRCS		=	$(addprefix srcs/, $(SRCS_NAME))
INC			=	$(addprefix includes/, $(INC_NAME))

OBJS		=	$(SRCS:.c=.o)

CC 			=	gcc
CFLAGS		=	-Wall -Wextra -Werror

all		:	$(NAME)

$(NAME)	:	$(OBJS) $(INC)
			$(CC) $(CFLAGS) $(OBJS) -o $@

clean	:
			rm -f $(OBJS)

fclean	:	clean
			rm -f $(NAME)

re		:	fclean all
