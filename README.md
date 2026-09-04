
# SENG21213-OS

## Stage 03

A custom 32-bit x86 operating system developed as part of the **SENG21213: Computer Architecture & Operating Systems** course at the Department of Software Engineering, Faculty of Engineering, University of Kelaniya.

---

## Features

- **x86 32-bit Protected Mode:** Built from scratch using C and Assembly (`nasm`).
- **VGA Text Mode Driver:** Supports custom color text rendering, box drawing, and a graphical boot splash screen.
- **Interactive Kernel Shell (`ksh`):** Built-in command interpreter supporting various kernel utilities.
- **Process Management:** Supports process creation, listing (`ps`), and termination (`kill`).
- **Thread & Synchronization Primitives:** Implements thread management along with Mutexes and Semaphores (demonstrated via Race Condition and Producer-Consumer problem solutions).
- **Physical Memory Manager (PMM):** Manages physical memory frames/pages with allocation and deallocation support.

---

## Shell Commands & Screenshots

Here is a demonstration of the core kernel commands and their outputs:

### 1. Help Command (`help`)
Displays the list of available commands in the kernel shell.
<img width="717" height="461" alt="Screenshot 2026-09-04 194138" src="https://github.com/user-attachments/assets/7644519b-df4e-4f4f-b9cf-f250b9b5b725" />



### 2. Memory Information (`mem`)
Displays details regarding the Physical Memory Manager and memory status.
<img width="717" height="461" alt="Screenshot 2026-09-04 194047" src="https://github.com/user-attachments/assets/20727033-db39-4d1f-90e3-22088e230358" />



### 3. Physical Memory Manager Test (`pmmtest`)
Runs a memory leak test by allocating and freeing 100 memory pages successfully.
<img width="718" height="463" alt="Screenshot 2026-09-04 194111" src="https://github.com/user-attachments/assets/5985829f-e1ec-489e-b93b-227245dbd491" />



### 4. Process Status (`ps`)
Lists the active processes running in the system with their PID, state, and name.
<img width="715" height="461" alt="Screenshot 2026-09-04 194156" src="https://github.com/user-attachments/assets/1af362d5-4e2c-443d-94cc-477209e70fef" />



### 5. Thread & Synchronization Demo (`threads`)
Executes the Stage 2 thread demos highlighting race condition handling via mutexes and the producer-consumer problem using semaphores.
<img width="717" height="463" alt="Screenshot 2026-09-04 194216" src="https://github.com/user-attachments/assets/5bbdcb1e-5e9b-43da-9a6c-ac7c38debc72" />



---

## Getting Started & Build Instructions

To build and run this operating system locally (requires `gcc`, `nasm`, `make`, and `qemu`):

```bash
# Clone the repository
git clone [https://github.com/Lemith020/seng21213-os.git](https://github.com/Lemith020/seng21213-os.git)
cd seng21213-os

# Build the OS image
make

# Run in QEMU emulator
make run

# Clean build artifacts
make clean



# SENG21213-OS — Stage 0: Kernel Foundations

> **Course**: SENG 21213 – Computer Architecture & Operating Systems  
> **Year**: 2nd Year, Software Engineering  
> **Assignment**: Build your own x86 Operating System

---

## What Is This?

This is **Stage 0** of your semester-long OS assignment. Over 5 lecture milestones
(Lectures 8–12), your team will transform this minimal kernel into a functioning
operating system with process management, threading, memory management, and a
file system.

```
seng21213-os/
├── boot/
│   └── boot.asm          ← MBR Bootloader (NASM, 16-bit → 32-bit transition)
├── kernel/
│   ├── kernel_entry.asm  ← Protected-mode entry, calls kernel_main()
│   ├── kernel.c          ← Main kernel: shell loop, command dispatch
│   ├── vga.c / vga.h     ← VGA 80×25 text-mode driver
│   ├── keyboard.c / .h   ← PS/2 keyboard polling driver
├── include/
│   └── types.h           ← Primitive types (no libc!)
├── linker.ld             ← Linker script (kernel at 0x10000)
├── Makefile              ← Build system
├── Dockerfile            ← Reproducible build environment
└── README.md             ← You are here
```

---

## Milestone Schedule

| Lecture | Milestone | Files to Add |
|---------|-----------|-------------|
| L08 | ✅ Stage 0 – Boot + VGA + Shell | *Given to you* |
| L09 | Process Management | `kernel/process.c`, `kernel/scheduler.c` |
| L10 | Threads & Synchronisation | `kernel/thread.c`, `kernel/mutex.c` |
| L11 | Memory Management | `kernel/pmm.c`, `kernel/vmm.c` |
| L12 | File System | `kernel/fs.c`, `kernel/ramdisk.c` |

---

## Quick Start

### Option A: Docker (Recommended for all platforms)

```bash
# 1. Install Docker Desktop (Windows/Mac) or Docker Engine (Linux)
# 2. Build the image once:
docker build -t seng21213-os-builder .

# 3. Build the OS:
docker run --rm -v "$(pwd)":/os seng21213-os-builder

# 4. Run in QEMU (install QEMU locally):
qemu-system-i386 -drive format=raw,file=seng21213-os.img -m 32M
```

### Option B: Native Linux/WSL2

```bash
# Ubuntu/Debian
sudo apt install nasm gcc gcc-multilib binutils qemu-system-x86 make

# Build
make all

# Run
make run
```

### Option C: macOS (Homebrew)

```bash
brew install nasm x86_64-elf-binutils qemu

# You also need an i686-elf-gcc cross-compiler:
# See: https://wiki.osdev.org/GCC_Cross-Compiler
make all
make run
```

---

## Understanding the Boot Process

```
Power On
  │
  ▼
BIOS (firmware in ROM)
  │  Loads 512-byte MBR from disk sector 1 into RAM at 0x7C00
  ▼
boot/boot.asm  (Real Mode, 16-bit)
  │  Prints "Loading SENG21213-OS..."
  │  Reads 64 sectors (kernel) from disk into RAM at 0x10000
  │  Sets up GDT (Global Descriptor Table)
  │  Switches CPU to 32-bit Protected Mode
  │  Far-jumps to 0x10000
  ▼
kernel/kernel_entry.asm  (Protected Mode, 32-bit)
  │  Calls kernel_main()
  ▼
kernel/kernel.c  →  kernel_main()
  │  vga_init()     – set up text display
  │  kb_init()      – set up keyboard
  │  print_splash() – welcome screen
  │  shell_run()    – interactive shell (infinite loop)
  ▼
Your code from here...
```

---

## Building Lecture 9: Process Management

When you reach Lecture 9, you'll add process support. Here's the interface to implement:

```c
/* kernel/process.h  — you write this! */

#define MAX_PROCESSES    16
#define STACK_SIZE     4096

typedef enum { READY, RUNNING, BLOCKED, TERMINATED } proc_state_t;

typedef struct pcb {
    uint32_t      pid;
    proc_state_t  state;
    uint32_t      esp;          /* Saved stack pointer */
    uint32_t      eip;          /* Saved instruction pointer */
    uint32_t      stack[STACK_SIZE / 4];
    struct pcb   *next;         /* For linked-list ready queue */
} pcb_t;

void   process_init(void);
pcb_t *process_create(void (*entry)(void));
void   process_yield(void);        /* Trigger context switch */
void   process_exit(void);
void   scheduler_tick(void);       /* Called by timer IRQ (Lecture 10) */
```

---

## Debugging Tips

```bash
# Debug with GDB
make run-debug
# In another terminal:
gdb
(gdb) target remote :1234
(gdb) set architecture i386
(gdb) symbol-file build/kernel.elf
(gdb) break kernel_main
(gdb) continue

# Inspect the disk image
xxd seng21213-os.img | head -32    # View MBR
xxd seng21213-os.img | grep -c aa55  # Verify boot signature
```

---

## Key Learning Resources

| Topic | Reference |
|-------|-----------|
| x86 Protected Mode | Intel IA-32 Manual, Vol 3, Chapter 3 |
| VGA Text Mode | OSDev Wiki: Text UI |
| Interrupts / IDT | Stallings Ch.1; OSDev: IDT |
| Process Management | Stallings Ch.3–4 (your lecture notes) |
| Memory Management | Stallings Ch.7–8 (your lecture notes) |
| OSDev community | https://wiki.osdev.org |

---

## Assessment Rubric (per milestone)

| Criterion | Weight |
|-----------|--------|
| Code compiles and kernel boots in QEMU | 30% |
| Feature implementation (correct behaviour) | 40% |
| Code quality and comments | 20% |
| Lab demo and viva questions | 10% |

---

*Happy hacking! Remember: every commercial OS started exactly like this.*
