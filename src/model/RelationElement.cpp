#include <utility>

#include "RelationElement.h"


RelationElement::RelationElement(std::set<std::string> uris) : m_uris(std::move(uris)), m_dependencies(), m_inDimensions()
{

}

std::string RelationElement::toString() const
{
    std::string retVal("RelationElement = [ ");

    for (const auto &uri : m_uris)
    {
        retVal += uri + " ";
    }

    retVal += " ]";

    return retVal;
}

RelationElement* RelationElement::getRelationElementFromIndividual(std::map<Individual *, RelationElement *> &indToEl, Individual *i)
{
    if (indToEl.find(i) == indToEl.end())
    {
        indToEl[i] = new RelationElement(i->getURIs());
    }

    return indToEl[i];
}

void RelationElement::addDependency(const std::string &dimensionName, RelationElement *dependency)
{
    m_dependencies[dimensionName].insert(dependency);
}

std::set<RelationElement*> RelationElement::getDependencies(const std::string &dimensionName) const
{
    if (m_dependencies.find(dimensionName) == m_dependencies.end())
    {
        return std::set<RelationElement*>();
    }

    return m_dependencies.at(dimensionName);
}

void RelationElement::addInDimension(const std::string &dimensionName)
{
    m_inDimensions.insert(dimensionName);
}

std::set<std::string> RelationElement::getInDimensions() const
{
    return m_inDimensions;
}
