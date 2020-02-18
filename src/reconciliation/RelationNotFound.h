#ifndef TCN3R_RELATIONNOTFOUND_H
#define TCN3R_RELATIONNOTFOUND_H


#include <exception>
#include <string>

class RelationNotFound : public std::exception
{
    public:
        RelationNotFound(std::string message) noexcept;
        virtual ~RelationNotFound() noexcept;
        virtual const char* what() const noexcept;

    private:
        const std::string m_message;
};


#endif //TCN3R_RELATIONNOTFOUND_H
