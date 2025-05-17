# Minishell - 42 Project

## Project Description  
Minishell is a collaborative project at 42 that involves building a simplified Unix shell. It includes command parsing, process execution, and handling of basic shell features like pipes, redirections, and built-in commands. The goal is to deepen understanding of process management, system calls, and shell internals.

---

## Features  
- Basic command execution (e.g., `ls`, `grep`, `echo`)  
- Built-in commands: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`  
- Redirections: `>`, `>>`, `<`, here-documents (`<<`)  
- Pipes (`|`) and multi-pipe command chains  
- Signal handling for `Ctrl-C` (SIGINT) and `Ctrl-\` (SIGQUIT), and EOF handling for `Ctrl-D`
- Environment variable expansion (e.g., `$PATH`)  
- Error handling for syntax and runtime issues  

---

## What We Learned  

### 1. **Abstract Syntax Tree (AST)**  
- Designed an AST to represent the hierarchical structure of parsed commands, enabling efficient traversal and execution.  
- Each node in the tree corresponds to a command, redirection, or pipe, simplifying complex command sequences.  

### 2. **Parsing and Lexing**  
- **Lexical Analysis**: Converted raw input into tokens (e.g., strings, operators) using state machines, handling quotes and escape characters.  
- **Syntax Analysis**: Validated token sequences against grammar rules, detecting errors like unmatched quotes or invalid operators.  
- **AST Construction**: Transformed validated tokens into an executable tree structure.  

### 3. **Execution Pipeline**  
- **Process Creation**: Used `fork()` and `execve()` to execute external binaries, resolving paths via the `PATH` environment variable.  
- **Built-in Commands**: Implemented shell-native functions (e.g., `cd`, `export`) without spawning child processes.  

### 4. **Memory Management**  
- Avoided leaks with careful allocation/freeing of tokens, AST nodes, and file descriptors.  
- Leveraged tools like `valgrind` to detect and resolve memory issues.  

### 5. **Signal Handling**  
- Intercepted `SIGINT` and `SIGQUIT` to prevent shell termination, ensuring only child processes are affected.  
- Restored default signal behavior in child processes.  

### 6. **Pipes and Redirections**  
- Managed file descriptors for pipes (`|`) using `pipe()` and `dup2()`, ensuring proper data flow between commands.  
- Implemented input/output redirections, including appending (`>>`) and here-docs.  

### 7. **Error Handling**  
- Gracefully handled syntax errors (e.g., `> file <`) and runtime failures (e.g., command not found).  
- Maintained shell stability even with invalid user input.  

### 8. **Collaboration & Debugging**  
- Used `git` for version control and teamwork, resolving merge conflicts and maintaining clean code history.  
- Tested edge cases (e.g., empty input, nested quotes) to ensure robustness.  

### 9. **Shell Internals**  
- Gained insight into how shells interact with the OS kernel, environment variables, and process tables.  

---

## Future Expansions (Bonus)  
- **Wildcards**: Expand `*` to match filenames.  
- **Logical Operators**: Support `&&` (and) and `||` (or) for command chaining. 