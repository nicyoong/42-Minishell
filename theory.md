# Minishell: Core Concepts and Theory

This document delves into the theoretical foundations behind building a Unix shell (minishell), covering parsing, process management, and system interactions.

---

## 1. Abstract Syntax Tree (AST)

### Representation of Commands
- **Hierarchical Structure**: An AST is a tree where each node represents a command component (e.g., simple command, pipe, redirection). Internal nodes denote control operators, while leaves hold command arguments.
- **Execution Flow**: The tree is traversed post-order to respect operator precedence (e.g., pipes execute left-to-right, redirections bind before commands).

### Node Types
- **Command Nodes**: Store executable names and arguments.
- **Operator Nodes**: Represent pipelines (`|`), logical operators (`&&`, `||`), or redirections (`>`, `>>`, `<`).
- **Redirection Nodes**: Contain target files and descriptors (e.g., `STDOUT` for `>`).

---

## 2. Parsing and Lexing

### Lexical Analysis (Tokenization)
- **State Machine Approach**: Handles quoted strings (`'...'`, `"..."`) and escape characters (`\`) by tracking context (e.g., inside quotes vs. normal mode).
- **Token Types**: Produces tokens for words, operators, pipes, and redirections. Whitespace separates tokens unless quoted.

### Syntax Analysis (Parsing)
- **Grammar Rules**: Defined using context-free grammar (CFG) to validate token sequences. 
- **Error Recovery**: Detects mismatched quotes or invalid operator positions (e.g., `> file <`).
---

## 3. Execution Pipeline

### External Commands
- **Process Creation**: `fork()` clones the shell process; `execve()` replaces the child’s memory space with the target binary.
- **PATH Resolution**: Searches directories in `$PATH` (via `access()` or `stat()`) to locate executables.

### Built-in Commands
- **Direct Execution**: Modify shell state directly:
- `cd`: Changes the working directory using `chdir()` and updates `PWD`/`OLDPWD`.
- `export`: Modifies the environment variable list (a linked list or `char **`).

---

## 4. Memory Management

### Resource Tracking
- **AST Nodes**: Allocated dynamically during parsing; freed post-execution.
- **File Descriptors**: Closed explicitly after redirections/pipes to prevent leaks.

### Tools
- **Valgrind**: Detects memory leaks and invalid accesses by tracking allocations/deallocations.

---

## 5. Signal Handling

### Signal Strategies
- **Parent Shell**: Ignores `SIGINT` (Ctrl-C) and `SIGQUIT` (Ctrl-\) using custom handlers to avoid termination.
- **Child Processes**: Reset to default signal behavior (`SIG_DFL`) so Ctrl-C kills them as expected.

---

## 6. Pipes and Redirections

### Pipeline Implementation
1. **Pipe Creation**: `pipe(fd)` creates two file descriptors (`fd[0]` for read, `fd[1]` for write).
2. **Redirection**: `dup2(old_fd, new_fd)` replaces `stdin`/`stdout` with pipe ends or file descriptors.
3. **Concurrency**: Commands in a pipeline run in parallel, with data flowing left-to-right.

### Redirection Types
- **Overwrite (`>`)**
- **Append(`>>`)**

## 7. Error Handling

### Error Types
- **Syntax Errors**:  
  - Unclosed quotes (`"hello`, `'world`)  
  - Invalid operator sequences (e.g., `|||`, `> file <`)  
- **Runtime Errors**:  
  - Non-existent commands (e.g., `./invalid_bin`)  
  - Permission denied (e.g., accessing protected files)  

### Strategies
- **Graceful Exit**:  
  - Display descriptive errors (e.g., `perror("minishell")`) without terminating the shell.  
  - Example: `minishell: syntax error near unexpected token '|'`.  
- **Status Codes**:  
  - Return POSIX-compliant exit codes:  
    - `127`: Command not found  
    - `126`: Permission denied  
    - `2`: General syntax error  

---

## 8. Collaboration & Debugging

### Version Control
- **Git Workflow**:  
  - Atomic commits for logical changes (e.g., "Add pipe support", "Fix quote handling").  
  - Rebasing to maintain a linear project history.  
- **Testing**:  
  - Automated scripts for edge cases:  
    - Heredocs with nested delimiters: `cat << EOF << END`  
    - Redirection combinations: `cmd1 < input.txt > output.txt | cmd2`  

---

## 9. Shell Internals

### Kernel Interaction
- **System Calls**:  
  - Process creation: `fork()`  
  - Program execution: `execve()`  
  - Process synchronization: `waitpid()`  
  - File operations: `open()`, `read()`, `close()`  
- **Environment Variables**:  
  - Accessed via `extern char **environ`.  
  - Modified using `setenv()`/`unsetenv()` for built-ins like `export`/`unset`.  