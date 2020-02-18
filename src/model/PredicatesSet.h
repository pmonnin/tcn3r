#ifndef TCN3R_PREDICATESSET_H
#define TCN3R_PREDICATESSET_H


#include <map>

#include "../configuration/Configuration.h"
#include "../io/Logger.h"
#include "../io/ServerManager.h"
#include "Predicate.h"

class PredicatesSet
{
    public:
        PredicatesSet(const ServerManager &serverManager, const Logger &logger);
        ~PredicatesSet();
        Predicate* getPredicateFromUri(const std::string &uri);

    private:
        std::map<std::string, Predicate*> m_predicates;
};


#endif //TCN3R_PREDICATESSET_H
