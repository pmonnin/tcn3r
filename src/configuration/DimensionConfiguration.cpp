#include <utility>

#include "DimensionConfiguration.h"


DimensionConfiguration::DimensionConfiguration(std::string name, boost::property_tree::ptree &pt, Logger const &logger) :
        m_name(std::move(name)), m_individualTypes(), m_relToIndPredicates(),  m_indToDepPredicates(), m_preorderConfiguration()
{
    try
    {
        for (boost::property_tree::ptree::value_type &t : pt.get_child("ind-types"))
        {
            m_individualTypes.insert(t.second.data());
        }

        for (boost::property_tree::ptree::value_type &p : pt.get_child("rel2ind-predicates"))
        {
            m_relToIndPredicates.insert(p.second.data());
        }

        for (boost::property_tree::ptree::value_type &p : pt.get_child("ind2dep-predicates"))
        {
            m_indToDepPredicates.insert(p.second.data());
        }

        auto preorder = pt.get<std::string>("preorder");
        if (preorder == "Individuals")
        {
            m_preorderName = INDIVIDUALS;

            for (boost::property_tree::ptree::value_type &p : pt.get_child("ind-leq-predicates"))
            {
                m_preorderConfiguration["ind-leq-predicates"].insert(p.second.data());
            }

            for (boost::property_tree::ptree::value_type &p: pt.get_child("ind-geq-predicates"))
            {
                m_preorderConfiguration["ind-geq-predicates"].insert(p.second.data());
            }
        }
        else if (preorder == "Annotations")
        {
            m_preorderName = ANNOTATIONS;

            m_preorderConfiguration["ann-base-uris"].clear();
            for (boost::property_tree::ptree::value_type &buri : pt.get_child("ann-base-uris"))
            {
                m_preorderConfiguration["ann-base-uris"].insert(buri.second.data());
            }

            m_preorderConfiguration["ind2ann-predicates"].clear();
            for (boost::property_tree::ptree::value_type &p : pt.get_child("ind2ann-predicates"))
            {
                m_preorderConfiguration["ind2ann-predicates"].insert(p.second.data());
            }

            m_preorderConfiguration["ann-leq-predicates"].clear();
            for (boost::property_tree::ptree::value_type &p : pt.get_child("ann-leq-predicates"))
            {
                m_preorderConfiguration["ann-leq-predicates"].insert(p.second.data());
            }

            m_preorderConfiguration["ann-geq-predicates"].clear();
            for (boost::property_tree::ptree::value_type &p : pt.get_child("ann-geq-predicates"))
            {
                m_preorderConfiguration["ann-geq-predicates"].insert(p.second.data());
            }
        }
        else if (preorder == "SetInclusion")
        {
            m_preorderName = SET_INCLUSION;
        }
        else
        {
            logger.critical("Unknown preorder: " + preorder);
            std::exit(-1);
        }
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

std::string DimensionConfiguration::toString() const
{
    std::string retVal = "Dimension " + m_name + "\n";

    retVal += "Individual types:\n";
    for (auto &t : m_individualTypes)
    {
        retVal += t + "\n";
    }

    retVal += "Individuals to relations predicates:\n";
    for (auto &p : m_relToIndPredicates)
    {
        retVal += p + "\n";
    }

    retVal += "Individuals to dependencies predicates:\n";
    for (auto &p : m_indToDepPredicates)
    {
        retVal += p + "\n";
    }

    if (m_preorderName == INDIVIDUALS)
    {
        retVal += "Preorder: Individuals\n";

        retVal += "Individuals <= predicates:\n";

        for (const auto &p : m_preorderConfiguration.at("ind-leq-predicates"))
        {
            retVal += p + "\n";
        }

        retVal += "Individuals >= predicates:\n";

        for (const auto &p : m_preorderConfiguration.at("ind-geq-predicates"))
        {
            retVal += p + "\n";
        }
    }
    else if (m_preorderName == ANNOTATIONS)
    {
        retVal += "Preorder: Annotations\n";

        retVal += "Annotations base URIs:\n";
        for (const auto &buri : m_preorderConfiguration.at("ann-base-uris"))
        {
            retVal += buri + "\n";
        }

        retVal += "Individual to annotations predicates:\n";
        for (const auto &p : m_preorderConfiguration.at("ind2ann-predicates"))
        {
            retVal += p + "\n";
        }

        retVal += "Annotations <= predicates:\n";
        for (const auto &p : m_preorderConfiguration.at("ann-leq-predicates"))
        {
            retVal += p + "\n";
        }

        retVal += "Annotations >= predicates:\n";
        for (const auto &p : m_preorderConfiguration.at("ann-geq-predicates"))
        {
            retVal += p + "\n";
        }
    }
    else
    {
        retVal += "Preorder: Set inclusion\n";
    }

    return retVal;
}

std::set<std::string> DimensionConfiguration::getIndividualTypes() const
{
    return m_individualTypes;
}

std::set<std::string> DimensionConfiguration::getRelToIndPredicates() const
{
    return m_relToIndPredicates;
}

std::set<std::string> DimensionConfiguration::getIndToDepPredicates() const
{
    return m_indToDepPredicates;
}

std::set<std::string> DimensionConfiguration::getUsedPredicates() const
{
    std::set<std::string> predicates(m_relToIndPredicates);
    predicates.insert(m_indToDepPredicates.begin(), m_indToDepPredicates.end());

    if (m_preorderName == INDIVIDUALS)
    {
        predicates.insert(m_preorderConfiguration.at("ind-leq-predicates").begin(), m_preorderConfiguration.at("ind-leq-predicates").end());
        predicates.insert(m_preorderConfiguration.at("ind-geq-predicates").begin(), m_preorderConfiguration.at("ind-geq-predicates").end());
    }

    else if (m_preorderName == ANNOTATIONS)
    {
        predicates.insert(m_preorderConfiguration.at("ind2ann-predicates").begin(), m_preorderConfiguration.at("ind2ann-predicates").end());
        predicates.insert(m_preorderConfiguration.at("ann-leq-predicates").begin(), m_preorderConfiguration.at("ann-leq-predicates").end());
        predicates.insert(m_preorderConfiguration.at("ann-geq-predicates").begin(), m_preorderConfiguration.at("ann-geq-predicates").end());
    }

    return predicates;
}

PreorderName DimensionConfiguration::getPreorderName() const
{
    return m_preorderName;
}

std::set<std::string> DimensionConfiguration::getPreorderConfiguration(const std::string &confKey) const
{
    if (m_preorderConfiguration.find(confKey) == m_preorderConfiguration.end())
    {
        return std::set<std::string>();
    }

    return m_preorderConfiguration.at(confKey);
}
