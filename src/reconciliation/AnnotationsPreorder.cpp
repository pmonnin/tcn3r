#include <boost/progress.hpp>

#include "AnnotationsPreorder.h"

AnnotationsPreorder::AnnotationsPreorder(std::map<Individual*, RelationElement*> &indToEl,
                                         IndividualsSet &individualsSet, PredicatesSet &predicatesSet,
                                         const DimensionConfiguration &configuration) : Preorder(), m_msa(), m_ancestors()
{
    // Get hierarchical predicates
    std::set<Predicate*> leqPredicates;
    for (const auto &uri : configuration.getPreorderConfiguration("ann-leq-predicates"))
    {
        leqPredicates.insert(predicatesSet.getPredicateFromUri(uri));
    }

    std::set<Predicate*> geqPredicates;
    for (const auto &uri : configuration.getPreorderConfiguration("ann-geq-predicates"))
    {
        geqPredicates.insert(predicatesSet.getPredicateFromUri(uri));
    }

    // Instantiation of annotations + hierarchical organization for new annotations
    boost::progress_display progressBar(configuration.getPreorderConfiguration("ind2ann-predicates").size());
    for (const auto &uri : configuration.getPreorderConfiguration("ind2ann-predicates"))
    {
        Predicate *p(predicatesSet.getPredicateFromUri(uri));

        for (const auto &i2a : individualsSet.getAdjacency(p))
        {
            for (const auto &a : i2a.second)
            {
                if (a->matchesPrefixes(configuration.getPreorderConfiguration("ann-base-uris")))
                {
                    RelationElement *el(RelationElement::getRelationElementFromIndividual(indToEl, i2a.first));
                    RelationElement *ann(RelationElement::getRelationElementFromIndividual(indToEl, a));
                    m_msa[el].insert(ann);

                    if (m_ancestors.find(ann) == m_ancestors.end())
                    {
                        m_ancestors[ann].clear();

                        for (const auto &ancestorInd : a->getAncestors(leqPredicates, geqPredicates))
                        {
                            m_ancestors[ann].insert(RelationElement::getRelationElementFromIndividual(indToEl, ancestorInd));

                            if (m_ancestors[ann].find(ann) != m_ancestors[ann].end())
                            {
                                m_ancestors[ann].erase(ann);
                            }
                        }
                    }
                }
            }
        }

        ++progressBar;
    }

    // Compute Most Specific Annotations
    progressBar.restart(m_msa.size());
    for (const auto &el2ann : m_msa)
    {
        std::set<RelationElement*> ancestorsToRemove;
        for (const auto &ann : el2ann.second)
        {
            ancestorsToRemove.insert(m_ancestors[ann].begin(), m_ancestors[ann].end());
        }

        std::set<RelationElement*> msa;
        std::set_difference(m_msa[el2ann.first].begin(), m_msa[el2ann.first].end(), ancestorsToRemove.begin(),
                            ancestorsToRemove.end(), std::inserter(msa, msa.begin()));

        m_msa[el2ann.first] = msa;

        ++progressBar;
    }
}

AnnotationsPreorder::~AnnotationsPreorder()
= default;

bool AnnotationsPreorder::isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    // Get all most specific annotations from dimension 2
    std::set<RelationElement*> msa2;
    for (const auto &el2 : dim2)
    {
        if (m_msa.find(el2) != m_msa.end())
        {
            msa2.insert(m_msa.at(el2).begin(), m_msa.at(el2).end());
        }
    }

    for (const auto &el1 : dim1)
    {
        if (!isLeq(el1, dim2, msa2))
        {
            return false;
        }
    }

    return true;
}

unsigned long AnnotationsPreorder::countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    std::set<RelationElement*> msa1;
    for (const auto &el1 : dim1)
    {
        if (m_msa.find(el1) != m_msa.end())
        {
            msa1.insert(m_msa.at(el1).begin(), m_msa.at(el1).end());
        }
    }

    std::set<RelationElement*> msa2;
    for (const auto &el2 : dim2)
    {
        if (m_msa.find(el2) != m_msa.end())
        {
            msa2.insert(m_msa.at(el2).begin(), m_msa.at(el2).end());
        }
    }

    std::set<RelationElement*> incomparable1;
    for (const auto &el1 : dim1)
    {
        if (!isLeq(el1, dim2, msa2))
        {
            incomparable1.insert(el1);
        }
    }

    std::set<RelationElement*> incomparable2;
    for (const auto &el2 : dim2)
    {
        if (!isLeq(el2, dim1, msa1))
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

bool AnnotationsPreorder::isLeq(RelationElement *el, const std::set<RelationElement*> &dim2, const std::set<RelationElement*> &msa2) const
{
    if (dim2.find(el) != dim2.end())
    {
        return true;
    }

    if (m_msa.find(el) == m_msa.end())
    {
        return false;
    }

    if (msa2.empty())
    {
        return false;
    }

    for (const auto &ann : m_msa.at(el))
    {
        if (msa2.find(ann) == msa2.end())
        {
            bool ancestorFound(false);
            auto it(m_ancestors.at(ann).begin());

            while (!ancestorFound && it != m_ancestors.at(ann).end())
            {
                if (msa2.find(*it) != msa2.end())
                {
                    ancestorFound = true;
                }

                it++;
            }

            if (!ancestorFound)
            {
                return false;
            }
        }
    }

    return true;
}
