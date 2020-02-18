#ifndef TCN3R_INDIVIDUALSSET_H
#define TCN3R_INDIVIDUALSSET_H


#include <map>
#include <set>
#include <string>

#include "../io/Logger.h"
#include "../io/ServerManager.h"
#include "Individual.h"

class IndividualsSet
{
    public:
        IndividualsSet(const ServerManager &serverManager, const Logger &logger);
        ~IndividualsSet();
        Individual* getIndividualFromURI(const std::string &uri);
        std::map<Individual*, std::set<Individual*>> getAdjacency(Predicate *p);

    private:
        std::map<std::string, Individual*> m_uriToIndividual;
};


#endif //TCN3R_INDIVIDUALSSET_H
