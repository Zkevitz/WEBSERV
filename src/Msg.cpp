#include "../include/Msg.hpp"
#include "../include/Webserv.hpp"

std::string Msg::file_name = "logfile.txt";
bool Msg::state = true;


void    Msg::logMsg(const char *color, Mode m, const char *msg, ...)
{
    char        output[8192];
    va_list     args;
    int         n;

    if (state == true)
    {
        va_start(args, msg);
        n = vsnprintf(output, 8192, msg, args);
        std::string date = getCurrTime();
        if (m == FILE_OUTPUT)
        {
            if (mkdir("logs", 0777) < 0 && errno != EEXIST)
            {
                std::cerr << "mkdir() Error: " << strerror(errno) << std::endl;
                return ;
            }
            int fd = open(("logs/" + file_name).c_str(), O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR); // S_IRUSR | S_IWUSR option de permission en cas de creation du fichier avec 0_CREAT
            write(fd, date.c_str(), date.length());
            write(fd, "   ", 3);
            write(fd, output, n);
            write(fd, "\n", 1);
            close(fd);
        }
        else if (m == CONSOLE_OUTPUT)
        {
            std::cout << color << getCurrTime() << output << RESET << std::endl;
        }      
        va_end(args);
    }
}

std::string Msg::getCurrTime()
{
    tzset(); // set variable d'environnement TZ lier au fuseau horaire pour une bonne application dans gmtime
    char date[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now); // convertit le temps en seconde en UTC(temps universel coordonne)
    tm.tm_hour = tm.tm_hour + GST;
    strftime(date, sizeof(date), "[%Y-%m-%d  %H:%M:%S]   ", &tm);
    return (std::string(date));
}