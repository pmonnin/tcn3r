#ifndef TCN3R_TTLWRITER_H
#define TCN3R_TTLWRITER_H


#include <fstream>
#include <string>

#include "Logger.h"

class TTLWriter
{
    public:
        TTLWriter(const std::string &filePath, const Logger &logger);
        ~TTLWriter();
        void writeTriple(const std::string &subject, const std::string &predicate, const std::string &object);

    private:
        std::ofstream m_fileStream;
};


#endif //TCN3R_TTLWRITER_H
