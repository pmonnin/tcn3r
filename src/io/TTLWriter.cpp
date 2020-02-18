#include "TTLWriter.h"

TTLWriter::TTLWriter(const std::string &filePath, const Logger &logger) : m_fileStream(filePath)
{
    if (!m_fileStream)
    {
        logger.critical("Not possible to open output file: " + filePath);
        std::exit(-1);
    }
}

TTLWriter::~TTLWriter()
{
    m_fileStream.close();
}

void TTLWriter::writeTriple(const std::string &subject, const std::string &predicate,
                            const std::string &object)
{
    m_fileStream << "<" << subject << "> <" << predicate << "> <" << object << "> ." << std::endl;
}
