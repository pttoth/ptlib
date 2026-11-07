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
