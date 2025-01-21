/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rheck <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 13:36:19 by rheck             #+#    #+#             */
/*   Updated: 2023/03/01 23:14:36 by rheck            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSQ_H
# define BSQ_H

# define BUF_SIZE 32

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <limits.h>

void			ft_putchar(char c);
int				ft_strlen(char *str);
int				ft_strcmp(char *s1, char *s2);
int				map_length(char **map);
int				check_chars_line(char *line, char *characters);
int				check_different_chars(char *characters);
int				check_all_same_length(char **map);
int				check_printable(char *characters);
int				check_all(char **map);
int				free_map(char ***map, int value, char *message);
int				ft_freetrun(char **data, int value);
int				err(char *message, int ret);
char			*ft_memcat(char *string, char *buf,
					int string_size, int buf_size);
char			**make_map(char *pre_map);
char			*ft_read_map(char *path);
char			*get_nblines(char *line);
char			*get_chars(char *line);
char			**ft_split(char *str, char *charset);
int				check_nb_lines(char *nb_lines);
struct	s_best	solving(int **tab, int lines, int cols, int p[3]);
char			*read_user_map(char *pre_map);
char			**fill_tab(char **map, struct s_best best, char *chars);
int				**transform_map(char **char_map, int rows, int cols);
void			ft_display_map(char **map);
int				get_next_line(int fd, char **line);
int				int_lines(char *nblines);
int				solve(char *path);
int				complet_line(char **line, char (*rest)[BUF_SIZE + 1]);
int				reading(char (*line)[BUF_SIZE + 1], int fd);
int				check_line(char *line);
int				check_char(char **char_map, int i, int j, char *chars);
void			ft_dsiplay_map(char **map);
void			add_fake(int **obstacle_map, int rows, int cols);
int				free_tab(int ***map, int size, int value, char *message);
int				*init_sbcd(int size);
int				*init_params(void);
int				check_header(char *header);
int				number_line_check(int lines, char **map);
int				user_way(char **map);
struct s_best	retfree(struct s_best cool, int p[3]);
int				malade(int r, int sbcd[4]);

typedef struct s_best
{
	int		x;
	int		y;
	int		size;
}	t_best;

#endif
