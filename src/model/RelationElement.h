#ifndef TCN3R_RELATIONELEMENT_H
#define TCN3R_RELATIONELEMENT_H


#include <set>
#include <string>
#include "Individual.h"

class RelationElement
{
    public:
        static RelationElement* getRelationElementFromIndividual(std::map<Individual*, RelationElement*> &indToEl, Individual *i);

        explicit RelationElement(std::set<std::string> uris);
        std::string toString() const;
        void addDependency(const std::string &dimensionName, RelationElement *dependency);
        std::set<RelationElement*> getDependencies(const std::string &dimensionName) const;
        void addInDimension(const std::string &dimensionName);
        std::set<std::string> getInDimensions() const;

    private:
        const std::set<std::string> m_uris;
        std::map<std::string, std::set<RelationElement*>> m_dependencies;
        std::set<std::string> m_inDimensions;
};


#endif //TCN3R_RELATIONELEMENT_H
