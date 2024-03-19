# serious-meta-library
For serious programmers.

## serious project structure 

- "sl/meta/string.hpp" 
    - string that can be used as template parameter
- "sl/meta/tuple.hpp"
    - tie_as_tuple for structs, see tests for more examples
    - different flavours of for_each in tuple
- "sl/meta/traits.hpp"
    - concepts and compile-time checks 
- "sl/meta/lifetime.hpp"
    - stack-based resource lifetime management
- "sl/meta/field.hpp"
    - struct/aggregate field descriptors
- "sl/meta/optional.hpp"
    - uses tl::optional
    - helpers for tuple<->optional operations
- "sl/meta/func.hpp"
    - meta function utilities
- "sl/meta/hash.hpp"
    - constexpr hash functions
- "sl/meta/match.hpp"
    - constexpr match statement and mapping
    - can be bidirectional
