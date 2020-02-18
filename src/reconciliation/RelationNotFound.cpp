#include "RelationNotFound.h"

#include <utility>

RelationNotFound::RelationNotFound(std::string message) noexcept : std::exception(), m_message(std::move(message))
{

}

RelationNotFound::~RelationNotFound() noexcept
= default;

const char* RelationNotFound::what() const noexcept
{
    return m_message.c_str();
}
