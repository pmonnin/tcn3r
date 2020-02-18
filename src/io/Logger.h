#ifndef TCN3R_LOGGER_H
#define TCN3R_LOGGER_H


#include <iostream>
#include <string>

enum LoggingLevel {
    DEBUG = 0b11111,
    INFO = 0b11110,
    WARNING = 0b11100,
    ERROR = 0b11000,
    CRITICAL = 0b10000
};

class Logger {
    public:
        explicit Logger(LoggingLevel level);
        void critical(const std::string &message) const;
        void error(const std::string &message) const;
        void warning(const std::string &message) const;
        void info(const std::string &message) const;
        void debug(const std::string &message) const;

    private:
        void log(const std::string &level, const std::string &message, std::ostream &stream) const;
        LoggingLevel m_level;
};


#endif //TCN3R_LOGGER_H
