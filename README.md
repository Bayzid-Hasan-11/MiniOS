# 🖥️ MiniOS: POSIX Systems Engineering Environment

### 🔗 Project Links
*   **GitHub Repository:** https://github.com/Bayzid-Hasan-11/MiniOS
*   **Video Demonstration (2-5 min):** https://youtu.be/_oMEgzVpzAk

---

## 🌟 Short Introduction
**MiniOS** is a custom-built, fully functional Linux shell and operating system management environment written entirely in C. 

The core philosophy of this project was **zero simulated data**. Rather than building a graphical dashboard that merely reads system states, MiniOS acts as a native control layer. It leverages raw POSIX system calls to interact directly with the Linux kernel—managing memory, routing file descriptors, handling asynchronous hardware interrupts, and dynamically parsing kernel files. 

---

## 🧠 Creative Skills Showcase: What This Project Proves
Building MiniOS required bridging the gap between high-level application logic and low-level kernel architecture. This project showcases mastery of the following Operating Systems domains:

*   🛡️ **Process Lifecycle Control:** Moving beyond simple execution to master state manipulation. Successfully implemented background threading, orphaned process reaping (preventing memory leaks), and live process suspension/resumption in RAM.
*   🚦 **Advanced Signal Routing:** Proving that an application can survive hardware interrupts. Designed the shell to intercept `SIGINT` (Ctrl+C), protecting the parent host while passing the interrupt safely to executing child processes.
*   🔄 **Memory & IPC Engineering:** Solved the classic "infinite blocking deadlock" in pipes by manually managing standard input/output streams and closing orphaned file descriptors in memory using `dup2()`.
*   🔒 **Thread Synchronization:** Conquered the hardest topic in OS theory by solving the Producer-Consumer race condition using native POSIX Mutexes and Semaphores to govern shared memory arrays.
*   📂 **Raw Filesystem Navigation:** Bypassed high-level APIs to read raw disk superblocks (`statvfs`) and inode bitmasks (`stat`), proving a deep understanding of how Linux structures storage.

---

## 🏗️ Core Architecture & Subsystems Implemented
To give a complete picture of the engineering behind this project, here is every major subsystem built into the MiniOS environment:

### 1. The Execution Engine (Process Management)
*   **Foreground & Background Tasks:** Utilizes `fork()` and `execvp()` to execute native Linux binaries. Supports background execution using the `&` operator.
*   **Zombie Process Reaping:** Implements a `SIGCHLD` event listener via `sigaction()`. When a background process finishes, the handler uses `waitpid(-1, NULL, WNOHANG)` to silently reap the exit status, completely eliminating Zombie processes and PID leaks.

### 2. Inter-Process Communication (IPC) & I/O
*   **Command Piping (`|`):** Uses the `pipe()` system call to create a kernel memory buffer, chaining processes together (e.g., `ls | grep`). 
*   **File Descriptor Routing:** Implements standard I/O redirection (`>`, `<`) by using `open()` to acquire file handles, and `dup2()` to overwrite `STDIN_FILENO` and `STDOUT_FILENO`.

### 3. Hardware & State Signals
*   **The Unkillable Shell:** Uses `signal(SIGINT, SIG_IGN)` in the parent process so the shell survives user interrupts, while restoring `SIG_DFL` in child processes so rogue programs can still be terminated.
*   **Process Freezing:** Uses `kill()` to send `SIGSTOP` (freezing a process entirely in RAM) and `SIGCONT` (resuming execution). 

### 4. Live Kernel Monitoring (`/proc` Parsing)
*   **Memory Parsing:** Reads `/proc/meminfo` dynamically to calculate total, used, and free physical hardware RAM.
*   **Process Table Mapping:** Opens `/proc`, iterates through numbered PID directories, and parses the `/proc/[PID]/stat` virtual files to display the live state (Running, Sleeping, Stopped, Zombie) of system tasks.

### 5. Advanced Filesystem & Disk Management
*   **Dynamic Prompt Environment:** Uses `getcwd()` to track the process's working directory, printing a dynamically updating, color-coded terminal prompt.
*   **Superblock Querying:** Uses `statvfs()` to query the root filesystem block sizes, converting the raw bytes into human-readable Gigabyte metrics for total disk space.
*   **Metadata Extraction:** Uses `stat()` to retrieve exact file sizes, timestamps, and bitwise parsing of `rwx` permissions.
*   **Recursive Mapping:** Uses `opendir()` and `readdir()` to recursively traverse nested folders, generating a visual, color-coded directory tree.

### 6. Concurrency & Synchronization
*   **Producer-Consumer Engine:** Uses `pthread_create` to spawn independent OS threads sharing a common memory buffer.
*   **Race Condition Prevention:** Wraps critical array modifications in a `pthread_mutex_lock()` and utilizes `sem_wait()` / `sem_post()` semaphores to dynamically track buffer capacity, ensuring threads communicate flawlessly with the OS scheduler.

---

## 🛠️ Command Reference Table

| Command | Action | System Call / Concept Used |
| :--- | :--- | :--- |
| `cd <dir>` | Changes the current working directory. | `chdir()`, `getcwd()` |
| `processes` | Lists live processes from the kernel. | `/proc/[PID]/stat` parsing |
| `kill <pid>`| Gracefully terminates a process. | `kill(pid, SIGTERM)` |
| `pause <pid>`| Freezes a running process in RAM. | `kill(pid, SIGSTOP)` |
| `resume <pid>`| Unfreezes a stopped process. | `kill(pid, SIGCONT)` |
| `memory` | Displays physical system RAM in MB. | `/proc/meminfo` parsing |
| `disk` | Displays total and available disk space. | `statvfs()` superblock query |
| `info <file>`| Parses exact file metadata and inodes. | `stat()` bitmask parsing |
| `tree` | Recursively maps the filesystem structure. | `opendir()`, recursion |
| `sync-demo` | Multithreaded Producer-Consumer demo. | `pthread`, `mutex`, `semaphore` |
| `exit` | Safely terminates the environment. | Loop termination |

---

## 🚀 Compilation & Execution

MiniOS requires a Linux environment (Native Linux, WSL, or macOS) with `gcc` and `make` installed.

```bash
# Clone the repository
git clone [INSERT YOUR GITHUB LINK HERE]
cd minios

# Compile the OS environment
make

# Launch the shell
./minios
