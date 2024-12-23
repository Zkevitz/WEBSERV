#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <iostream>
# include <cstddef>

template <typename T>

class Array {
	
	private :
		T *arr;
		unsigned int n;
	
	public :
		Array<T>();
		Array<T>(unsigned int n);
		Array<T>(const Array &src);
		T &operator[](unsigned int i)const;
		T &operator=(const Array &src);
		~Array<T>();

		unsigned int size()const;

		class OutOfBounds : public std::exception
		{
			public:
				virtual	const char *what() const throw();
		};
};
template <typename T>
Array<T>::Array(): arr(), n(0){
	std::cout << "[C] default constructor called with empty array" << std::endl;
}

template <typename T>
Array<T>::Array(unsigned int n):arr(new T[n]), n(n){
	std::cout << "[C] array constructor called" << std::endl;
}

template <typename T>
Array<T>::Array(const Array &src):arr(new T[src.size()]), n(src.size()){
	std::cout << "[C] copy constructor called" << std::endl;
	for(unsigned int i = 0; i < src.size(); i++)
		this->arr[i] = src[i];
}

template <typename T>
T &Array<T>::operator[](unsigned int i)const
{
	if (i >= this->size())
		throw OutOfBounds();
	return this->arr[i];
}

template <typename T>
T &Array<T>::operator=(const Array &src)
{
	if (this->n > 0)
		delete[] arr;
	arr = NULL;

	arr = new T[src.size()];
	for (int i = 0; i < src.size(); i++)
		arr[i] = src[i];
	return *this;
}

template <typename T>
Array<T>::~Array()
{
	if(this->arr)
		delete[] arr;
	std::cout << "[D] array destructor called" << std::endl;
}

template <typename T>
unsigned int Array<T>::size()const
{
	return(n);
};

template <typename T>
const char *Array<T>::OutOfBounds::what(void) const throw()
{
	return ("Out of bonds");
};
#endif