#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Configuration.h"


Configuration::Configuration(const std::string &configFilePath, int maxRows, int threadsNumber, std::string output,
                             bool explainMode, int nonEmptyDimensionLimit, int comparableDimensionLimit,
                             double similarityLimit, Logger const &logger) : m_threadsNumber((threadsNumber > 0) ? threadsNumber : 1),
                                                                             m_outputPath(std::move(output)),
                                                                             m_explainMode(explainMode),
                                                                             m_nonEmptyDimensionLimit(nonEmptyDimensionLimit),
                                                                             m_comparableDimensionLimit(comparableDimensionLimit),
                                                                             m_similarityLimit(similarityLimit),
                                                                             m_serverMaxRows((maxRows > 0) ? maxRows : 10000),
                                                                             m_relationTypes(),
                                                                             m_dimensions()
{
    // Parse configuration file
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(configFilePath, pt);

    try
    {
        // Parse server configuration
        m_serverAddress = pt.get<std::string>("server-address");
        m_serverJsonAttribute = pt.get<std::string>("url-json-conf-attribute");
        m_serverJsonValue = pt.get<std::string>("url-json-conf-value");
        m_serverGraphAttribute = pt.get<std::string>("url-default-graph-attribute");
        m_serverGraphValue = pt.get<std::string>("url-default-graph-value");
        m_serverQueryAttribute = pt.get<std::string>("url-query-attribute");
        m_serverUsername = pt.get<std::string>("username", "");
        m_serverPassword = pt.get<std::string>("password", "");
        m_serverTimeout = pt.get<int>("timeout", 10000);

        // Relation types
        for (boost::property_tree::ptree::value_type &t : pt.get_child("relation-types"))
        {
            m_relationTypes.insert(t.second.data());
        }

        // Dimensions
        for (boost::property_tree::ptree::value_type &d: pt.get_child("dimensions"))
        {
            m_dimensions.insert(std::pair<std::string, const DimensionConfiguration>(d.first, DimensionConfiguration(d.first, d.second, logger)));
        }

        // Parse output predicates
        m_outputPredEqual = pt.get<std::string>("output-pred-equal");
        m_outputPredEquiv = pt.get<std::string>("output-pred-equiv");
        m_outputPredLeq = pt.get<std::string>("output-pred-leq");
        m_outputPredGeq = pt.get<std::string>("output-pred-geq");
        m_outputPredComparable = pt.get<std::string>("output-pred-comparable");
        m_outputPredDependencyRelated = pt.get<std::string>("output-pred-dependency-related");
    }
    catch (boost::property_tree::ptree_bad_path &e)
    {
        logger.critical("Bad configuration file (bad field): " + std::string(e.what()));
        std::exit(-1);
    }
    catch (boost::property_tree::ptree_bad_data &e)
    {
        logger.critical("Bad configuration file (bad data type): " + std::string(e.what()));
        std::exit(-1);
    }
}

std::string Configuration::toString() const
{
    std::string configurationString = "-- Configuration --\n";
    configurationString += "Number of threads for comparison: " + std::to_string(m_threadsNumber) + "\n";
    configurationString += "Output in: " + m_outputPath + "\n";

    if (m_explainMode)
        configurationString += "Mode: explain\n";
    else
        configurationString += "Mode: batch\n";

    configurationString += "Non empty dimension limit: " + std::to_string(m_nonEmptyDimensionLimit) + "\n";
    configurationString += "Comparable non-empty dimension limit: " + std::to_string(m_comparableDimensionLimit) + "\n";
    configurationString += "Similarity mean between non-empty dimension limit: " + std::to_string(m_similarityLimit) + "\n";

    configurationString += "Server address: " + m_serverAddress + "\n";
    configurationString += "timeout = " + std::to_string(m_serverTimeout) + "\n";
    configurationString += "max_rows = " + std::to_string(m_serverMaxRows) + "\n";
    configurationString += m_serverJsonAttribute + " = " + m_serverJsonValue + "\n";
    configurationString += m_serverGraphAttribute + " = " + m_serverGraphValue + "\n";
    configurationString += "Query attribute: " + m_serverQueryAttribute + "\n";

    if (hasServerAuthentication())
        configurationString += "Authentication set to user " + m_serverUsername + "\n";
    else
        configurationString += "No authentication\n";

    configurationString += "Output predicates:\n";
    configurationString += "=: " + m_outputPredEqual + "\n";
    configurationString += "~: " + m_outputPredEquiv + "\n";
    configurationString += "<=: " + m_outputPredLeq + "\n";
    configurationString += ">=: " + m_outputPredGeq + "\n";
    configurationString += "comparable: " + m_outputPredComparable + "\n";
    configurationString += "dependency-related" + m_outputPredDependencyRelated + "\n";

    configurationString += "Relation types:\n";
    for (const auto &t : m_relationTypes)
    {
        configurationString += t + "\n";
    }

    configurationString += "Dimensions:\n";
    for (const auto &d : m_dimensions)
    {
        configurationString += "-- " + d.first + "\n";
        configurationString += d.second.toString();
    }

    configurationString += "-------------------\n";

    return configurationString;
}

int Configuration::getThreadsNumber() const
{
    return m_threadsNumber;
}

std::string Configuration::getOutputPath() const
{
    return m_outputPath;
}

bool Configuration::isExplainMode() const
{
    return m_explainMode;
}

int Configuration::getNonEmptyDimensionLimit() const
{
    return m_nonEmptyDimensionLimit;
}

int Configuration::getComparableDimensionLimit() const
{
    return m_comparableDimensionLimit;
}

double Configuration::getSimilarityLimit() const
{
    return m_similarityLimit;
}

bool Configuration::hasServerAuthentication() const
{
    return !m_serverUsername.empty() && !m_serverUsername.empty();
}

std::string Configuration::getServerAddress() const
{
    return m_serverAddress;
}

std::string Configuration::getServerJsonAttribute() const
{
    return m_serverJsonAttribute;
}

std::string Configuration::getServerJsonValue() const
{
    return m_serverJsonValue;
}

std::string Configuration::getServerGraphAttribute() const
{
    return m_serverGraphAttribute;
}

std::string Configuration::getServerGraphValue() const
{
    return m_serverGraphValue;
}

std::string Configuration::getServerQueryAttribute() const
{
    return m_serverQueryAttribute;
}

std::string Configuration::getServerUsername() const
{
    return m_serverUsername;
}

std::string Configuration::getServerPassword() const
{
    return m_serverPassword;
}

int Configuration::getServerTimeout() const
{
    return m_serverTimeout;
}

int Configuration::getServerMaxRows() const
{
    return m_serverMaxRows;
}

std::set<std::string> Configuration::getRelationTypes() const
{
    return m_relationTypes;
}

std::map<std::string, const DimensionConfiguration> Configuration::getDimensions() const
{
    return m_dimensions;
}

std::set<std::string> Configuration::getPredicatesInDimensions() const
{
    std::set<std::string> predicates;

    for (const auto &d : m_dimensions)
    {
        std::set<std::string> predicatesInDimension = d.second.getUsedPredicates();
        predicates.insert(predicatesInDimension.begin(), predicatesInDimension.end());
    }

    return predicates;
}

std::string Configuration::getOutputPredEqual() const
{
    return m_outputPredEqual;
}

std::string Configuration::getOutputPredEquiv() const
{
    return m_outputPredEquiv;
}

std::string Configuration::getOutputPredLeq() const
{
    return m_outputPredLeq;
}

std::string Configuration::getOutputPredGeq() const
{
    return m_outputPredGeq;
}

std::string Configuration::getOutputPredComparable() const
{
    return m_outputPredComparable;
}

std::string Configuration::getOutputPredDependencyRelated() const
{
    return m_outputPredDependencyRelated;
}

std::ostream& operator<<(std::ostream &stream, Configuration const &configuration)
{
    stream << configuration.toString() << std::endl;
    return stream;
}
