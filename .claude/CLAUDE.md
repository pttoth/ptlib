# Git access

Git is read-only for Claude in this repo. The allow/deny rules are in `.claude/settings.json`.

## Only run git commands that have a rule

Before running any git command, check that it matches an allow or deny rule in `.claude/settings.json`.
    If it matches no rule:
        - you can only run it if it's read-only
        - tell the user which command it is
        - add a new rule for it

Whenever you update `.claude/settings.json`, insert the allow and block rules alphabetically.

## Git aliases

Never run git aliases.

# CMake style

## Function calls with list elements

When list elements are passed to a function, use this layout:

```cmake
fn( <target|variable> [modifiers]
    list_element1
    list_element2
)
```

e.g.:

```cmake
target_link_libraries( ${LIB_NAME} PUBLIC
    backtrace
    ptlib
)

list( APPEND LIB_GLOBS
    src/${pattern}
    include/${pattern}
)
```

Exception: multiple modifier groups in one call.
Each modifier starts its own group on a new line, its elements aligned after it:

```cmake
target_include_directories( ${LIB_NAME}
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/include/platform/${PLATFORM}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/pch
)
```
