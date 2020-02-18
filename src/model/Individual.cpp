#include <algorithm>

#include "Individual.h"


Individual::Individual(const std::set<std::string> uris) : m_uris(uris), m_adjacency(), m_invAdjacency()
{

}

std::set<std::string> Individual::getURIs() const
{
    return m_uris;
}

void Individual::addEdge(Predicate *predicate, Individual *target)
{
    m_adjacency[predicate].insert(target);
    target->addInvEdge(predicate, this);
}

void Individual::addInvEdge(Predicate *predicate, Individual *source)
{
    m_invAdjacency[predicate].insert(source);
}

std::set<Individual*> Individual::getInstances(Predicate *type, Predicate *subClassOf)
{
    std::set<Individual*> instances;

    instances.insert(m_invAdjacency[type].begin(), m_invAdjacency[type].end());

    for (const auto &d : getDescendants(subClassOf))
    {
        instances.insert(d->m_invAdjacency[type].begin(), d->m_invAdjacency[type].end());
    }

    return instances;
}

std::set<Individual*> Individual::getDescendants(Predicate *subClassOf)
{
    std::set<Individual*> descendants;
    descendants.insert(m_invAdjacency[subClassOf].begin(), m_invAdjacency[subClassOf].end());

    std::set<Individual*> toExpand;
    toExpand.insert(m_invAdjacency[subClassOf].begin(), m_invAdjacency[subClassOf].end());

    while (not toExpand.empty())
    {
        std::set<Individual*> newToExpand;

        for (const auto &i : toExpand)
        {
            newToExpand.insert(i->m_invAdjacency[subClassOf].begin(), i->m_invAdjacency[subClassOf].end());
        }

        toExpand.clear();
        std::set_difference(newToExpand.begin(), newToExpand.end(), descendants.begin(), descendants.end(),
                            std::inserter(toExpand, toExpand.begin()));
        descendants.insert(toExpand.begin(), toExpand.end());
    }

    if (descendants.find(this) != descendants.end())
    {
        descendants.erase(this);
    }

    return descendants;
}

std::set<Individual*> Individual::getAdjacentIndividuals(Predicate *p)
{
    return m_adjacency[p];
}

bool Individual::matchesPrefixes(const std::set<std::string> &prefixes)
{
    for (const auto &prefix : prefixes)
    {
        for (const auto &uri : m_uris)
        {
            if (uri.rfind(prefix, 0) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

std::set<Individual*> Individual::getAncestors(const std::set<Predicate*> &leqPredicate, const std::set<Predicate*> &geqPredicate)
{
    std::set<Individual*> ancestors;
    std::set<Individual*> toExpand;
    for (const auto &p : leqPredicate)
    {
        ancestors.insert(m_adjacency[p].begin(), m_adjacency[p].end());
        toExpand.insert(m_adjacency[p].begin(), m_adjacency[p].end());
    }

    for (const auto &p : geqPredicate)
    {
        ancestors.insert(m_invAdjacency[p].begin(), m_invAdjacency[p].end());
        toExpand.insert(m_invAdjacency[p].begin(), m_invAdjacency[p].end());
    }

    while (!toExpand.empty())
    {
        std::set<Individual*> newToExpand;

        for (const auto &i : toExpand)
        {
            for (const auto &p : leqPredicate)
            {
                newToExpand.insert(i->m_adjacency[p].begin(), i->m_adjacency[p].end());
            }

            for (const auto &p : geqPredicate)
            {
                newToExpand.insert(i->m_invAdjacency[p].begin(), i->m_invAdjacency[p].end());
            }
        }

        toExpand.clear();
        std::set_difference(newToExpand.begin(), newToExpand.end(), ancestors.begin(), ancestors.end(),
                            std::inserter(toExpand, toExpand.begin()));
        ancestors.insert(toExpand.begin(), toExpand.end());
    }

    if (ancestors.find(this) != ancestors.end())
    {
        ancestors.erase(this);
    }

    return ancestors;
}
