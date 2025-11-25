//
// Created by sera on 10/19/24.
//

#ifndef MAIL_HPP
#define MAIL_HPP
#include <string>
#include <ctime>

struct Mail {
    std::string filename;
    std::string sender;
    std::string subject;
    std::string body;
    std::time_t timestamp;
};

#endif //MAIL_HPP
