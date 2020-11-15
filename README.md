# tcn3r

Reconciliation rules for n-ary relations encoded in RDF triples

A detailed description of the motivation and the algorithms of tcn3r is available in [the related article](https://arxiv.org/pdf/2002.08103.pdf).

## Citing tcn3r

When citing tcn3r, please use the following reference:

Pierre Monnin, Miguel Couceiro, Amedeo Napoli, and Adrien Coulet. "Knowledge-Based
Matching of n-ary Tuples". In: Ontologies and Concepts in Mind and Machine - 25th
International Conference on Conceptual Structures, ICCS 2020, Bolzano, Italy, September
18–20, 2020, Proceedings. Ed. by Mehwish Alam, Tanya Braun, and Bruno Yun. Vol. 12277.
Lecture Notes in Computer Science. Springer, 2020, pp. 48–56. doi: 10.1007/978-3-030-
57855-8_4. url: https://doi.org/10.1007/978-3-030-57855-8_4.

```
@inproceedings{Monnin2020,
  author    = {Pierre Monnin and
               Miguel Couceiro and
               Amedeo Napoli and
               Adrien Coulet},
  editor    = {Mehwish Alam and
               Tanya Braun and
               Bruno Yun},
  title     = {Knowledge-Based Matching of n-ary Tuples},
  booktitle = {Ontologies and Concepts in Mind and Machine - 25th International Conference
               on Conceptual Structures, {ICCS} 2020, Bolzano, Italy, September 18-20,
               2020, Proceedings},
  series    = {Lecture Notes in Computer Science},
  volume    = {12277},
  pages     = {48--56},
  publisher = {Springer},
  year      = {2020},
  url       = {https://doi.org/10.1007/978-3-030-57855-8_4},
  doi       = {10.1007/978-3-030-57855-8_4},
}
```

## Execution

### ``batch`` mode

Executes reconciliation rules on every pair of relationships in the triplestore.

#### Execution (without Docker)

```bash
tcn3r --configuration conf.json -o output.ttl --simlimit SL --complimit CL --dimensionlimit DL --max-rows MR -t threads
```

where:

* *conf.json*: is the configuration file needed to configure the scripts -- see below
* *output.ttl*: is the path to the output TTL file where the generated links between relationships will be stored
* *SL*: Minimum similarity on non-empty aggregated dimensions to consider relations as related (< 0 to disable)
* *CL*: Minimum number of non-empty comparable aggregated dimensions to consider relations as related (< 0 to disable)
* *DL*: Minimum number of non-empty aggregated dimensions to apply simlimit or complimit (< 0 to disable)
* *MR*: Max number of rows the SPARQL endpoint can return for a query
* *threads*: number of threads to use when comparing relations (e.g., 8)

#### Execution (in Docker)

You can use the target ``run`` of the provided Makefile that calls the Docker image with:

```bash
docker run --rm $(MAPUSER) -v ${PWD}/data:/data $(INAME):$(VERSION) --configuration /data/conf.json.example -o /data/output.ttl --simlimit 0.8 --complimit 2 --dimensionlimit 2 --max-rows 10000 --explain false -t 4
```

The ``data`` subdirectory of the current directory is shared with the Docker container as ``/data``.
It is expected that the JSON configuration file is in this directory.
``/data`` is also the directory where the output TTL file will be stored.
``simlimit`` is set to 0.8, ``complimit`` to 2 and ``dimensionlimit`` is set to 2.
4 threads will be used.

### ``explain`` mode

#### Execution (without Docker)

```bash
tcn3r --configuration conf.json -o output.ttl --simlimit SL --complimit CL --dimensionlimit DL --max-rows MR --explain true
```

where:

* *conf.json*: is the configuration file needed to configure the scripts -- see below
* *output.txt*: is the path to the output text file where explanations of links between relationships will be stored
* *SL*: Minimum similarity on non-empty aggregated dimensions to consider relations as related (< 0 to disable)
* *CL*: Minimum number of non-empty comparable aggregated dimensions to consider relations as related (< 0 to disable)
* *DL*: Minimum number of non-empty aggregated dimensions to apply simlimit or complimit (< 0 to disable)
* *MR*: Max number of rows the SPARQL endpoint can return for a query

URIs of relations to compare will be asked interactively.

#### Execution (in Docker)

Not available.

## Input

### Configuration JSON file

A configuration JSON file is needed to configure the scripts. [An example is provided](data/conf.json.example).
It should contains:

```json
{
    "server-address": "http://pgxlod.loria.fr/sparql",
    "url-json-conf-attribute": "format",
    "url-json-conf-value": "application/sparql-results+json",
    "url-default-graph-attribute": "default-graph-uri",
    "url-default-graph-value": "http://pgxlod.loria.fr/",
    "url-query-attribute": "query",
    "timeout": 10000000,
    "relation-types": [
        "http://pgxo.loria.fr/PharmacogenomicRelationship"
    ],
    "dimensions": {
        "GeneticFactor": {
            "ind-types": [
                "http://pgxo.loria.fr/GeneticFactor"
            ],
            "rel2ind-predicates": [
                "http://pgxo.loria.fr/isAssociatedWith",
                "http://pgxo.loria.fr/isNotAssociatedWith"
            ],
            "ind2dep-predicates": [
            ],
            "preorder": "Individuals",
            "ind-leq-predicates": [
                "http://purl.obolibrary.org/obo/BFO_0000050"
            ],
            "ind-geq-predicates": [
                "http://purl.obolibrary.org/obo/BFO_0000051"
            ]
        },
        "Drug": {
            "ind-types": [
                "http://pgxo.loria.fr/Drug"
            ],
            "rel2ind-predicates": [
                "http://pgxo.loria.fr/isAssociatedWith",
                "http://pgxo.loria.fr/isNotAssociatedWith"
            ],
            "ind2dep-predicates": [
            ],
            "preorder": "Annotations",
            "ann-base-uris": [
                "http://purl.obolibrary.org/obo/CHEBI_",
                "http://bio2rdf.org/chebi:",
                "http://identifiers.org/chebi/",
                "http://purl.bioontology.org/ontology/UATC/",
                "http://bio2rdf.org/atc:",
                "http://identifiers.org/atc/"
            ],
            "ind2ann-predicates": [
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
            ],
            "ann-leq-predicates": [
                "http://www.w3.org/2000/01/rdf-schema#subClassOf"
            ],
            "ann-geq-predicates": [
            ]
        },
        "Phenotype": {
            "ind-types": [
                "http://pgxo.loria.fr/Phenotype"
            ],
            "rel2ind-predicates": [
                "http://pgxo.loria.fr/isAssociatedWith",
                "http://pgxo.loria.fr/isNotAssociatedWith"
            ],
            "ind2dep-predicates": [
                "http://purl.obolibrary.org/obo/RO_0002502"
            ],
            "preorder": "Annotations",
            "ann-base-uris": [
                "http://purl.bioontology.org/ontology/MESH/",
                "http://bio2rdf.org/mesh:",
                "http://identifiers.org/mesh/"
            ],
            "ind2ann-predicates": [
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
            ],
            "ann-leq-predicates": [
                "http://www.w3.org/2000/01/rdf-schema#subClassOf"
            ],
            "ann-geq-predicates": [
            ]
        }
    },
    "output-pred-equal": "http://www.w3.org/2002/07/owl#sameAs",
    "output-pred-equiv": "http://www.w3.org/2004/02/skos/core#closeMatch",
    "output-pred-leq": "http://www.w3.org/2004/02/skos/core#broadMatch",
    "output-pred-geq": "http://www.w3.org/2004/02/skos/core#narrowMatch",
    "output-pred-comparable": "http://www.w3.org/2004/02/skos/core#relatedMatch",
    "output-pred-dependency-related": "http://www.w3.org/2004/02/skos/core#related"
}
```

with:

* _server-address_: address of the SPARQL endpoint to query
* _url-json-conf-attribute_: URL attribute to use to get JSON results
* _url-json-conf-value_: value of the _url-json-conf-attribute_ to get JSON results
* _url-default-graph-attribute_: URL attribute to use to define the default graph
* _url-default-graph-value_: value of _url-default-graph-attribute_ to define the default graph
* _url-query-attribute_: URL attribute to use to define the query
* _timeout_: timeout value for HTTP requests
* _relation-types_: URIs of classes whose instances are relationships to reconcile
* _dimensions_: dictionary of dimensions. Each dimension should contain:
  * _ind-types_: classes that are instantiated by elements of this dimension. Subclasses will be considered as well.
  * _rel2ind-predicates_: predicates connecting relations with elements of this dimension. Subproperties will be considered as well.
  * _ind2dep-predicates_: predicates connecting elements of this dimension with potential dependencies. Subproperties will be considered as well.
  * _preorder_: preorder to use for comparison on this dimension. Potential values:
    * _SetInclusion_: set inclusion preorder
    * _Individuals_: preorder between individuals, must be specified:
      * _ind-leq-predicates_: list of predicates indicating that an individual is lower or equal than another. Subproperties will be considered as well.
      * _ind-leq-predicates_: list of predciates indicating that an individual is greater or equal than another. Subproperties will be considered as well.
    * _Annotations_: preorder based on annotations of individuals, must be specified:
      * _ann-base-uris_: prefixes of URIs of annotations to consider
      * _ind2ann-predicates_: predicates linking elements of this dimension to their annotations. Subproperties will be considered as well.
      * _ann-leq-predicates_: predicates indicating that an annotation is lower or equal than another. Subproperties will be considered as well.
      * _ann-geq-predicates_: predicates indicating that an annotation is greater or equal than another. Subproperties will be considered as well.
* _output-pred-equal_: URI of a predicate to use to identify equal relationships
* _output-pred-equiv_: URI of a predicate to use to identify equivalent relationships
* _output-pred-leq_: URI of a predicate to use to identify lower or equal relationships
* _output-pred-geq_: URI of a predicate to use to identify greater or equal relationships
* _output-pred-comparable_: URI of a predicate to use to identify comparable relationships
* _output-do-related-predicate_: URI of a predicate to use to identify relationships that are related

## Tests

A [test/pgxo+test.owl](test/pgxo+test.owl) file is available to test tcn3r.
It should be imported in a triplestore.
After the import, run tcn3r using the test configuration provided in [test/test-conf.json](test/test-conf.json) and the following parameters
``--simlimit 0.85 --complimit 3 --dimensionlimit 3``.
You may need to adapt ``server-address`` depending on the location of the triplestore you use.
The description of the test cases can be found in [test/documentation-tests.pdf](test/documentation-tests.pdf).
The (sorted) expected results can be found in [test/expected-output.ttl](test/expected-output.ttl).

## Dependencies

* C++17
* boost
* libcurl
* OpenMP
