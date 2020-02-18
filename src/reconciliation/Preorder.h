#ifndef TCN3R_PREORDER_H
#define TCN3R_PREORDER_H


#include <set>
#include <string>

#include "../model/RelationElement.h"

enum OrderResult
{
    INCOMPARABLE = 0b00000,
    COMPARABLE = 0b00001,
    LEQ = 0b00011,
    GEQ = 0b00101,
    EQUIV = 0b00111,
    EQUAL = 0b01111,
    RELATED = 0b10000
};

inline OrderResult operator& (OrderResult r1, OrderResult r2)
{
    return static_cast<OrderResult>(static_cast<int>(r1) & static_cast<int>(r2));
}

inline OrderResult& operator&= (OrderResult &r1, OrderResult r2)
{
    return reinterpret_cast<OrderResult &>(reinterpret_cast<int&>(r1) &= static_cast<int>(r2));
}

class Preorder
{
    public:
        virtual ~Preorder();
        OrderResult compare(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
        double incomparableJacquard(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const;
        static std::string toString(OrderResult r);

    protected:
        virtual bool isLeq(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const = 0;
        virtual unsigned long countIncomparableElements(const std::set<RelationElement*> &dim1, const std::set<RelationElement*> &dim2) const = 0;
};


#endif //TCN3R_PREORDER_H
