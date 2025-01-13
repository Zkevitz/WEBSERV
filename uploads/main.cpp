#include "iter.hpp"

int main()
{

	int num[5] = {0, 1, 2, 3, 4};
	std::string str[4] = {"how", "are", "you", "?"};
	char strnum[6] = "abcde";

	iter(num, 5, print);
	std::cout << std::endl;
	iter(str, 4, print);
	std::cout << std::endl;
	iter(strnum, 5, print);

	return 0;
}