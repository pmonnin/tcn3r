#ifndef TCN3R_CACHEMANAGER_H
#define TCN3R_CACHEMANAGER_H


#include <map>
#include <string>
#include <vector>

class CacheManager
{
    public:
        CacheManager();
        unsigned int getElementIndex(const std::string& element);
        std::string getElementFromIndex(unsigned int index) const;

    private:
        std::map<std::string, unsigned int> m_cache;
        std::vector<std::string> m_inverseCache;
};


#endif //TCN3R_CACHEMANAGER_H
