#ifndef ITER_HPP
# define ITER_HPP

# include <iostream>

template <typename T>
void print(T &var)
{
	std::cout << var << std::endl;
};

template <typename T>
void iter(T *array, size_t len, void (*func)(T const &array))
{
	for (size_t i = 0; i < len; i++)
		func(array[i]);
};

#endif