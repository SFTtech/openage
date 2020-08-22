# Converter Architecture Overview

This document describes the code architecture of the openage converter module.

## Design Principles

Our converter uses hierarchical multi-layered object-oriented code principles similar to those found in
[Domain-Driven Design](https://en.wikipedia.org/wiki/Domain-driven_design) and [WAM](http://wam-ansatz.de/).
The main focus of using these principles was to provide **readability** and **maintainability** as well
as enabling us to quickly integrate extensions that support other games and game versions than the original
Age of Empires 2 release.

A hierarchical structure is achieved by designing objects as part of one of the 5 domains described below.
Every domain defines a category of complexity. The idea is that every object can only access functionality
from domains with the same or lower complexity. For example, an object for data storage should not access
the methods that drive the main conversion process. The resulting code will thus not evolve into a spaghetti
mess.

### Value Object

Value objects are used to store primitive data or as definition of how to read primitive data from files.
In the converter, the parsers utilize these objects to extract attributes from the original Genie Engine
file formats. Extracted attributes are also saved as value objects.

Value objects are treated as *immutable*. Operations on the objects's values will therefore always return
a new object instance with the result and leave the original values as-is.

### Entity Object

Entity objects are used for structures that have a defined identity. An example for such a structure would
be one `Unit` object from AoE2 or a `GameEntity` instance from the openage API. Entity objects are mutable
and can have any number of attributes assigned to them. In the converter, they are used to model the
more complex structures from Genie games such as units, civilizations and techs. The transition to the nyan
API objects is also done utilizing etntity objects.

### Service

Services are interfaces for requesting or passing collections of entity objects and value objects from/to
a processor or tool. It is also used to communicate with external interfaces such as included libraries
or Python packages. A services main job is to translate the data it receives/forwards into a usable format.
Examples for services are internal name lookup service for mapping unit IDs to nyan object names as well as
the nyan file and graphics exporters.

### Processor

Processors operate on a given dataset consisting of multiple entity objects to produce a result. For the
converter, the input dataset is the data read in from Genie file formats, while the result are modpacks
for openage. Processors can be split into several subprocessor modules for which the same hierarchical
structure applies. A subprocessor can never access functions from a parent processor, but use any other
(sub-)processor or entity object/value object.

openage conversion processor are not instanced and implement most of their functions with the `@staticmethod`
decorator. The reason for this is to allow conversion processors for game expansions cross-reference
processor functions of their base game and thus reduce code redundancy.

### Tool

Tools are used to encapsulate processor with a user interface (GUI, TUI or CLI), pass user input to processors
and start the conversion process.
