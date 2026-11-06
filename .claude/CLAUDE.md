# Git access

Git is read-only for Claude in this repo. The allow/deny rules are in `.claude/settings.json`.

## Only run git commands that have a rule

Before running any git command, check that it matches an allow or deny rule in `.claude/settings.json`. If it matches no rule, do not run it; tell the user which command you wanted to run and why.

## Git aliases

Never run git aliases.

