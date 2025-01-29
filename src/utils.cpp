#include "../include/Webserv.hpp"
std::string myItoa(int to_translate)
{
    std::stringstream ss;
    ss << to_translate;
    std::string str = ss.str();
    return str;
}

bool fileExists (const std::string& f) 
{
    std::ifstream file(f.c_str());
    return (file.good());
}

bool is_directory(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    return S_ISDIR(path_stat.st_mode);  // Vérifie si c'est un dossier
}