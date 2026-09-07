
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

# SENG21213-OS - Stage 4: RAM Disk File System & Kernel Verification

This document outlines the implementation and testing of the RAM Disk File System (Stage 4) and core kernel features (Memory Management, Threads, and Shell Commands) for SENG21213-OS.

---

## 1. Core Kernel & Memory Management Tests

### Physical Memory Manager (PMM) & Memory Info
* **Command:** `mem` (or `meminfo`)
* **Description:** Displays total and free physical memory frames managed by the PMM.
* **Screenshot:**
  <img width="783" alt="meminfo" src="https://github.com/user-attachments/assets/c0b57e77-3087-49d2-afef-4c240eec8a1a" />

* **Command:** `pmmtest`
* **Description:** Executes a 100-frame allocation and deallocation leak test to verify memory safety.
* **Screenshot:**
  <img width="783" alt="pmmtest" src="https://github.com/user-attachments/assets/c0b57e77-3087-49d2-afef-4c240eec8a1a" />

### Thread Synchronization Demo
* **Command:** `threads`
* **Description:** Demonstrates race conditions (mutex) and the producer-consumer problem using semaphores.
* **Screenshot:**
  <img width="783" alt="threads" src="https://github.com/user-attachments/assets/c0b57e77-3087-49d2-afef-4c240eec8a1a" />

---

## 2. Filesystem Testing Commands & Verification

### A. Creating and Writing to the First File
* **Command:** `touch a.txt`
* **Description:** Creates an empty file named `a.txt` by allocating an inode and directory entry.

* **Command:** `write a.txt Hello`
* **Description:** Writes the string `"Hello"` into `a.txt` using the allocated data blocks on the RAM disk.

* **Command:** `ls`
* **Description:** Lists all active files present in the RAM disk file system.
* **Expected Output:** `a.txt`, `b.txt`
* **Screenshot:**
  <img width="783" height="477" alt="ls" src="https://github.com/user-attachments/assets/c0b57e77-3087-49d2-afef-4c240eec8a1a" />

---

### B. Reading File Content
* **Command:** `cat a.txt`
* **Description:** Reads and outputs the contents stored within `a.txt` to the VGA display.
* **Expected Output:** `Hello`

---

### C. Adding a Second File and Updating State
* **Command:** `touch b.txt`
* **Description:** Creates a second file named `b.txt`.

* **Command:** `write b.txt Testing123`
* **Description:** Writes data into `b.txt`.

---

### D. Deleting a File (`rm`)
* **Command:** `rm a.txt`
* **Description:** Unlinks `a.txt`, freeing its associated blocks and inode index.
* **Screenshot:**
  <img width="757" height="438" alt="rm a txt" src="https://github.com/user-attachments/assets/04e40bd8-abc1-4a49-b988-dc67fe892f83" />

* **Command:** `ls`
* **Description:** Verifies that `a.txt` has been removed and only `b.txt` remains.
* **Expected Output:** `b.txt`
* **Screenshot:**
  <img width="747" height="433" alt="ls" src="https://github.com/user-attachments/assets/952f7e61-ad98-4626-adf9-def53a765c74" />

---

### E. Creating Multiple Files
* **Commands:** 
  ```text
  touch c.txt
  touch d.txt
  touch e.txt
# Clean build artifacts
make clean



