# serious-meta-library
For serious programmers.

## v1 API

- "sl/meta/traits.hpp"
    - unique: inheritance tags and concepts for default c-tors and assignment op-s
    - constructible, count_fields: field counting for aggregate types
    - concept, is_specialization: compile-time checks
- "sl/meta/type.hpp"
    - unit, identity, undefined - fundamental types
    - lazy, const, pack - type evaluation/operations
- "sl/meta/monad.hpp"
    - type alias on tl::optional and tl::expected (waiting for C++23 std::optional and std::expected)
    - combinators: WIP
- "sl/meta/func.hpp"
    - type alias for function
    - pipeline, lift, lazy_eval: utilities

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
- "sl/meta/hash.hpp"
    - constexpr hash functions
- "sl/meta/match.hpp"
    - constexpr match statement and mapping, can be bidirectional
    - constexpr pattern matching of typical "union" containers (variant/expected/optional/ptr)
- "sl/meta/storage.hpp"
    - persistent storage of items with unique identifiers
