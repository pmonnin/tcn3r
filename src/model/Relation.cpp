#include "Relation.h"


Relation::Relation(Individual *relInd, std::map<Individual*, RelationElement*> &indToEl,
                   const std::map<std::string, std::set<Individual*>> &dimensionInstances, IndividualsSet &individualsSet,
                   PredicatesSet &predicatesSet, const Configuration &parameters) : m_uris(relInd->getURIs()), m_dimensions()
{
    for (const auto &d : parameters.getDimensions())
    {
        // Detect all predicates associated with this dimension
        std::set<Predicate*> dimensionPredicates;
        for (const auto &predUri : d.second.getRelToIndPredicates())
        {
            Predicate *p = predicatesSet.getPredicateFromUri(predUri);
            dimensionPredicates.insert(p);

            for (const auto &pDescendant : p->getDescendants())
            {
                dimensionPredicates.insert(pDescendant);
            }
        }

        for (const auto &p : dimensionPredicates)
        {
            std::pair<std::string, Predicate*> dimKey(d.first, p);
            m_dimensions[dimKey].clear();

            for (const auto &i : relInd->getAdjacentIndividuals(p))
            {
                if (dimensionInstances.at(d.first).find(i) != dimensionInstances.at(d.first).end())
                {
                    m_dimensions[dimKey].insert(RelationElement::getRelationElementFromIndividual(indToEl, i));
                }
            }
        }
    }
}

std::set<std::string> Relation::getURIs() const
{
    return m_uris;
}

std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> Relation::getDimensions() const
{
    return m_dimensions;
}

std::map<std::string, std::set<RelationElement*>> Relation::getAggregatedDimensions() const
{
    std::map<std::string, std::set<RelationElement*>> aggregatedDimensions;

    for (const auto &d : m_dimensions)
    {
        if (aggregatedDimensions.find(d.first.first) == aggregatedDimensions.end())
        {
            aggregatedDimensions[d.first.first].clear();
        }

        for (const auto &el : d.second)
        {
            aggregatedDimensions[d.first.first].insert(el);

            for (const auto &dep : el->getDependencies(d.first.first))
            {
                if (dep->getInDimensions().empty())
                {
                    aggregatedDimensions[d.first.first].insert(dep);
                }

                else
                {
                    for (const auto &dName : dep->getInDimensions())
                    {
                        aggregatedDimensions[dName].insert(dep);
                    }
                }
            }
        }
    }

    return aggregatedDimensions;
}

std::string Relation::toString() const
{
    std::string retVal("Relation URIs = [ ");
    for (const auto &uri : m_uris)
    {
        retVal += uri + " ";
    }
    retVal += "]\n";

    for (const auto &d : m_dimensions)
    {
        retVal += "Dimension " + d.first.first + " " + d.first.second->getURI() + " = [ ";

        for (const auto &el : d.second)
        {
            retVal += el->toString() + " ";
        }

        retVal += "]\n";
    }

    return retVal;
}
