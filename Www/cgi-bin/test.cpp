#include <iostream>
int main(void)
{
    std::cout << "Content-Type: text/html\r\n\r\n";
    std::cout << "Status: 200 OK\r\n\r\n";
    std::cout << "<html><head><title>Hello CGI</title></head><body>";
    std::cout << "<h1>Hello, World!</h1>";
    // Afficher l'heure actuelle
    std::time_t now = std::time(nullptr);
    std::cout << "<p>Current server time: " << std::asctime(std::localtime(&now)) << "</p>";
    
    // Finir la page HTML
    std::cout << "</body></html>";
}