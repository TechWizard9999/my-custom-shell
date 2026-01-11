---
description: Push shell project stages to personal GitHub repo
---

# Push to GitHub Workflow

This workflow helps push incremental stages of the shell project to your personal GitHub repo while keeping CodeCrafters separate.

## Remotes Setup
- `origin` → CodeCrafters (for testing)
- `github` → Your personal repo (https://github.com/TechWizard9999/my-custom-shell.git)

---

## How to Use This Workflow

When you say something like:
- "Push the basic shell" → I push just main.cpp with basic REPL
- "Push echo command" → I add EchoCommand.h and update main.cpp
- "Push cd command" → I add CdCommand.h and update main.cpp

---

## Stages to Push (in order)

### Stage 1: Basic Shell (Brute Force)
- Basic main.cpp with REPL loop (no OOP)
- Just reads input and prints "command not found"

### Stage 2: Exit Command
- ExitCommand.h
- Command.h (base interface)
- Updated main.cpp

### Stage 3: Echo Command
- EchoCommand.h
- Updated main.cpp

### Stage 4: Type Command
- TypeCommand.h
- PathSearcher.h
- Updated main.cpp

### Stage 5: Pwd Command
- PwdCommand.h
- Updated main.cpp

### Stage 6: Cd Command
- CdCommand.h
- Updated main.cpp

### Stage 7: External Commands
- ExternalCommand.h
- Shell.h
- Final main.cpp

---

## Push Commands

```bash
# Check what's different
git status

# Add specific files
git add src/filename.h src/main.cpp

# Commit with descriptive message
git commit -m "feat: implement [command name] builtin"

# Push to your GitHub
git push github main
```

---

## Daily Usage

Just tell me:
> "Push [stage/command name] to GitHub"

And I'll handle it!
