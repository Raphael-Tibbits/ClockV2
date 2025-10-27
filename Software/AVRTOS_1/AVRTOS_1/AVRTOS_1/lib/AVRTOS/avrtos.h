/**
 * @file avrtos.h
 * @brief Minimal cooperative/preemptive RTOS for AVR microcontrollers.
 *
 * Provides task creation, cooperative delay, and context switching using
 * inline assembly. Designed for use with a timer ISR to trigger scheduling.
 */

#ifndef AVRTOS_H
#define AVRTOS_H

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/// Maximum number of tasks the RTOS can manage
#define MAX_TASKS 16

/// Stack size for each task in bytes
#define STACK_SIZE 64

/// Function pointer type for tasks
typedef void (*task_f_type)(void);

/**
 * @brief Task Control Block (TCB) structure
 */
typedef struct {
    task_f_type function;            ///< Task function pointer
    uint16_t ticks_till_unblocked;   ///< Delay counter, 0 = ready
    uint8_t *stackPtr;               ///< Saved stack pointer for context switching
} AVRTOS_TCB;

/**
 * @brief start the RTOS scheduler
 * This function does not return.
 * It finds the first ready task, restores its context, and enables interrupts.
 *
 * @note Must be called after creating tasks and setting up the timer ISR.
 * 
 */
extern void AVRTOS_start(void);
/**
 * @brief Create a new task.
 *
 * Initializes the task's stack and TCB for context switching.
 *
 * @param taskHandle   Pointer to the task function
 * @param taskPriority Task priority (0 = highest, MAX_TASKS-1 = lowest)
 */
extern void AVRTOS_create_task(task_f_type taskHandle, uint8_t taskPriority);

/**
 * @brief Put the current task into a blocked state for a number of ticks.
 *
 * Saves the CPU context, stores the current SP in the TCB, and switches
 * to the next ready task. The blocked task will resume automatically
 * after the specified delay and all higher priority tasks in the resuming tick.
 *
 * @param ticks Number of scheduler ticks to delay
 */
extern void AVRTOS_delay(uint16_t ticks);

/**
 * @brief Main task scheduler.
 *
 * Iterates through the TCB array starting from the specified priority,
 * selects the first ready task, restores its context, and switches to it.
 * 
 * call from anywhere but ISR
 * If no tasks are ready, the function returns.
 *  
 * @param next_prio Priority index to start the scheduling scan from
 */
extern void AVRTOS_sequencer(uint8_t next_prio);

/**
 * @brief Main task scheduler.
 *
 * Iterates through the TCB array starting from the specified priority,
 * selects the first ready task, restores its context, and switches to it.
 *
 * call from ISR for tick sequencing
 * If no tasks are ready, the function returns.
 *  
 * @param next_prio Priority index to start the scheduling scan from
 */
extern void AVRTOS_sequencer_ISR();

/**
 * @brief Default idle task that puts the CPU to sleep in idle mode.
 *
 * CPU halts until the next interrupt, typically a timer tick.
 * Recommended as an idle task or run after the sequencer in ISR
 */
extern void idle_sleep_task(void);

#endif // AVRTOS_H