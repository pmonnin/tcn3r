#include <iostream>

#include <boost/progress.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "ServerManager.h"


ServerManager::ServerManager(Configuration const &parameters, Logger const &logger) : m_parameters(parameters), m_logger(logger)
{

}

boost::property_tree::ptree ServerManager::query(std::string const &sparqlQuery) const
{
    CURL *curl;
    boost::property_tree::ptree jsonTree;

    curl = curl_easy_init();
    if (curl)
    {
        bool done(false);

        // Prepare URL
        std::string url = m_parameters.getServerAddress() + "?"
                + escapeUrl(curl, m_parameters.getServerJsonAttribute()) + "="
                + escapeUrl(curl, m_parameters.getServerJsonValue()) + "&"
                + escapeUrl(curl, m_parameters.getServerGraphAttribute()) + "="
                + escapeUrl(curl, m_parameters.getServerGraphValue()) + "&"
                + escapeUrl(curl, m_parameters.getServerQueryAttribute()) + "="
                + escapeUrl(curl, sparqlQuery);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set up timeout
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, m_parameters.getServerTimeout());

        // Set up authentication (if needed)
        if (!m_parameters.getServerUsername().empty() && !m_parameters.getServerPassword().empty())
        {
            std::string authentication = m_parameters.getServerUsername() + ":" + m_parameters.getServerPassword();
            curl_easy_setopt(curl, CURLOPT_USERPWD, authentication.c_str());
        }

        // Set up content receiver
        std::string queryResponse;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, queryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &queryResponse);

        while (!done)
        {
            done = true;

            // Perform query
            CURLcode res = curl_easy_perform(curl);

            // Check potential errors
            if (res != CURLE_OK)
            {
                m_logger.critical("CURL error " + std::string(curl_easy_strerror(res)));
                std::exit(-1);
            }

            else
            {
                // Check HTTP status code
                long httpCode(0);
                res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

                if (res == CURLE_OK)
                {
                    if (httpCode / 100 == 2) // 2xx codes
                    {
                        std::stringstream jsonStream;
                        jsonStream << queryResponse;
                        boost::property_tree::read_json(jsonStream, jsonTree);
                    }

                    else if (httpCode / 100 == 3) // 3xx codes
                    {
                        char *location(nullptr);
                        res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &location);

                        if (res == CURLE_OK && location)
                        {
                            done = false;
                            curl_easy_setopt(curl, CURLOPT_URL, location);
                            queryResponse = "";
                        }

                        else
                        {
                            m_logger.critical("CURL error " + std::string(curl_easy_strerror(res)));
                            std::exit(-1);
                        }
                    }

                    else if (httpCode == 404) // 404 code
                    {
                        done = false;
                        m_logger.warning("New try after 404 error while querying triplestore");
                    }

                    else
                    {
                        m_logger.critical("HTTP error while querying triplestore " + std::to_string(httpCode));
                        std::exit(-1);
                    }
                }

                else
                {
                    m_logger.critical("CURL error " + std::string(curl_easy_strerror(res)));
                    std::exit(-1);
                }
            }
        }

        curl_easy_cleanup(curl);
    }

    return jsonTree;
}

int ServerManager::queryCountElements(const std::string& whereClause) const
{
    boost::property_tree::ptree jsonTree = query("SELECT COUNT(DISTINCT ?e) as ?count WHERE { " + whereClause + " } ");
    boost::property_tree::ptree::value_type &binding = *jsonTree.get_child("results.bindings").begin();
    return binding.second.get<int>("count.value");
}

int ServerManager::queryCountTwoElements(const std::string &whereClause) const
{
    boost::property_tree::ptree jsonTree = query("SELECT COUNT(*) as ?count WHERE { "
                                                 "SELECT DISTINCT ?e1 ?e2 WHERE { " + whereClause + " }}");
    boost::property_tree::ptree::value_type &binding = *(jsonTree.get_child("results.bindings").begin());
    return binding.second.get<int>("count.value");
}

std::set<std::string> ServerManager::queryElements(const std::string &whereClause) const
{
    std::set<std::string> elements;
    auto elementsCount = static_cast<unsigned int>(queryCountElements(whereClause));

    while (elements.size() != elementsCount)
    {
        elements.clear();
        unsigned int offset(0);
        boost::progress_display progressBar(elementsCount);

        while (offset <= elementsCount)
        {
            std::ostringstream queryStream;
            queryStream << "SELECT DISTINCT ?e WHERE { " << whereClause << " } ";
            queryStream << "LIMIT " << m_parameters.getServerMaxRows() << " OFFSET " << offset;
            boost::property_tree::ptree jsonTree = query(queryStream.str());

            for (boost::property_tree::ptree::value_type &binding : jsonTree.get_child("results.bindings"))
            {
                elements.insert(binding.second.get<std::string>("e.value"));
                ++progressBar;
            }

            offset += m_parameters.getServerMaxRows();
        }

        if (elements.size() != elementsCount)
        {
            std::cerr << "Number of elements different from count, retry..." << std::endl;
        }
    }

    return elements;
}

std::set<std::pair<std::string, std::string>> ServerManager::queryTwoElements(const std::string &whereClause) const
{
    std::set<std::pair<std::string, std::string>> elements;
    auto elementsCount = static_cast<unsigned int>(queryCountTwoElements(whereClause));

    while (elements.size() != elementsCount)
    {
        elements.clear();
        unsigned int offset(0);
        boost::progress_display progressBar(elementsCount);

        while (offset <= elementsCount)
        {
            std::ostringstream queryStream;
            queryStream << "SELECT DISTINCT ?e1 ?e2 WHERE { " << whereClause << " } ";
            queryStream << "LIMIT " << m_parameters.getServerMaxRows() << " OFFSET " << offset;
            boost::property_tree::ptree jsonTree = query(queryStream.str());

            for (boost::property_tree::ptree::value_type &binding : jsonTree.get_child("results.bindings"))
            {
                elements.insert(std::pair<std::string, std::string>(
                        binding.second.get<std::string>("e1.value"),
                        binding.second.get<std::string>("e2.value"))
                        );
                ++progressBar;
            }

            offset += m_parameters.getServerMaxRows();
        }

        if (elements.size() != elementsCount)
        {
            std::cerr << "Number of elements different from count, retry..." << std::endl;
        }
    }

    return elements;
}

std::string ServerManager::escapeUrl(CURL *curl, std::string const &url)
{
    std::string escapedUrl;

    if (curl)
    {
        char *output = curl_easy_escape(curl, url.c_str(), 0);
        if (output)
        {
            escapedUrl = std::string(output);
            curl_free(output);
        }
    }

    return escapedUrl;
}

size_t queryCallback(char *ptr, size_t size, size_t nmemb, std::string *queryResponse)
{
    *queryResponse += std::string(ptr);
    return nmemb;
}
