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
    - static_string: can be used as a template parameter
- "sl/meta/monad.hpp"
    - type alias on tl::optional and tl::expected (waiting for C++23 std::optional and std::expected)
    - combinators: WIP
- "sl/meta/func.hpp"
    - type alias for function
    - pipeline, lift, lazy_eval: utilities
- "sl/meta/lifetime.hpp"
    - defer, finalizer: stack-based resource lifetime management
    - deref: correct perfect pointer forwarding
- "sl/meta/match.hpp"
    - match statement and mapping, can be bidirectional
    - pattern matching for typical sum-types (variant/expected/optional/ptr)
- "sl/meta/hash.hpp"
    - fnv1a: constexpr hash function (TODO: more)
    - hash_string_view: string_view with precalculated hash
- "sl/meta/tuple.hpp"
    - tie_as_tuple: tie an aggregate as tuple (see tests)
    - decay, enumerate, for_each, for_each_type: utilities
- "sl/meta/enum.hpp"
    - to/from string
    - flag
- "sl/meta/intrusive.hpp"
    - forward_list: node and non-owning container
    - algorithm: node
- "sl/meta/storage.hpp"
    - persistent, persistent_array: persistent storage of items with unique identifiers
    - unique_string: persistent storage for unique strings

- "sl/meta/field.hpp"
    - struct/aggregate field descriptors
