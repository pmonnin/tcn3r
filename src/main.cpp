#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <curl/curl.h>

#include "configuration/Configuration.h"
#include "io/CacheManager.h"
#include "io/Logger.h"
#include "io/ServerManager.h"
#include "io/TTLWriter.h"
#include "model/Relation.h"
#include "reconciliation/RelationNotFound.h"
#include "reconciliation/RelationsReconcilier.h"

int main(int argc, char* argv[])
{
    Logger logger(INFO);
    logger.info("tcn3r");

    // Initialize libcurl
    if (curl_global_init(CURL_GLOBAL_ALL))
    {
        logger.critical("Error while initializing libcurl");
        std::exit(1);
    }

    try
    {
        // Define command line arguments and parse command line
        boost::program_options::options_description argsDesc("tcn3r -- allowed options");
        argsDesc.add_options()
                (
                    "help,h",
                    "Produce help message"
                )
                (
                    "configuration",
                    boost::program_options::value<std::string>()->default_value("configuration.json"),
                    "Path to the JSON file containing the configuration for tcn3r"
                )
                (
                    "max-rows",
                    boost::program_options::value<int>()->default_value(10000),
                    "Max rows returned by the SPARQL endpoint"
                )
                (
                    "threads,t",
                    boost::program_options::value<int>()->default_value(1),
                    "Number of threads to use to compare relations"
                )
                (
                    "simlimit,s",
                    boost::program_options::value<double>()->default_value(1.0),
                    "Minimum similarity on non-empty aggregated dimensions to consider relations as related (< 0 to disable)"
                )
                (
                    "complimit,c",
                    boost::program_options::value<int>()->default_value(1),
                    "Minimum number of non-empty comparable aggregated dimensions to consider relations as related (< 0 to disable)"
                )
                (
                    "dimensionlimit,d",
                    boost::program_options::value<int>()->default_value(1),
                    "Minimum number of non-empty aggregated dimensions to apply simlimit or complimit (< 0 to disable)"
                )
                (
                    "output,o",
                    boost::program_options::value<std::string>()->default_value("output"),
                    "Output file (TTL format for the batch comparison process, TXT format for the explain mode)"
                )
                (
                    "explain,e",
                    boost::program_options::value<bool>()->default_value(false),
                    "Launch the program in explain mode (interactive)"
                )
                ;

        boost::program_options::variables_map argsParsed;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, argsDesc), argsParsed);
        boost::program_options::notify(argsParsed);

        if (argsParsed.count("help"))
        {
            std::cout << argsDesc << std::endl;
            return 0;
        }
        
        logger.info("From JSON configuration file: " + argsParsed["configuration"].as<std::string>());

        // Store configuration parameters
        Configuration parameters(argsParsed["configuration"].as<std::string>(), argsParsed["max-rows"].as<int>(),
                argsParsed["threads"].as<int>(), argsParsed["output"].as<std::string>(),
                argsParsed["explain"].as<bool>(), argsParsed["dimensionlimit"].as<int>(),
                argsParsed["complimit"].as<int>(), argsParsed["simlimit"].as<double>(),
                logger);
        logger.info(parameters.toString());

        // Prepare ServerManager
        ServerManager serverManager(parameters, logger);
        CacheManager cacheManager;

        // Build the relations reconciliator object
        RelationsReconcilier relationsReconciliator(serverManager, parameters, logger);

        // Explain mode
        if (parameters.isExplainMode())
        {
            bool newExplain(true);
            std::ofstream fileStream(parameters.getOutputPath());
            if(!fileStream)
            {
                logger.critical("Not possible to open output file: " + argsParsed["output"].as<std::string>());
                std::exit(-1);
            }

            do
            {
                std::cout << "Explain mode" << std::endl;

                std::string uri1, uri2;

                std::cout << "Enter URI 1: ";
                std::cin >> uri1;
                std::cout << "Enter URI 2: ";
                std::cin >> uri2;

                try
                {
                    relationsReconciliator.reconcileExplained(uri1, uri2, fileStream, parameters);
                }
                catch(const RelationNotFound &e)
                {
                    logger.error(e.what());
                }

                std::string answer;
                std::cout << "Continue [Y/n]? ";
                std::cin >> answer;

                if (answer == "n" || answer == "N")
                {
                    newExplain = false;
                }

            } while (newExplain);

            fileStream.close();
        }

        // Batch mode
        else
        {
            logger.info("Start batch reconciliation");
            TTLWriter ttlWriter(parameters.getOutputPath(), logger);
            relationsReconciliator.reconcileBatch(ttlWriter, parameters);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception of unknown type!" << std::endl;
        return 1;
    }

    // libcurl global cleanup
    curl_global_cleanup();

    return 0;
}