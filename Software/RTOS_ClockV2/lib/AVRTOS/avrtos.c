#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <avrtos.h>

// --- Context save/restore macros ---
#define SAVE_CONTEXT() \
    asm volatile ( \
        "push r0           \n\t" \
        "in   r0, __SREG__ \n\t" \
        "cli               \n\t" \
        "push r0           \n\t" \
        "push r1           \n\t" \
        "clr  r1           \n\t" \
        "push r2           \n\t" \
        "push r3           \n\t" \
        "push r4           \n\t" \
        "push r5           \n\t" \
        "push r6           \n\t" \
        "push r7           \n\t" \
        "push r8           \n\t" \
        "push r9           \n\t" \
        "push r10          \n\t" \
        "push r11          \n\t" \
        "push r12          \n\t" \
        "push r13          \n\t" \
        "push r14          \n\t" \
        "push r15          \n\t" \
        "push r16          \n\t" \
        "push r17          \n\t" \
        "push r18          \n\t" \
        "push r19          \n\t" \
        "push r20          \n\t" \
        "push r21          \n\t" \
        "push r22          \n\t" \
        "push r23          \n\t" \
        "push r24          \n\t" \
        "push r25          \n\t" \
        "push r26          \n\t" \
        "push r27          \n\t" \
        "push r28          \n\t" \
        "push r29          \n\t" \
        "push r30          \n\t" \
        "push r31          \n\t" \
    )

#define RESTORE_CONTEXT() \
    asm volatile ( \
        "pop r31           \n\t" \
        "pop r30           \n\t" \
        "pop r29           \n\t" \
        "pop r28           \n\t" \
        "pop r27           \n\t" \
        "pop r26           \n\t" \
        "pop r25           \n\t" \
        "pop r24           \n\t" \
        "pop r23           \n\t" \
        "pop r22           \n\t" \
        "pop r21           \n\t" \
        "pop r20           \n\t" \
        "pop r19           \n\t" \
        "pop r18           \n\t" \
        "pop r17           \n\t" \
        "pop r16           \n\t" \
        "pop r15           \n\t" \
        "pop r14           \n\t" \
        "pop r13           \n\t" \
        "pop r12           \n\t" \
        "pop r11           \n\t" \
        "pop r10           \n\t" \
        "pop r9            \n\t" \
        "pop r8            \n\t" \
        "pop r7            \n\t" \
        "pop r6            \n\t" \
        "pop r5            \n\t" \
        "pop r4            \n\t" \
        "pop r3            \n\t" \
        "pop r2            \n\t" \
        "pop r1            \n\t" \
        "pop r0            \n\t" \
        "out __SREG__, r0  \n\t" \
        "pop r0            \n\t" \
    )

#define RESTORE_CONTEXT_ISR() \
    asm volatile ( \
        "pop r31           \n\t" \
        "pop r30           \n\t" \
        "pop r29           \n\t" \
        "pop r28           \n\t" \
        "pop r27           \n\t" \
        "pop r26           \n\t" \
        "pop r25           \n\t" \
        "pop r24           \n\t" \
        "pop r23           \n\t" \
        "pop r22           \n\t" \
        "pop r21           \n\t" \
        "pop r20           \n\t" \
        "pop r19           \n\t" \
        "pop r18           \n\t" \
        "pop r17           \n\t" \
        "pop r16           \n\t" \
        "pop r15           \n\t" \
        "pop r14           \n\t" \
        "pop r13           \n\t" \
        "pop r12           \n\t" \
        "pop r11           \n\t" \
        "pop r10           \n\t" \
        "pop r9            \n\t" \
        "pop r8            \n\t" \
        "pop r7            \n\t" \
        "pop r6            \n\t" \
        "pop r5            \n\t" \
        "pop r4            \n\t" \
        "pop r3            \n\t" \
        "pop r2            \n\t" \
        "pop r1            \n\t" \
        "pop r0            \n\t" \
        "out __SREG__, r0  \n\t" \
        "pop r0            \n\t" \
        "reti              \n\t" \
    )

// --- Global task stacks and TCBs ---
static uint8_t taskStacks[MAX_TASKS][STACK_SIZE];
static AVRTOS_TCB taskList[MAX_TASKS];
static uint8_t currentTask = 0;

// --- Task creation: initialize stack for context switching ---
void AVRTOS_create_task(task_f_type taskHandle, uint8_t taskPriority) {
    if (taskPriority >= MAX_TASKS || taskHandle == 0) return;
    taskList[taskPriority].function = taskHandle;
    taskList[taskPriority].ticks_till_unblocked = 0;

    // Initialize stack pointer for the task
    // Stack grows down, so start at top of stack
    uint8_t *sp = &taskStacks[taskPriority][STACK_SIZE - 1];

    // Simulate stack frame as if the task was interrupted 
    // Push return address (task entry point)
    uint16_t addr = (uint16_t)taskHandle;
    *(sp--) = (uint8_t)(addr & 0xFF); // low byte
    *(sp--) = (uint8_t)((addr >> 8) & 0xFF); // high byte

    // Save initial context (all registers = 0)
    for (uint8_t i = 0; i < 32; i++) *(sp--) = 0x00;
    *(sp--) = 0x80; // SREG
    taskList[taskPriority].stackPtr = sp;
}

// --- Start the RTOS: switch to first ready task ---
void AVRTOS_start(void) {
    // Find first ready task
    uint8_t nextTask = 0xFF;
    for (uint8_t prio = 0; prio < MAX_TASKS; prio++) {
        if (taskList[prio].function == 0) continue;
        if (taskList[prio].ticks_till_unblocked == 0) {
            nextTask = prio;
            break;
        }
    }
    if (nextTask == 0xFF) nextTask = 0; // idle if none ready

    currentTask = nextTask;
    uint16_t newSp = (uint16_t)taskList[currentTask].stackPtr;
    asm volatile(
        "out __SP_L__, %A0 \n\t"
        "out __SP_H__, %B0"
        :
        : "r"(newSp)
    );
    
    RESTORE_CONTEXT();
    sei();
}

// --- Idle task ---
void idle_sleep_task(void){
    while(1);
    // set_sleep_mode(SLEEP_MODE_IDLE);
    // cli();
    // sleep_enable();
    // sei();
    // sleep_cpu();
    // sleep_disable();
}



// --- Central scheduler: finds next ready task and switches context ---
void AVRTOS_sequencer(uint8_t next_prio) {
    // Save current stack pointer
    uint16_t sp;
    asm volatile(
        "in %A0, __SP_L__ \n\t"
        "in %B0, __SP_H__"
        : "=r"(sp)
    );
    taskList[currentTask].stackPtr = (uint8_t*)sp;

    // Find next ready task
    uint8_t nextTask = 0xFF;
    for (uint8_t prio = next_prio; prio < MAX_TASKS; prio++) {
        if (taskList[prio].function == 0) continue;
        if (taskList[prio].ticks_till_unblocked == 0) {
            nextTask = prio;
            break;
        }
    }
    if (nextTask == 0xFF) nextTask = 0; // idle if none ready

    currentTask = nextTask;
    uint16_t newSp = (uint16_t)taskList[currentTask].stackPtr;
    asm volatile(
        "out __SP_L__, %A0 \n\t"
        "out __SP_H__, %B0"
        :
        : "r"(newSp)
    );
}

// --- Preemptive scheduler called from timer ISR ---
void AVRTOS_sequencer_ISR() {
    SAVE_CONTEXT();
    uint8_t next_prio = 0; // start search from highest priority
    for (uint8_t prio = 0; prio < MAX_TASKS; prio++) {
        if (taskList[prio].ticks_till_unblocked > 0)
            taskList[prio].ticks_till_unblocked--;
        if (taskList[prio].ticks_till_unblocked == 0) {
            next_prio = prio;
            break;
        }
    }
    AVRTOS_sequencer(next_prio);
    RESTORE_CONTEXT_ISR();
}

// --- OS delay: block current task for N ticks ---
void AVRTOS_delay(uint16_t ticks){
    cli();
    taskList[currentTask].ticks_till_unblocked = ticks;
    sei();
    // Yield to scheduler (force context switch)
    SAVE_CONTEXT();
    AVRTOS_sequencer(0);
    RESTORE_CONTEXT();
}

// --- Tick handler: call from timer ISR in your main code ---
// ISR(TIMER1_COMPA_vect) {
//     for (uint8_t prio = 0; prio < MAX_TASKS; prio++) {
//         if (taskList[prio].ticks_till_unblocked > 0)
//             taskList[prio].ticks_till_unblocked--;
//     }
//     AVRTOS_sequencer_ISR(0);
// }