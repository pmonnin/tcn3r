#include "Preorder.h"


Preorder::~Preorder()
= default;

std::string Preorder::toString(OrderResult r)
{
    switch (r)
    {
        case INCOMPARABLE:
            return "INCOMPARABLE";

        case COMPARABLE:
            return "COMPARABLE";

        case LEQ:
            return "LEQ";

        case GEQ:
            return "GEQ";

        case EQUIV:
            return "EQUIV";

        case EQUAL:
            return "EQUAL";

        case RELATED:
            return "RELATED";
    }

    return "INCOMPARABLE";
}

OrderResult Preorder::compare(const std::set<RelationElement *> &dim1, const std::set<RelationElement *> &dim2) const
{
    if (dim1 == dim2)
    {
        return EQUAL;
    }

    if (dim2.empty())
    {
        return LEQ;
    }

    if (dim1.empty())
    {
        return GEQ;
    }

    bool leq = isLeq(dim1, dim2);
    bool geq = isLeq(dim2, dim1);

    if (leq && geq)
    {
        return EQUIV;
    }

    if (leq)
    {
        return LEQ;
    }

    if (geq)
    {
        return GEQ;
    }

    return INCOMPARABLE;
}

double Preorder::incomparableJacquard(const std::set<RelationElement *> &dim1, const std::set<RelationElement *> &dim2) const
{
    if (dim1.empty() || dim2.empty())
    {
        return 1.0;
    }

    unsigned long nbIncomparableElements = countIncomparableElements(dim1, dim2);

    std::set<RelationElement*> dimUnion;
    dimUnion.insert(dim1.begin(), dim1.end());
    dimUnion.insert(dim2.begin(), dim2.end());

    return 1.0 - static_cast<double>(nbIncomparableElements) / static_cast<double>(dimUnion.size());
}
