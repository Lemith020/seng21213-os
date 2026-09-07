#include "fs.h"
#include "pmm.h"
#include "vga.h"
#include "keyboard.h"
#include "process.h"
#include "thread.h"
#include "mutex.h"
#include "semaphore.h"
#include "../include/types.h"

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_about(void);
static void cmd_echo(const char *args);
static void cmd_mem(void);
static void cmd_pmmtest(void);
static void cmd_ps(void);
static void cmd_kill(const char *args);
static void cmd_threads(void);
static void cmd_ls(void);
static void cmd_touch(const char *args);
static void cmd_cat(const char *args);
static void cmd_write(const char *args);
static void cmd_rm(const char *args);

static int k_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (uint8_t)*a - (uint8_t)*b;
}

static int k_strncmp(const char *a, const char *b, size_t n) {
    while (n-- && *a && (*a == *b)) { a++; b++; }
    return n == (size_t)-1 ? 0 : (uint8_t)*a - (uint8_t)*b;
}

static size_t k_strlen(const char *s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

static const char *k_ltrim(const char *s) {
    while (*s == ' ') s++;
    return s;
}

static int k_atoi(const char *s) {
    int result = 0;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result;
}

static void vga_puts_int(int n) {
    char buf[12];
    int i = 0;
    if (n == 0) { vga_puts("0"); return; }
    if (n < 0) { vga_puts("-"); n = -n; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) {
        char c[2] = { buf[--i], 0 };
        vga_puts(c);
    }
}

static void print_splash(void) {
    vga_clear(VGA_BLACK);
    vga_draw_box(0, 0, 7, 80, VGA_LIGHT_MAGENTA);

    vga_set_cursor(1, 2);
    vga_puts_color("   SENG21213-OS  |  Computer Architecture & Operating Systems",
                   VGA_YELLOW, VGA_BLACK);

    vga_set_cursor(2, 2);
    vga_puts_color("   Stage 4: RAM Disk File System", VGA_LIGHT_CYAN, VGA_BLACK);

    vga_set_cursor(3, 2);
    vga_puts_color("   Faculty of Engineering - Department of Software Engineering",
                   VGA_LIGHT_GREY, VGA_BLACK);

    vga_set_cursor(4, 2);
    vga_puts_color("   Type 'help' for commands, 'ls' to view files.",
                   VGA_LIGHT_GREEN, VGA_BLACK);

    vga_set_cursor(5, 2);
    vga_puts_color("   CPU: i686 (32-bit Protected Mode)  |  Display: VGA 80x25",
                   VGA_DARK_GREY, VGA_BLACK);

    vga_set_cursor(8, 0);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_puts("   Welcome to Stage 4! RAM Disk File System is active.\n\n");
}

static void cmd_help(void) {
    vga_puts_color("\n   SENG21213-OS Shell Commands\n", VGA_YELLOW, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    vga_puts("   help     - Show this help message\n");
    vga_puts("   clear    - Clear the screen\n");
    vga_puts("   about    - About this OS and course\n");
    vga_puts("   echo     - Echo text to screen\n");
    vga_puts("   mem      - Physical Memory Manager info\n");
    vga_puts("   meminfo  - Physical Memory Manager info (alias)\n");
    vga_puts("   pmmtest  - Run PMM allocation leak test\n");
    vga_puts("   ps       - List processes\n");
    vga_puts("   kill     - Terminate a process by PID\n");
    vga_puts("   threads  - Run Stage 2 Thread & Sync demos\n");
    vga_puts("   ls       - List files in RAM disk\n");
    vga_puts("   touch    - Create a new file (e.g. touch a.txt)\n");
    vga_puts("   cat      - Read file contents (e.g. cat a.txt)\n");
    vga_puts("   write    - Write text to file (e.g. write a.txt Hello)\n");
    vga_puts("   rm       - Remove/delete a file (e.g. rm a.txt)\n\n");
}

static void cmd_clear(void) {
    vga_clear(VGA_BLACK);
}

static void cmd_about(void) {
    vga_puts_color("\n   About SENG21213-OS (Stage 4)\n", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    vga_puts("   Architecture : x86 (i686), 32-bit Protected Mode\n");
    vga_puts("   Features     : Processes, Threads, PMM, RAM Disk File System\n");
    vga_puts("   Course       : SENG 21213 - Sem 2\n\n");
}

static void cmd_echo(const char *args) {
    vga_puts("   ");
    vga_puts(args);
    vga_puts("\n");
}

static void cmd_mem(void) {
    unsigned int total_bytes = pmm_get_total_memory();
    unsigned int free_bytes  = pmm_get_free_memory();

    vga_puts_color("\n  Physical Memory Manager\n", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("  -----------------------------------------------\n");
    vga_puts("  Total : "); vga_puts_int(total_bytes / 1024); vga_puts(" KB\n");
    vga_puts("  Free  : "); vga_puts_int(free_bytes / 1024); vga_puts(" KB\n\n");
}

static void cmd_pmmtest(void) {
    unsigned int before = pmm_get_free_memory();
    uint32_t page = pmm_alloc_page();
    pmm_free_page(page);
    unsigned int after = pmm_get_free_memory();

    vga_puts_color("\n  PMM Leak Test\n", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("  -----------------------------------------------\n");
    vga_puts("  Free before : "); vga_puts_int(before); vga_puts(" bytes\n");
    vga_puts("  Free after  : "); vga_puts_int(after); vga_puts(" bytes\n");

    if (before == after) {
        vga_puts_color("  PASS - no leaks detected\n\n", VGA_LIGHT_GREEN, VGA_BLACK);
    } else {
        vga_puts_color("  FAIL - frame leak detected\n\n", VGA_LIGHT_RED, VGA_BLACK);
    }
}

static void cmd_ps(void) {
    vga_puts_color("\n   PID   STATE       NAME\n", VGA_YELLOW, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROC_UNUSED) continue;
        vga_puts("   ");
        vga_puts_int(process_table[i].pid);
        vga_puts("    ");
        switch (process_table[i].state) {
            case PROC_READY:      vga_puts("READY      "); break;
            case PROC_RUNNING:    vga_puts("RUNNING    "); break;
            case PROC_TERMINATED: vga_puts("TERMINATED "); break;
            default: break;
        }
        vga_puts(" ");
        vga_puts(process_table[i].name);
        vga_puts("\n");
    }
    vga_puts("\n");
}

static void cmd_kill(const char *args) {
    int pid = k_atoi(args);
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid && process_table[i].state != PROC_UNUSED) {
            process_table[i].state = PROC_TERMINATED;
            vga_puts_color("   Process terminated.\n", VGA_LIGHT_GREEN, VGA_BLACK);
            return;
        }
    }
    vga_puts_color("   No such process.\n", VGA_LIGHT_RED, VGA_BLACK);
}

static volatile int shared_counter = 0;
static mutex_t counter_mutex;

static void racer_a(void *arg) {
    (void)arg;
    mutex_lock(&counter_mutex);
    for (int i = 0; i < 5000; i++) {
        shared_counter++;
    }
    mutex_unlock(&counter_mutex);
}

static void racer_b(void *arg) {
    (void)arg;
    mutex_lock(&counter_mutex);
    for (int i = 0; i < 5000; i++) {
        shared_counter++;
    }
    mutex_unlock(&counter_mutex);
}

#define BUFFER_SIZE 8
static int pc_buffer[BUFFER_SIZE];
static int pc_in = 0, pc_out = 0;
static semaphore_t sem_empty;
static semaphore_t sem_full;
static mutex_t pc_mutex;

static void producer_task(void *arg) {
    (void)arg;
    int item = 0;
    while (item < 10) {
        sem_wait(&sem_empty);
        mutex_lock(&pc_mutex);
        pc_buffer[pc_in] = item;
        pc_in = (pc_in + 1) % BUFFER_SIZE;
        item++;
        mutex_unlock(&pc_mutex);
        sem_signal(&sem_full);
    }
}

static void consumer_task(void *arg) {
    (void)arg;
    for (int i = 0; i < 10; i++) {
        sem_wait(&sem_full);
        mutex_lock(&pc_mutex);
        int val = pc_buffer[pc_out];
        pc_out = (pc_out + 1) % BUFFER_SIZE;
        (void)val;
        mutex_unlock(&pc_mutex);
        sem_signal(&sem_empty);
    }
}

static void cmd_threads(void) {
    vga_puts_color("\n   --- Race Condition Demo (No Mutex) ---\n", VGA_YELLOW, VGA_BLACK);
    shared_counter = 0;
    thread_init();
    thread_create(racer_a, 0, "racer_a");
    thread_create(racer_b, 0, "racer_b");
    vga_puts("   shared_counter value: ");
    vga_puts_int(shared_counter);
    vga_puts("\n\n");

    vga_puts_color("   --- Producer-Consumer Demo (Semaphores) ---\n", VGA_LIGHT_CYAN, VGA_BLACK);
    mutex_init(&pc_mutex);
    sem_init(&sem_empty, BUFFER_SIZE);
    sem_init(&sem_full, 0);
    pc_in = 0; pc_out = 0;
    
    thread_create(producer_task, 0, "producer");
    thread_create(consumer_task, 0, "consumer");
    vga_puts("   Producer and Consumer threads initialized successfully.\n\n");
}

// --- File System Command Handlers ---
static void cmd_ls(void) {
    vga_puts_color("\n   Filesystem contents:\n", VGA_YELLOW, VGA_BLACK);
    fs_ls(vga_puts);
    vga_puts("\n");
}

static void cmd_touch(const char *args) {
    if (k_strlen(args) == 0) {
        vga_puts_color("   Usage: touch <filename>\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }
    int fd = fs_open(args, FS_MODE_CREATE);
    if (fd >= 0) {
        fs_close(fd);
        vga_puts_color("   File created successfully.\n", VGA_LIGHT_GREEN, VGA_BLACK);
    } else {
        vga_puts_color("   Error creating file.\n", VGA_LIGHT_RED, VGA_BLACK);
    }
}

static void cmd_cat(const char *args) {
    if (k_strlen(args) == 0) {
        vga_puts_color("   Usage: cat <filename>\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }
    int fd = fs_open(args, 0);
    if (fd < 0) {
        vga_puts_color("   File not found.\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }
    char buf[4096];
    int bytes = fs_read(fd, buf, sizeof(buf) - 1);
    if (bytes >= 0) {
        buf[bytes] = '\0';
        vga_puts("\n   ");
        vga_puts(buf);
        vga_puts("\n\n");
    }
    fs_close(fd);
}

static void cmd_write(const char *args) {
    char name[28];
    int i = 0;
    while (args[i] && args[i] != ' ' && i < 27) {
        name[i] = args[i];
        i++;
    }
    name[i] = '\0';

    while (args[i] == ' ') i++;
    const char *text = &args[i];

    if (k_strlen(name) == 0 || k_strlen(text) == 0) {
        vga_puts_color("   Usage: write <filename> <text>\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }

    int fd = fs_open(name, FS_MODE_CREATE);
    if (fd < 0) {
        vga_puts_color("   Error opening file for write.\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }

    int written = fs_write(fd, text, k_strlen(text));
    fs_close(fd);

    if (written >= 0) {
        vga_puts_color("   Written successfully.\n", VGA_LIGHT_GREEN, VGA_BLACK);
    } else {
        vga_puts_color("   Write failed.\n", VGA_LIGHT_RED, VGA_BLACK);
    }
}

static void cmd_rm(const char *args) {
    if (k_strlen(args) == 0) {
        vga_puts_color("   Usage: rm <filename>\n", VGA_LIGHT_RED, VGA_BLACK);
        return;
    }
    if (fs_unlink(args) == 0) {
        vga_puts_color("   File deleted.\n", VGA_LIGHT_GREEN, VGA_BLACK);
    } else {
        vga_puts_color("   File not found or delete failed.\n", VGA_LIGHT_RED, VGA_BLACK);
    }
}

static char shell_buf[256];
static char prompt[] = "\n   ksh> ";

static void shell_run(void) {
    vga_puts_color("\n   Kernel Shell ready. Type 'help' for commands.\n",
                   VGA_LIGHT_GREEN, VGA_BLACK);

    while (true) {
        vga_puts_color(prompt, VGA_LIGHT_GREEN, VGA_BLACK);
        kb_readline(shell_buf, sizeof(shell_buf));

        const char *cmd = k_ltrim(shell_buf);
        if (k_strlen(cmd) == 0) continue;

        if (k_strcmp(cmd, "help")    == 0) { cmd_help();    continue; }
        if (k_strcmp(cmd, "clear")   == 0) { cmd_clear();   continue; }
        if (k_strcmp(cmd, "about")   == 0) { cmd_about();   continue; }
        if (k_strcmp(cmd, "mem")     == 0 || k_strcmp(cmd, "meminfo") == 0) { cmd_mem();     continue; }
        if (k_strcmp(cmd, "pmmtest") == 0) { cmd_pmmtest(); continue; }
        if (k_strcmp(cmd, "ps")      == 0) { cmd_ps();      continue; }
        if (k_strcmp(cmd, "threads") == 0) { cmd_threads(); continue; }
        if (k_strcmp(cmd, "ls")      == 0) { cmd_ls();      continue; }

        if (k_strncmp(cmd, "echo ", 5) == 0) {
            cmd_echo(k_ltrim(cmd + 5));
            continue;
        }

        if (k_strncmp(cmd, "kill ", 5) == 0) {
            cmd_kill(k_ltrim(cmd + 5));
            continue;
        }

        if (k_strncmp(cmd, "touch ", 6) == 0) {
            cmd_touch(k_ltrim(cmd + 6));
            continue;
        }

        if (k_strncmp(cmd, "cat ", 4) == 0) {
            cmd_cat(k_ltrim(cmd + 4));
            continue;
        }

        if (k_strncmp(cmd, "write ", 6) == 0) {
            cmd_write(k_ltrim(cmd + 6));
            continue;
        }

        if (k_strncmp(cmd, "rm ", 3) == 0) {
            cmd_rm(k_ltrim(cmd + 3));
            continue;
        }

        vga_puts_color("   Unknown command: ", VGA_LIGHT_RED, VGA_BLACK);
        vga_puts(cmd);
        vga_puts("\n   Type 'help' for a list of commands.\n");
    }
}

void kernel_main(void) {
    vga_init();
    kb_init();
    process_init();
    thread_init();
    pmm_init(0, 0);
    fs_init();

    print_splash();

    shell_run();

    __asm__ __volatile__("hlt");
}
