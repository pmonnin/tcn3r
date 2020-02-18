#ifndef TCN3R_DIMENSIONCONFIGURATION_H
#define TCN3R_DIMENSIONCONFIGURATION_H


#include <map>
#include <set>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "../io/Logger.h"

enum PreorderName {
    ANNOTATIONS,
    INDIVIDUALS,
    SET_INCLUSION
};

class DimensionConfiguration
{
    public:
        DimensionConfiguration(std::string name, boost::property_tree::ptree &pt, Logger const &logger);

        std::string toString() const;
        std::set<std::string> getIndividualTypes() const;
        std::set<std::string> getRelToIndPredicates() const;
        std::set<std::string> getIndToDepPredicates() const;
        std::set<std::string> getUsedPredicates() const;
        PreorderName getPreorderName() const;
        std::set<std::string> getPreorderConfiguration(const std::string &confKey) const;

    private:
        std::string m_name;
        std::set<std::string> m_individualTypes;
        std::set<std::string> m_relToIndPredicates;
        std::set<std::string> m_indToDepPredicates;
        PreorderName m_preorderName;
        std::map<std::string, std::set<std::string>> m_preorderConfiguration;
};


#endif //TCN3R_DIMENSIONCONFIGURATION_H
