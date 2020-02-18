#ifndef TCN3R_PREDICATE_H
#define TCN3R_PREDICATE_H


#include <set>
#include <string>

class Predicate
{
    public:
        explicit Predicate(std::string uri);
        std::string getURI() const;
        void addInverse(Predicate *inverse);
        void addSuperPredicate(Predicate *p);
        void addSubPredicate(Predicate *p);
        std::set<Predicate*> getInverses() const;
        std::set<Predicate*> getDescendants();
        std::set<Predicate*> getAncestors();

    private:
        const std::string m_uri;
        std::set<Predicate*> m_inverses;
        std::set<Predicate*> m_superPredicates;
        std::set<Predicate*> m_subPredicates;
};


#endif //TCN3R_PREDICATE_H
