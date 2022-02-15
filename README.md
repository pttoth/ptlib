# ptlib

A library for some generic purpose tools.

### Array

Currently just a wrapper around 'std::vector'. It's only good for setting up dependencies for custom memory management features added later.

### Config

Writes/reads key-value pairs into/from a file where the key is converted from enum values to strings and vice versa.


### Event

Class-independent event object, that can register and call functions sequentially in the order of registration.

### Logger

A 'tee'-like logger with STL-like syntax that writes simultaneously to the standard output and into a specified log file. 

Currently it blocks the calling thread.
In later versions this will be moved out into a separate logging process.

### Utilities

Various generic-purpose functions, that simplify the usage of STL solutions to problems.
