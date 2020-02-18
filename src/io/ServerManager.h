#ifndef TCN3R_SERVERMANAGER_H
#define TCN3R_SERVERMANAGER_H


#include <set>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>
#include <curl/curl.h>

#include "../configuration/Configuration.h"
#include "../io/Logger.h"

class ServerManager
{
    public:
        explicit ServerManager(Configuration const &parameters, Logger const &logger);
        boost::property_tree::ptree query(std::string const &sparqlQuery) const;
        int queryCountElements(const std::string &whereClause) const;
        int queryCountTwoElements(const std::string &whereClause) const;
        std::set<std::string> queryElements(const std::string &whereClause) const;
        std::set<std::pair<std::string, std::string>> queryTwoElements(const std::string &whereClause) const;

    private:
        static std::string escapeUrl(CURL *curl, std::string const &url);

        Configuration const &m_parameters;
        Logger const &m_logger;
};

size_t queryCallback(char *ptr, size_t size, size_t nmemb, std::string *queryResponse);


#endif //TCN3R_SERVERMANAGER_H
