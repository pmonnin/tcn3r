#ifndef TCN3R_SETINCLUSIONPREORDER_H
#define TCN3R_SETINCLUSIONPREORDER_H


#include <set>

#include "Preorder.h"

class SetInclusionPreorder : public Preorder
{
    public:
        SetInclusionPreorder();
        virtual ~SetInclusionPreorder();

    protected:
        virtual bool isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
        virtual unsigned long countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
};


#endif //TCN3R_SETINCLUSIONPREORDER_H
