# serious-meta-library
For serious programmers.

## v1 API

- "sl/meta/traits.hpp"
    - unique: inheritance tags and concepts for default c-tors and assignment op-s
    - constructible, count_fields: field counting for aggregate types
    - concept, is_specialization: compile-time checks

- "sl/meta/string.hpp" 
    - string that can be used as template parameter
- "sl/meta/tuple.hpp"
    - tie_as_tuple for structs, see tests for more examples
    - different flavours of for_each in tuple
- "sl/meta/lifetime.hpp"
    - stack-based resource lifetime management
    - correct pointer forwarding
    - lazy evaluation
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
    - constexpr match statement and mapping, can be bidirectional
    - constexpr pattern matching of typical "union" containers (variant/expected/optional/ptr)
- "sl/meta/storage.hpp"
    - persistent storage of items with unique identifiers
