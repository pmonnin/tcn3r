#include "PredicatesSet.h"


PredicatesSet::PredicatesSet(const ServerManager &serverManager, const Logger &logger) : m_predicates()
{
    // Query all predicates
    logger.info("Query all predicates");
    std::set<std::string> predicateUris = serverManager.queryElements("[] ?e [] .");
    for (const auto &uri : predicateUris)
    {
        m_predicates[uri] = new Predicate(uri);
    }

    // Query inverse predicates
    logger.info("Query owl:inverseOf edges");
    std::set<std::pair<std::string, std::string>> inverseOfEdges = serverManager.queryTwoElements("?e1 owl:inverseOf ?e2");

    logger.info("Add inverses for predicates");
    for (const auto &e : inverseOfEdges)
    {
        Predicate *p1 = getPredicateFromUri(e.first);
        Predicate *p2 = getPredicateFromUri(e.second);

        p1->addInverse(p2);
        p2->addInverse(p1);
    }

    // Query symmetric predicates
    logger.info("Query symmetric predicates");
    std::set<std::string> symmetricPredicates = serverManager.queryElements("?e rdf:type owl:SymmetricProperty");

    logger.info("Add symmetry for predicates");
    for (const auto &uri : symmetricPredicates)
    {
        Predicate *p = getPredicateFromUri(uri);
        p->addInverse(p);
    }

    // Query owl:subPropertyOf edges
    logger.info("Query rdfs:subPropertyOf edges");
    std::set<std::pair<std::string, std::string>> subPropertyOfEdges = serverManager.queryTwoElements("?e1 rdfs:subPropertyOf ?e2");

    logger.info("Build hierarchy of predicates");
    for (const auto &e : subPropertyOfEdges)
    {
        Predicate *p1 = getPredicateFromUri(e.first);
        Predicate *p2 = getPredicateFromUri(e.second);

        p1->addSuperPredicate(p2);
        p2->addSubPredicate(p1);
    }
}

PredicatesSet::~PredicatesSet()
{
    std::set<Predicate*> toDelete;
    for (const auto &u2p : m_predicates)
    {
        toDelete.insert(u2p.second);
    }

    for (const auto &p : toDelete)
    {
        delete p;
    }
}

Predicate* PredicatesSet::getPredicateFromUri(const std::string &uri)
{
    if (m_predicates.find(uri) == m_predicates.end())
    {
        m_predicates[uri] = new Predicate(uri);
    }

    return m_predicates[uri];
}
