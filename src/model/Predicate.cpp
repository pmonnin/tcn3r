#include <algorithm>

#include "Predicate.h"


Predicate::Predicate(const std::string uri) : m_uri(uri), m_inverses(), m_superPredicates(), m_subPredicates()
{

}

std::string Predicate::getURI() const
{
    return m_uri;
}

void Predicate::addInverse(Predicate *inverse)
{
    m_inverses.insert(inverse);
}

void Predicate::addSuperPredicate(Predicate *p)
{
    m_superPredicates.insert(p);
}

void Predicate::addSubPredicate(Predicate *p)
{
    m_subPredicates.insert(p);
}

std::set<Predicate*> Predicate::getInverses() const
{
    return m_inverses;
}

std::set<Predicate*> Predicate::getDescendants()
{
    std::set<Predicate*> descendants;
    descendants.insert(m_subPredicates.begin(), m_subPredicates.end());

    std::set<Predicate*> toExpand;
    toExpand.insert(m_subPredicates.begin(), m_subPredicates.end());

    while (not toExpand.empty())
    {
        std::set<Predicate*> newToExpand;

        for (const auto &p : toExpand)
        {
            newToExpand.insert(p->m_subPredicates.begin(), p->m_subPredicates.end());
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

std::set<Predicate*> Predicate::getAncestors()
{
    std::set<Predicate*> ancestors;
    ancestors.insert(m_superPredicates.begin(), m_superPredicates.end());

    std::set<Predicate*> toExpand;
    toExpand.insert(m_superPredicates.begin(), m_superPredicates.end());

    while (not toExpand.empty())
    {
        std::set<Predicate*> newToExpand;

        for (const auto &p : toExpand)
        {
            newToExpand.insert(p->m_superPredicates.begin(), p->m_superPredicates.end());
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

