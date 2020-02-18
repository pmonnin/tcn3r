#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

#include <boost/progress.hpp>

#include "AnnotationsPreorder.h"
#include "IndividualsPreorder.h"
#include "RelationsReconcilier.h"
#include "RelationNotFound.h"
#include "SetInclusionPreorder.h"


RelationsReconcilier::RelationsReconcilier(const ServerManager &serverManager, const Configuration &parameters,
                                           const Logger &logger) : m_predicatesSet(serverManager, logger), m_relations(),
                                                                       m_uriToRelation(), m_relationElements()
{
    // Build individuals set (handling canonical individuals from owl:sameAs edges)
    IndividualsSet individualsSet(serverManager, logger);

    // Add edges for useful predicates
    addEdges(individualsSet, serverManager, parameters, logger);

    // Build relations
    buildRelationsAndPreorders(individualsSet, parameters, logger);
}

RelationsReconcilier::~RelationsReconcilier()
{
    for (const auto &r : m_relations)
    {
        delete r;
    }

    for (const auto &el : m_relationElements)
    {
        delete el;
    }

    for (const auto &p : m_preorders)
    {
        delete p.second;
    }
}

void RelationsReconcilier::addEdges(IndividualsSet &individualsSet, const ServerManager &serverManager,
                                    const Configuration &parameters, const Logger &logger)
{
    // Detect useful predicates to add
    std::set<Predicate*> predicatesToQuery;
    predicatesToQuery.insert(m_predicatesSet.getPredicateFromUri("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));
    predicatesToQuery.insert(m_predicatesSet.getPredicateFromUri("http://www.w3.org/2000/01/rdf-schema#subClassOf"));

    for (const auto &pUri : parameters.getPredicatesInDimensions())
    {
        predicatesToQuery.insert(m_predicatesSet.getPredicateFromUri(pUri));

        for (const auto &descendant : m_predicatesSet.getPredicateFromUri(pUri)->getDescendants())
        {
            predicatesToQuery.insert(descendant);
        }

        for (const auto &inverse : m_predicatesSet.getPredicateFromUri(pUri)->getInverses())
        {
            predicatesToQuery.insert(inverse);
        }
    }

    // Add edges for useful predicates
    for (const auto &p : predicatesToQuery)
    {
        // Query edges for given predicate
        logger.info("Query " + p->getURI() + " edges");
        std::set<std::pair<std::string, std::string>> edges = serverManager.queryTwoElements("?e1 <" + p->getURI() + "> ?e2");

        // Add edges to individuals
        logger.info("Build " + p->getURI() + " adjacency from edges");

        boost::progress_display progressBar(edges.size());
        for (const auto &e : edges)
        {
            Individual *i1 = individualsSet.getIndividualFromURI(e.first);
            Individual *i2 = individualsSet.getIndividualFromURI(e.second);

            i1->addEdge(p, i2);

            // Add edges for inverses
            for (const auto &pInv : p->getInverses())
            {
                i2->addEdge(pInv, i1);
            }

            // Add edges for ancestors (and their inverses)
            for (const auto &pAncestor : p->getAncestors())
            {
                i1->addEdge(pAncestor, i2);

                for (const auto &pAncestorInv : pAncestor->getInverses())
                {
                    i2->addEdge(pAncestorInv, i1);
                }
            }

            ++progressBar;
        }
    }
}

void RelationsReconcilier::buildRelationsAndPreorders(IndividualsSet &individualsSet, const Configuration &parameters, const Logger &logger)
{
    Predicate *type = m_predicatesSet.getPredicateFromUri("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    Predicate *subClassOf = m_predicatesSet.getPredicateFromUri("http://www.w3.org/2000/01/rdf-schema#subClassOf");

    // Detect dimension instances
    logger.info("Detect dimension instances");
    std::map<std::string, std::set<Individual*>> dimensionInstances;
    for (const auto &d : parameters.getDimensions())
    {
        for (const auto &uri : d.second.getIndividualTypes())
        {
            std::set<Individual*> instances(individualsSet.getIndividualFromURI(uri)->getInstances(type, subClassOf));
            dimensionInstances[d.first].insert(instances.begin(), instances.end());
        }
    }

    // Build Relations from Individuals
    logger.info("Build relations");

    std::map<Individual*, Relation*> indToRel;
    std::map<Individual*, RelationElement*> indToEl;

    for (const auto &relTypeUri : parameters.getRelationTypes())
    {
        logger.info("From individuals instantiating " + relTypeUri);
        Individual *relType = individualsSet.getIndividualFromURI(relTypeUri);

        logger.info("Get instances");
        std::set<Individual*> instances = relType->getInstances(type, subClassOf);

        logger.info("Transform instances in relations");
        boost::progress_display progressBar(instances.size());
        for (const auto &relInd : instances)
        {
            // If this individual is not associated to a relation => this is a new relation to add
            if (indToRel.find(relInd) == indToRel.end())
            {
                auto *rel = new Relation(relInd, indToEl, dimensionInstances, individualsSet, m_predicatesSet, parameters);
                m_relations.push_back(rel);
                indToRel[relInd] = rel;

                for (const auto &uri : rel->getURIs())
                {
                    m_uriToRelation[uri] = rel;
                }
            }

            ++progressBar;
        }
    }

    // Build dependencies for dimensions
    logger.info("Build dependencies of relation elements");
    std::set<std::string> treated;
    for (const auto &d : parameters.getDimensions())
    {
        for (const auto &uri : d.second.getIndToDepPredicates())
        {
            if (treated.find(uri) == treated.end())
            {
                treated.insert(uri);
                Predicate *p(m_predicatesSet.getPredicateFromUri(uri));

                for (const auto &i2dep : individualsSet.getAdjacency(p))
                {
                    if (indToEl.find(i2dep.first) != indToEl.end())
                    {
                        RelationElement *el = indToEl[i2dep.first];

                        for (const auto &dep : i2dep.second)
                        {
                            el->addDependency(d.first, RelationElement::getRelationElementFromIndividual(indToEl, dep));
                        }
                    }
                }
            }
        }
    }
    treated.clear();

    // Build preorders for dimensions
    logger.info("Build preorders");
    for (const auto &d : parameters.getDimensions())
    {
        switch (d.second.getPreorderName())
        {
            case SET_INCLUSION:
                logger.info("Dimension " + d.first + ": preorder Set inclusion");
                m_preorders[d.first] = new SetInclusionPreorder();
                break;

            case INDIVIDUALS:
                logger.info("Dimension " + d.first + ": preorder Individuals");
                m_preorders[d.first] = new IndividualsPreorder(indToEl, individualsSet, m_predicatesSet, d.second);
                break;

            case ANNOTATIONS:
                logger.info("Dimension " + d.first + ": preorder Annotations");
                m_preorders[d.first] = new AnnotationsPreorder(indToEl, individualsSet, m_predicatesSet, d.second);
                break;
        }
    }

    // Add all RelationElements to m_relationElements
    for (const auto &i2e : indToEl)
    {
        m_relationElements.insert(i2e.second);
    }

    // Check types of RelationElements w.r.t. dimension types
    logger.info("Add dimension types to relation elements");
    for (const auto &d : parameters.getDimensions())
    {
        for (const auto &uri : d.second.getIndividualTypes())
        {
            for (const auto &i : individualsSet.getIndividualFromURI(uri)->getInstances(type, subClassOf))
            {
                if (indToEl.find(i)  != indToEl.end())
                {
                    indToEl[i]->addInDimension(d.first);
                }
            }
        }
    }


    logger.info("Found " + std::to_string(m_relations.size()) + " relations");
}

void RelationsReconcilier::reconcileExplained(const std::string &uri1, const std::string &uri2, std::ofstream &outputStream, const Configuration &parameters)
{
    if (m_uriToRelation.find(uri1) == m_uriToRelation.end())
    {
        throw RelationNotFound(uri1 + " not found as relation");
    }

    if (m_uriToRelation.find(uri2) == m_uriToRelation.end())
    {
        throw RelationNotFound(uri2 + " not found as relation");
    }

    Relation *r1 = m_uriToRelation[uri1];
    Relation *r2 = m_uriToRelation[uri2];

    outputStream << "===================RECONCILIATION RESULTS====================" << std::endl;
    outputStream << "RELATION 1:" << std::endl << r1->toString() << std::endl;
    outputStream << "RELATION 2:" << std::endl << r2->toString() << std::endl;
    outputStream << "RESULTS:" << std::endl;

    const std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> dimR1 = r1->getDimensions();
    const std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> dimR2 = r2->getDimensions();

    OrderResult result(EQUAL);
    int count1DimEmpty(0);
    for (const auto &d1 : dimR1)
    {
        Preorder *preorder = m_preorders[d1.first.first];
        OrderResult dimResult = preorder->compare(dimR1.at(d1.first), dimR2.at(d1.first));

        if ((dimR1.at(d1.first).empty() && !dimR2.at(d1.first).empty()) || (!dimR1.at(d1.first).empty() && dimR2.at(d1.first).empty()))
        {
            count1DimEmpty++;
        }

        outputStream << "Dimension " << d1.first.first << " " << d1.first.second->getURI() << ": " << Preorder::toString(dimResult) << std::endl;

        result &= dimResult;
    }

    outputStream << "=> TEMPORARY RECONCILIATION RESULT: " << Preorder::toString(result) << std::endl << std::endl;

    if (result == COMPARABLE && count1DimEmpty != 0)
    {
        outputStream << count1DimEmpty << " COMPARISONS BETWEEN NON-EMPTY/EMPTY DIMENSIONS" << std::endl;
        result = INCOMPARABLE;
    }

    if (result == INCOMPARABLE && parameters.getNonEmptyDimensionLimit() >= 0 && (parameters.getSimilarityLimit() >= 0.0 || parameters.getComparableDimensionLimit() >= 0))
    {
        outputStream << "AGGREGATED COMPARISON + INCLUSION OF DEPENDENCIES" << std::endl;

        outputStream << "RELATION 1 aggregated dimensions :" << std::endl;
        std::map<std::string, std::set<RelationElement*>> aggDimR1 = r1->getAggregatedDimensions();
        printAggregatedDimension(aggDimR1, outputStream);

        outputStream << "RELATION 2 aggregated dimensions :" << std::endl;
        std::map<std::string, std::set<RelationElement*>> aggDimR2 = r2->getAggregatedDimensions();
        printAggregatedDimension(aggDimR2, outputStream);

        std::vector<double> jacquardResults;
        for (const auto &d1 : aggDimR1)
        {
            if (!aggDimR1.at(d1.first).empty() && !aggDimR2.at(d1.first).empty())
            {
                Preorder *preorder = m_preorders[d1.first];
                double dimJacquard = preorder->incomparableJacquard(aggDimR1.at(d1.first), aggDimR2.at(d1.first));

                outputStream << "Non-empty aggregated dimension " << d1.first << " similarity: " << dimJacquard << std::endl;

                jacquardResults.push_back(dimJacquard);
            }

            else
            {
                outputStream << "Aggregated dimension empty for at least one relation" << std::endl;
            }
        }

        if (static_cast<unsigned long>(parameters.getNonEmptyDimensionLimit()) <= jacquardResults.size())
        {
            if (parameters.getSimilarityLimit() >= 0.0 &&
                    std::all_of(jacquardResults.begin(), jacquardResults.end(), [parameters](double r) { return r >= parameters.getSimilarityLimit(); }))
            {
                result = RELATED;
            }

            if (parameters.getComparableDimensionLimit() >= 0 &&
                    std::count(jacquardResults.begin(), jacquardResults.end(), 1.0) >= parameters.getComparableDimensionLimit())
            {
                result = RELATED;
            }
        }
    }

    outputStream << std::endl << "FINAL RECONCILIATION RESULT: " << Preorder::toString(result) << std::endl;
    outputStream << "=============================================================" << std::endl << std::endl << std::endl;
}

void RelationsReconcilier::reconcileBatch(TTLWriter &ttlWriter, const Configuration &parameters)
{
    unsigned long comparisonNumber(m_relations.size() * (m_relations.size() - 1) / 2);
    boost::progress_display progressBar(comparisonNumber);

    #pragma omp parallel for default(shared) num_threads(parameters.getThreadsNumber()) schedule(dynamic)
    for (unsigned long i = 0; i < m_relations.size(); i++)
    {
        for (unsigned long j = i + 1; j < m_relations.size(); j++)
        {
            OrderResult result(reconcile(m_relations[i], m_relations[j], parameters));
            std::string uri1 = *(m_relations[i]->getURIs().begin());
            std::string uri2 = *(m_relations[j]->getURIs().begin());

            if (result == EQUAL && !parameters.getOutputPredEqual().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredEqual(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredEqual(), uri1);
                };
            }
            else if (result == EQUIV && !parameters.getOutputPredEquiv().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredEquiv(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredEquiv(), uri1);
                };
            }
            else if (result == LEQ && !parameters.getOutputPredLeq().empty() && !parameters.getOutputPredGeq().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredLeq(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredGeq(), uri1);
                };
            }
            else if (result == GEQ && !parameters.getOutputPredLeq().empty() && !parameters.getOutputPredGeq().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredGeq(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredLeq(), uri1);
                };
            }
            else if (result == COMPARABLE && !parameters.getOutputPredComparable().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredComparable(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredComparable(), uri1);
                };
            }
            else if (result == RELATED && !parameters.getOutputPredComparable().empty())
            {
                #pragma omp critical
                {
                    ttlWriter.writeTriple(uri1, parameters.getOutputPredDependencyRelated(), uri2);
                    ttlWriter.writeTriple(uri2, parameters.getOutputPredDependencyRelated(), uri1);
                };
            }

            #pragma omp critical
            ++progressBar;
        }
    }
}

OrderResult RelationsReconcilier::reconcile(Relation *r1, Relation *r2, const Configuration &parameters)
{
    const std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> dimR1 = r1->getDimensions();
    const std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> dimR2 = r2->getDimensions();

    OrderResult result(EQUAL);
    int count1DimEmpty(0);
    auto it(dimR1.begin());

    while (it != dimR1.end() && result != INCOMPARABLE)
    {
        Preorder *preorder = m_preorders[it->first.first];
        OrderResult dimResult = preorder->compare(dimR1.at(it->first), dimR2.at(it->first));
        result &= dimResult;

        if ((dimR1.at(it->first).empty() && !dimR2.at(it->first).empty()) || (!dimR1.at(it->first).empty() && dimR2.at(it->first).empty()))
        {
            count1DimEmpty++;
        }

        it++;
    }

    if (result == COMPARABLE && count1DimEmpty != 0)
    {
        result = INCOMPARABLE;
    }

    if (result == INCOMPARABLE && parameters.getNonEmptyDimensionLimit() >= 0 && (parameters.getSimilarityLimit() >= 0.0 || parameters.getComparableDimensionLimit() >= 0))
    {
        std::map<std::string, std::set<RelationElement*>> aggDimR1 = r1->getAggregatedDimensions();
        std::map<std::string, std::set<RelationElement*>> aggDimR2 = r2->getAggregatedDimensions();

        std::vector<double> jacquardResults;
        for (const auto &d1 : aggDimR1)
        {
            if (!aggDimR1.at(d1.first).empty() && !aggDimR2.at(d1.first).empty())
            {
                Preorder *preorder = m_preorders[d1.first];
                double dimJacquard = preorder->incomparableJacquard(aggDimR1.at(d1.first), aggDimR2.at(d1.first));
                jacquardResults.push_back(dimJacquard);
            }
        }

        if (static_cast<unsigned long>(parameters.getNonEmptyDimensionLimit()) <= jacquardResults.size())
        {
            if (parameters.getSimilarityLimit() >= 0.0 &&
                std::accumulate(jacquardResults.begin(), jacquardResults.end(), 0.0) / static_cast<double>(jacquardResults.size()) >=
                        parameters.getSimilarityLimit())
            {
                result = RELATED;
            }

            if (parameters.getComparableDimensionLimit() >= 0 &&
                std::count(jacquardResults.begin(), jacquardResults.end(), 1.0) >= parameters.getComparableDimensionLimit())
            {
                result = RELATED;
            }
        }
    }

    return result;
}

void RelationsReconcilier::printAggregatedDimension(const std::map<std::string, std::set<RelationElement *>> &aggDimensions, std::ofstream &outputStream)
{
    for (const auto &d : aggDimensions)
    {
        outputStream << "Aggregated dimension " << d.first << " = [ ";

        for (const auto &el : d.second)
        {
            outputStream << el->toString() << " ";
        }

        outputStream << "]" << std::endl;
    }
}
