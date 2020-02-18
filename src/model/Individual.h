#ifndef TCN3R_INDIVIDUAL_H
#define TCN3R_INDIVIDUAL_H


#include <map>
#include <set>

#include "../io/CacheManager.h"
#include "Predicate.h"

class Individual
{
    public:
        explicit Individual(std::set<std::string> uris);
        std::set<std::string> getURIs() const;
        void addEdge(Predicate *predicate, Individual *target);
        std::set<Individual*> getInstances(Predicate *type, Predicate *subClassOf);
        std::set<Individual*> getAdjacentIndividuals(Predicate *p);
        bool matchesPrefixes(const std::set<std::string> &prefixes);
        std::set<Individual*> getAncestors(const std::set<Predicate*> &leqPredicate, const std::set<Predicate*> &geqPredicate);

    private:
        std::set<Individual*> getDescendants(Predicate *subClassOf);
        void addInvEdge(Predicate *predicate, Individual *source);

        const std::set<std::string> m_uris;
        std::map<Predicate*, std::set<Individual*>> m_adjacency;
        std::map<Predicate*, std::set<Individual*>> m_invAdjacency;
};


#endif //TCN3R_INDIVIDUAL_H
