#ifndef MSG_HPP
# define MSG_HPP


#include "Webserv.hpp"

#define RESET          "\x1B[0m"
#define RED            "\x1B[31m"
#define LIGHT_RED      "\x1B[91m"
#define WHITE          "\x1B[37m"
#define BLINK           "\x1b[5m"
#define YELLOW         "\x1B[33m"
#define LIGHT_BLUE     "\x1B[94m"
#define CYAN           "\x1B[36m"
#define DARK_GREY      "\x1B[90m"
#define LIGHTMAGENTA   "\x1B[95m"

#define GST 4 // valeur a verifier temps pour le decalage UTC;

enum Mode{
    CONSOLE_OUTPUT,
    FILE_OUTPUT
};

class Msg {
    public:
        static std::string file_name;
        static bool state;


        static void         setFilenName(std::string Filename);
        static void         setState(bool state);

        static void         logMsg(const char *color, Mode m, const char *msg, ...);
        static std::string  getCurrTime();

};

#endif 