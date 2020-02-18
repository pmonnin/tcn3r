#ifndef TCN3R_RELATIONSRECONCILIER_H
#define TCN3R_RELATIONSRECONCILIER_H


#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../configuration/Configuration.h"
#include "../io/Logger.h"
#include "../io/ServerManager.h"
#include "../io/TTLWriter.h"
#include "../model/IndividualsSet.h"
#include "../model/Predicate.h"
#include "../model/PredicatesSet.h"
#include "../model/Relation.h"
#include "../model/RelationElement.h"
#include "Preorder.h"

class RelationsReconcilier
{
    public:
        RelationsReconcilier(const ServerManager &serverManager, const Configuration &parameters, const Logger &logger);
        ~RelationsReconcilier();
        void reconcileExplained(const std::string &uri1, const std::string &uri2, std::ofstream &outputStream, const Configuration &parameters);
        void reconcileBatch(TTLWriter &ttlWriter, const Configuration &parameters);

    private:
        void addEdges(IndividualsSet &individualsSet, const ServerManager &serverManager,
                      const Configuration &parameters, const Logger &logger);
        void buildRelationsAndPreorders(IndividualsSet &individualsSet, const Configuration &parameters,
                                        const Logger &logger);
        OrderResult reconcile(Relation *r1, Relation *r2, const Configuration &parameters);
        static void printAggregatedDimension(const std::map<std::string, std::set<RelationElement*>> &aggDimensions, std::ofstream &outputStream);

        PredicatesSet m_predicatesSet;

        std::vector<Relation*> m_relations;
        std::map<std::string, Relation*> m_uriToRelation;

        std::set<RelationElement*> m_relationElements;

        std::map<std::string, Preorder*> m_preorders;
};


#endif //TCN3R_RELATIONSRECONCILIER_H
