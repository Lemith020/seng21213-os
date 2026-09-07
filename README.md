
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




