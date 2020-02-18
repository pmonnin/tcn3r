#ifndef TCN3R_CONFIGURATION_H
#define TCN3R_CONFIGURATION_H


#include <iostream>
#include <map>
#include <set>
#include <string>
#include <set>


#include "DimensionConfiguration.h"
#include "../io/Logger.h"

class Configuration
{
    public:
        Configuration(const std::string &configFilePath, int maxRows, int threadsNumber, std::string output,
                      bool explainMode, int nonEmptyDimensionLimit, int comparableDimensionLimit,
                      double similarityLimit, Logger const &logger);

        int getThreadsNumber() const;
        std::string getOutputPath() const;
        bool isExplainMode() const;
        int getNonEmptyDimensionLimit() const;
        int getComparableDimensionLimit() const;
        double getSimilarityLimit() const;

        std::string toString() const;

        bool hasServerAuthentication() const;
        std::string getServerAddress() const;
        std::string getServerJsonAttribute() const;
        std::string getServerJsonValue() const;
        std::string getServerGraphAttribute() const;
        std::string getServerGraphValue() const;
        std::string getServerQueryAttribute() const;
        std::string getServerUsername() const;
        std::string getServerPassword() const;
        int getServerTimeout() const;
        int getServerMaxRows() const;

        std::set<std::string> getRelationTypes() const;

        std::map<std::string, const DimensionConfiguration> getDimensions() const;
        std::set<std::string> getPredicatesInDimensions() const;

        std::string getOutputPredEqual() const;
        std::string getOutputPredEquiv() const;
        std::string getOutputPredLeq() const;
        std::string getOutputPredGeq() const;
        std::string getOutputPredComparable() const;
        std::string getOutputPredDependencyRelated() const;

    private:
        int m_threadsNumber;
        std::string m_outputPath;
        bool m_explainMode;
        int m_nonEmptyDimensionLimit;
        int m_comparableDimensionLimit;
        double m_similarityLimit;

        // Server configuration
        std::string m_serverAddress;
        std::string m_serverJsonAttribute;
        std::string m_serverJsonValue;
        std::string m_serverGraphAttribute;
        std::string m_serverGraphValue;
        std::string m_serverQueryAttribute;
        std::string m_serverUsername;
        std::string m_serverPassword;
        int m_serverTimeout;
        int m_serverMaxRows;

        // Relation types
        std::set<std::string> m_relationTypes;

        // Dimensions
        std::map<std::string, const DimensionConfiguration> m_dimensions;

        // Output predicates configuration
        std::string m_outputPredEqual;
        std::string m_outputPredEquiv;
        std::string m_outputPredLeq;
        std::string m_outputPredGeq;
        std::string m_outputPredComparable;
        std::string m_outputPredDependencyRelated;
};

std::ostream& operator<<(std::ostream &stream, Configuration const &configuration);


#endif //TCN3R_CONFIGURATION_H
