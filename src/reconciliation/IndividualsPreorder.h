#ifndef TCN3R_INDIVIDUALSPREORDER_H
#define TCN3R_INDIVIDUALSPREORDER_H


#include <map>

#include "../configuration/DimensionConfiguration.h"
#include "../model/IndividualsSet.h"
#include "../model/PredicatesSet.h"
#include "Preorder.h"

class IndividualsPreorder : public Preorder
{
    public:
        IndividualsPreorder(std::map<Individual*, RelationElement*> &indToEl, IndividualsSet &individualsSet,
                            PredicatesSet &predicatesSet, const DimensionConfiguration &configuration);
        virtual ~IndividualsPreorder();

    protected:
        virtual bool isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
        virtual unsigned long countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;

    private:
        std::map<RelationElement*, std::set<RelationElement*>> m_ancestors;
        bool isLeq(RelationElement *el, const std::set<RelationElement*> &dim2) const;
};


#endif //TCN3R_INDIVIDUALSPREORDER_H
