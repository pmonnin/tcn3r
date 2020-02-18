#ifndef TCN3R_ANNOTATIONSPREORDER_H
#define TCN3R_ANNOTATIONSPREORDER_H


#include <map>

#include "../configuration/DimensionConfiguration.h"
#include "../model/Individual.h"
#include "../model/IndividualsSet.h"
#include "../model/PredicatesSet.h"
#include "../model/RelationElement.h"
#include "Preorder.h"

class AnnotationsPreorder : public Preorder
{
    public:
        AnnotationsPreorder(std::map<Individual*, RelationElement*> &indToEl, IndividualsSet &individualsSet,
                            PredicatesSet &predicatesSet, const DimensionConfiguration &configuration);
        virtual ~AnnotationsPreorder();

    protected:
        virtual bool isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
        virtual unsigned long countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;

    private:
        bool isLeq(RelationElement *el, const std::set<RelationElement*> &dim2, const std::set<RelationElement*> &msa2) const;

        std::map<RelationElement*, std::set<RelationElement*>> m_msa;
        std::map<RelationElement*, std::set<RelationElement*>> m_ancestors;
};


#endif //TCN3R_ANNOTATIONSPREORDER_H
