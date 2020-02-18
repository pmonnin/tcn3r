#ifndef TCN3R_RELATION_H
#define TCN3R_RELATION_H


#include <map>
#include <set>
#include <string>

#include "../configuration/Configuration.h"
#include "Individual.h"
#include "IndividualsSet.h"
#include "PredicatesSet.h"
#include "RelationElement.h"

class Relation
{
    public:
        Relation(Individual *relInd, std::map<Individual*, RelationElement*> &indToEl,
                 const std::map<std::string, std::set<Individual*>> &dimensionInstances, IndividualsSet &individualsSet,
                 PredicatesSet &predicatesSet, const Configuration &parameters);
        std::set<std::string> getURIs() const;
        std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> getDimensions() const;
        std::map<std::string, std::set<RelationElement*>> getAggregatedDimensions() const;
        std::string toString() const;

    private:
        const std::set<std::string> m_uris;
        std::map<std::pair<std::string, Predicate*>, std::set<RelationElement*>> m_dimensions;
};


#endif //TCN3R_RELATION_H
