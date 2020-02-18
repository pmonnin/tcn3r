#include <boost/progress.hpp>

#include "IndividualsPreorder.h"


IndividualsPreorder::IndividualsPreorder(std::map<Individual*, RelationElement*> &indToEl,
                                         IndividualsSet &individualsSet, PredicatesSet &predicatesSet,
                                         const DimensionConfiguration &configuration) : Preorder(), m_ancestors()
{
    // LEQ predicates
    boost::progress_display progressBar(configuration.getPreorderConfiguration("ind-leq-predicates").size());
    for (const auto &uri : configuration.getPreorderConfiguration("ind-leq-predicates"))
    {
        Predicate *p(predicatesSet.getPredicateFromUri(uri));

        for (const auto &i2a : individualsSet.getAdjacency(p))
        {
            RelationElement *el(RelationElement::getRelationElementFromIndividual(indToEl, i2a.first));
            for (const auto &i : i2a.second)
            {
                RelationElement *el2(RelationElement::getRelationElementFromIndividual(indToEl, i));
                m_ancestors[el].insert(el2);

                if (m_ancestors.find(el2) == m_ancestors.end())
                {
                    m_ancestors[el2].clear();
                }
            }
        }

        ++progressBar;
    }

    // GEQ predicates
    progressBar.restart(configuration.getPreorderConfiguration("ind-geq-predicates").size());
    for (const auto &uri : configuration.getPreorderConfiguration("ind-geq-predicates"))
    {
        Predicate *p(predicatesSet.getPredicateFromUri(uri));

        for (const auto &i2a : individualsSet.getAdjacency(p))
        {
            RelationElement *el(RelationElement::getRelationElementFromIndividual(indToEl, i2a.first));
            for (const auto &i : i2a.second)
            {
                RelationElement *el2(RelationElement::getRelationElementFromIndividual(indToEl, i));
                m_ancestors[el2].insert(el);

                if (m_ancestors.find(el) == m_ancestors.end())
                {
                    m_ancestors[el].clear();
                }
            }
        }

        ++progressBar;
    }

    // Compute ancestors by expanding hierarchy
    progressBar.restart(m_ancestors.size());
    for (const auto &el2a : m_ancestors)
    {
        std::set<RelationElement*> toExpand(m_ancestors[el2a.first]);

        while (!toExpand.empty())
        {
            std::set<RelationElement*> newToExpand;

            for (const auto &a : toExpand)
            {
                newToExpand.insert(m_ancestors[a].begin(), m_ancestors[a].end());
            }

            toExpand.clear();
            std::set_difference(newToExpand.begin(), newToExpand.end(), m_ancestors[el2a.first].begin(),
                                m_ancestors[el2a.first].end(), std::inserter(toExpand, toExpand.begin()));
            m_ancestors[el2a.first].insert(toExpand.begin(), toExpand.end());
        }

        if (m_ancestors[el2a.first].find(el2a.first) != m_ancestors[el2a.first].end())
        {
            m_ancestors[el2a.first].erase(el2a.first);
        }

        ++progressBar;
    }
}

IndividualsPreorder::~IndividualsPreorder()
= default;

bool IndividualsPreorder::isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    for (const auto &el : dim1)
    {
        if (!isLeq(el, dim2))
        {
            return false;
        }
    }

    return true;
}

unsigned long IndividualsPreorder::countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    std::set<RelationElement*> incomparable1;
    for (const auto &el1 : dim1)
    {
        if (!isLeq(el1, dim2))
        {
            incomparable1.insert(el1);
        }
    }

    std::set<RelationElement*> incomparable2;
    for (const auto &el2 : dim2)
    {
        if (!isLeq(el2, dim1))
        {
            incomparable2.insert(el2);
        }
    }

    if (incomparable1.empty() || incomparable2.empty())
    {
        return 0;
    }

    return incomparable1.size() + incomparable2.size();
}

bool IndividualsPreorder::isLeq(RelationElement *el, const std::set<RelationElement*> &dim2) const
{
    if (dim2.find(el) != dim2.end())
    {
        return true;
    }

    if (m_ancestors.find(el) == m_ancestors.end())
    {
        return false;
    }

    auto it(m_ancestors.at(el).begin());
    while (it != m_ancestors.at(el).end())
    {
        if (dim2.find(*it) != dim2.end())
        {
            return true;
        }

        it++;
    }

    return false;
}