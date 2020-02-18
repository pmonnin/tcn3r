#include <chrono>
#include <ctime>

#include "Logger.h"


Logger::Logger(LoggingLevel level) : m_level(level)
{

}

void Logger::critical(const std::string &message) const
{
    if (m_level & CRITICAL)
        log("CRITICAL", message, std::cerr);
}

void Logger::error(const std::string &message) const
{
    if (m_level & ERROR)
        log("ERROR", message, std::cerr);
}

void Logger::warning(const std::string &message) const
{
    if (m_level & WARNING)
        log("WARNING", message, std::cerr);
}

void Logger::info(const std::string &message) const
{
    if (m_level & INFO)
        log("INFO", message, std::cout);
}

void Logger::debug(const std::string &message) const
{
    if (m_level & DEBUG)
        log("DEBUG", message, std::cout);
}

void Logger::log(const std::string &level, const std::string &message, std::ostream &stream) const
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string current_time = std::ctime(&now);
    current_time.erase(current_time.find('\n', 0));
    stream << "[" << current_time  << "][" <<  level << "] " << message << std::endl;
}
