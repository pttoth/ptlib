# ptlib

#### A library for some generic purpose tools.

### Alias

Class alias definitions to allow easier/friendlier usage of STL features.

### Config

Writes/reads key-value pairs into/from a file where the key is converted from enum values to strings and vice versa.


### Event

Class-independent event object, that can register and call functions sequentially in the order of registration.

### Logger

A 'tee'-like logger that writes simultaneously to the standard output and into a specified log file. The output file is UTF-8 encoded.

Currently it blocks the calling thread.
In later versions this will be moved out into a separate logging process.

### Utilities

Various generic-purpose functions, that simplify the usage of STL solutions to problems.
