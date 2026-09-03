#include "vga.h"
#include "keyboard.h"
#include "process.h"
#include "../include/types.h"

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_about(void);
static void cmd_echo(const char *args);
static void cmd_mem(void);
static void cmd_ps(void);
static void cmd_kill(const char *args);

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
    vga_puts_color("   Stage 1: Process Management", VGA_LIGHT_CYAN, VGA_BLACK);

    vga_set_cursor(3, 2);
    vga_puts_color("   Faculty of Engineering - Department of Software Engineering",
                   VGA_LIGHT_GREY, VGA_BLACK);

    vga_set_cursor(4, 2);
    vga_puts_color("   Built by students, for students.  Type 'help' to begin.",
                   VGA_LIGHT_GREEN, VGA_BLACK);

    vga_set_cursor(5, 2);
    vga_puts_color("   CPU: i686 (32-bit Protected Mode)  |  Display: VGA 80x25",
                   VGA_DARK_GREY, VGA_BLACK);

    vga_set_cursor(8, 0);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_puts("   Welcome! This kernel was compiled from source and booted entirely\n");
    vga_puts("   from bare metal. There is no Linux or Windows underneath - only\n");
    vga_puts("   the code you and your team write.\n\n");
}

static void cmd_help(void) {
    vga_puts_color("\n   SENG21213-OS Shell Commands\n", VGA_YELLOW, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    vga_puts("   help    - Show this help message\n");
    vga_puts("   clear   - Clear the screen\n");
    vga_puts("   about   - About this OS and course\n");
    vga_puts("   echo    - Echo text to screen\n");
    vga_puts("   mem     - Memory map (stub)\n");
    vga_puts("   ps      - List processes\n");
    vga_puts("   kill    - Terminate a process by PID\n\n");
}

static void cmd_clear(void) {
    vga_clear(VGA_BLACK);
}

static void cmd_about(void) {
    vga_puts_color("\n   About SENG21213-OS\n", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    vga_puts("   Architecture : x86 (i686), 32-bit Protected Mode\n");
    vga_puts("   Bootloader   : Custom MBR (NASM)\n");
    vga_puts("   Kernel       : Freestanding C (GCC, no libc)\n");
    vga_puts("   VM Target    : QEMU (qemu-system-i386)\n");
    vga_puts("   Course       : SENG 21213 - Sem 2\n\n");
}

static void cmd_echo(const char *args) {
    vga_puts("   ");
    vga_puts(args);
    vga_puts("\n");
}

static void cmd_mem(void) {
    vga_puts_color("\n   Memory Map (stub - implement PMM in Lecture 11)\n",
                   VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("   -----------------------------------------------\n");
    vga_puts("   0x00000000 - 0x000FFFFF  :  First 1 MB (reserved/BIOS)\n");
    vga_puts("   0x00100000 - 0x00EFFFFF  :  Extended memory (usable ~14 MB)\n");
    vga_puts("   0xB8000    - 0xBFFFF     :  VGA frame buffer\n\n");
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

        if (k_strcmp(cmd, "help")  == 0) { cmd_help();  continue; }
        if (k_strcmp(cmd, "clear") == 0) { cmd_clear(); continue; }
        if (k_strcmp(cmd, "about") == 0) { cmd_about(); continue; }
        if (k_strcmp(cmd, "mem")   == 0) { cmd_mem();   continue; }
        if (k_strcmp(cmd, "ps")    == 0) { cmd_ps();    continue; }

        if (k_strncmp(cmd, "echo ", 5) == 0) {
            cmd_echo(k_ltrim(cmd + 5));
            continue;
        }

        if (k_strncmp(cmd, "kill ", 5) == 0) {
            cmd_kill(k_ltrim(cmd + 5));
            continue;
        }

        if (k_strcmp(cmd, "threads") == 0 ||
            k_strcmp(cmd, "free")    == 0 ||
            k_strcmp(cmd, "ls")      == 0 ||
            k_strcmp(cmd, "cat")     == 0) {
            vga_puts_color("   [TODO] This command is not yet implemented.\n",
                           VGA_YELLOW, VGA_BLACK);
            vga_puts("   Implement it as part of your lecture assignment.\n");
            continue;
        }

        vga_puts_color("   Unknown command: ", VGA_LIGHT_RED, VGA_BLACK);
        vga_puts(cmd);
        vga_puts("\n   Type 'help' for a list of commands.\n");
    }
}

static void process_a_task(void) {
    while (true) {
        vga_puts_color("A", VGA_LIGHT_CYAN, VGA_BLACK);
        for (volatile int i = 0; i < 1000000; i++);
    }
}

static void process_b_task(void) {
    while (true) {
        vga_puts_color("B", VGA_LIGHT_MAGENTA, VGA_BLACK);
        for (volatile int i = 0; i < 3000000; i++);
    }
}

void kernel_main(void) {
    vga_init();
    kb_init();
    process_init();

    print_splash();

    create_process(process_a_task, "proc_a");
    create_process(process_b_task, "proc_b");

    shell_run();

    __asm__ __volatile__("hlt");
}
