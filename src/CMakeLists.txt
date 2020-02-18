find_package(Boost REQUIRED program_options)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")

find_package(CURL REQUIRED)
message(STATUS "CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}")
message(STATUS "CURL_LIBRARIES: ${CURL_LIBRARIES}")
message(STATUS "CURL_VERSION: ${CURL_VERSION_STRING}")

find_package(OpenMP REQUIRED)

if(Boost_FOUND AND CURL_FOUND)
    add_executable(tcn3r main.cpp configuration/Configuration.cpp configuration/Configuration.h io/ServerManager.cpp io/ServerManager.h io/CacheManager.cpp io/CacheManager.h reconciliation/RelationsReconcilier.cpp reconciliation/RelationsReconcilier.h io/Logger.cpp io/Logger.h configuration/DimensionConfiguration.cpp configuration/DimensionConfiguration.h model/Individual.cpp model/Individual.h model/PredicatesSet.cpp model/PredicatesSet.h model/Predicate.cpp model/Predicate.h model/Relation.cpp model/Relation.h model/RelationElement.cpp model/RelationElement.h model/IndividualsSet.cpp model/IndividualsSet.h reconciliation/RelationNotFound.cpp reconciliation/RelationNotFound.h reconciliation/Preorder.cpp reconciliation/Preorder.h reconciliation/SetInclusionPreorder.cpp reconciliation/SetInclusionPreorder.h io/TTLWriter.cpp io/TTLWriter.h reconciliation/IndividualsPreorder.cpp reconciliation/IndividualsPreorder.h reconciliation/AnnotationsPreorder.cpp reconciliation/AnnotationsPreorder.h)
    target_include_directories(tcn3r PUBLIC ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
    target_compile_options(tcn3r PUBLIC -std=c++17 -Wall -Wno-pedantic "${OpenMP_CXX_FLAGS}")
    target_link_libraries(tcn3r ${Boost_LIBRARIES} ${CURL_LIBRARIES} "${OpenMP_CXX_FLAGS}")
endif()
