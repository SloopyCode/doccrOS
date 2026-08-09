#include <kernel/arch/x86_64/cpu.h>
#include <kernel/arch/hal/irqflags.h>

// Disable interrupts
void cli(void) {
    __asm__ volatile("cli");
}

// Enable interrupts
void sti(void) {
    __asm__ volatile("sti");
}

irq_state_t irq_save(void)
{
    u64 flags;

    __asm__ volatile("pushfq; pop %0; cli" : "=r"(flags) :: "memory");

    return (irq_state_t)flags;
}

void irq_restore(irq_state_t state)
{
    __asm__ volatile("push %0; popfq" :: "r"((u64)state) : "memory", "cc");

}

// Full system halt
__attribute__((noreturn)) void chalt(void) {
    __asm__ volatile("cli");
    for (;;) __asm__ volatile("hlt");
}

void halt(void) {
    __asm__ volatile("hlt");
}

// Idle halt
__attribute__((noreturn)) void idle(void) {
    for (;;) __asm__ volatile("hlt");
}

// Wait for interrupt
void wfi(void) {
    __asm__ volatile("sti; pause; hlt");
}

void nop(void) {
	__asm__ volatile("nop");
}
