#include <algorithm>

#include "SetInclusionPreorder.h"

SetInclusionPreorder::SetInclusionPreorder() : Preorder()
{

}

SetInclusionPreorder::~SetInclusionPreorder()
= default;

bool SetInclusionPreorder::isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    return std::includes(dim2.begin(), dim2.end(), dim1.begin(), dim1.end());
}

unsigned long SetInclusionPreorder::countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const
{
    std::set<RelationElement*> diff12, diff21;

    std::set_difference(dim1.begin(), dim1.end(), dim2.begin(), dim2.end(), std::inserter(diff12, diff12.begin()));
    std::set_difference(dim2.begin(), dim2.end(), dim1.begin(), dim1.end(), std::inserter(diff21, diff21.begin()));

    if (diff12.empty() || diff21.empty())
    {
        return 0;
    }

    return diff12.size() + diff21.size();
}
