#include "../include/Webserv.hpp"
std::string myItoa(int to_translate)
{
    std::stringstream ss;
    ss << to_translate;
    std::string str = ss.str();
    std::cout << "Converted string: " << str << std::endl;
    return str;
}

bool fileExists (const std::string& f) 
{
    std::ifstream file(f.c_str());
    return (file.good());
}