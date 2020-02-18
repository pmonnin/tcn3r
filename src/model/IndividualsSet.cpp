#include <boost/progress.hpp>

#include "IndividualsSet.h"


IndividualsSet::IndividualsSet(const ServerManager &serverManager, const Logger &logger) : m_uriToIndividual()
{
    // Query owl:sameAs links to compute canonical graph
    logger.info("Query owl:sameAs edges");
    std::set<std::pair<std::string, std::string>> sameAsEdges = serverManager.queryTwoElements("?e1 owl:sameAs ?e2");

    // Build owl:sameAs adjacency
    logger.info("Build owl:sameAs adjacency from edges");
    std::map<std::string, std::set<std::string>> sameAsAdjacency;

    boost::progress_display progressBar(sameAsEdges.size());
    for (const auto &e : sameAsEdges)
    {
        sameAsAdjacency[e.first].insert(e.second);
        sameAsAdjacency[e.second].insert(e.first);

        ++progressBar;
    }

    // Compute canonical individuals
    logger.info("Compute canonical individuals from owl:sameAs adjacency");
    progressBar.restart(sameAsAdjacency.size());
    for (const auto &adjList : sameAsAdjacency)
    {
        if (m_uriToIndividual.find(adjList.first) == m_uriToIndividual.end())
        {
            std::set<std::string> connectedComponent;
            connectedComponent.insert(adjList.first);

            std::set<std::string> toExpand;
            toExpand.insert(adjList.first);

            while (not toExpand.empty())
            {
                std::set<std::string> newToExpand;
                for (const auto &uri : toExpand)
                {
                    newToExpand.insert(sameAsAdjacency[uri].begin(), sameAsAdjacency[uri].end());
                }

                toExpand.clear();
                std::set_difference(newToExpand.begin(), newToExpand.end(), connectedComponent.begin(),
                                    connectedComponent.end(), std::inserter(toExpand, toExpand.begin()));
                connectedComponent.insert(toExpand.begin(), toExpand.end());
            }

            auto *individual = new Individual(connectedComponent);
            for (const auto &rdfIndex : connectedComponent)
            {
                m_uriToIndividual[rdfIndex] = individual;
            }
        }

        ++progressBar;
    }
}

IndividualsSet::~IndividualsSet()
{
    std::set<Individual*> toDelete;
    for (const auto &u2i : m_uriToIndividual)
    {
        toDelete.insert(u2i.second);
    }

    for (const auto &i : toDelete)
    {
        delete i;
    }
}

Individual* IndividualsSet::getIndividualFromURI(const std::string &uri)
{
    if (m_uriToIndividual.find(uri) == m_uriToIndividual.end())
    {
        std::set<std::string> uris;
        uris.insert(uri);
        m_uriToIndividual[uri] = new Individual(uris);
    }

    return m_uriToIndividual[uri];
}

std::map<Individual*, std::set<Individual*>> IndividualsSet::getAdjacency(Predicate *p)
{
    std::map<Individual*, std::set<Individual*>> adjacency;

    for (const auto &u2i : m_uriToIndividual)
    {
        adjacency[u2i.second] = u2i.second->getAdjacentIndividuals(p);
    }

    return adjacency;
}
