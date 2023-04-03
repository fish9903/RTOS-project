#include "stdint.h"
#include "memio.h"
#include "Interrupt.h"
#include "HalInterrupt.h"
#include "armcpu.h"

#ifndef STDDEF_H_
#define STDDEF_H_
#include <stddef.h> // inclue NULL
#endif

extern volatile GicCput_t* GicCpu;
extern volatile GicDist_t* GicDist;

static InterHdlr_fptr sHandlers[INTERRUPT_HANDLER_NUM]; // can stores 255 interrupt handlers

void Hal_interrupt_init(void)
{
    GicCpu->cpucontrol.bits.Enable = 1;
    GicCpu->prioritymask.bits.Prioritymask = GIC_PRIORITY_MASK_NONE; // allow interrupt(0x0~0xF priority)
    GicDist->distributorctrl.bits.Enable = 1;

    for (uint32_t i = 0 ; i < INTERRUPT_HANDLER_NUM ; i++)
    {
        sHandlers[i] = NULL;
    }

    enable_irq(); // control cspr to allow IRQ
}

void Hal_interrupt_enable(uint32_t interrupt_num)
{
    // GIC_IRQ_START = 32, GIC_IRQ_END = 95
    if ((interrupt_num < GIC_IRQ_START) || (GIC_IRQ_END < interrupt_num))
    {
        return;
    }

    uint32_t bit_num = interrupt_num - GIC_IRQ_START; // get offset

    // setenable1, 2 register each has 32 interrupts
    // setenable1: interrupt_num 32 ~ 63
    // setenable2: interrupt_unm 64 ~ 95
    if (bit_num < GIC_IRQ_START)
    {
        SET_BIT(GicDist->setenable1, bit_num);
    }
    else
    {
        bit_num -= GIC_IRQ_START;
        SET_BIT(GicDist->setenable2, bit_num);
    }
}

void Hal_interrupt_disable(uint32_t interrupt_num)
{
    if ((interrupt_num < GIC_IRQ_START) || (GIC_IRQ_END < interrupt_num))
    {
        return;
    }

    uint32_t bit_num = interrupt_num - GIC_IRQ_START;

    if (bit_num < GIC_IRQ_START)
    {
        CLR_BIT(GicDist->setenable1, bit_num);
    }
    else
    {
        bit_num -= GIC_IRQ_START;
        CLR_BIT(GicDist->setenable2, bit_num);
    }
}

void Hal_interrupt_register_handler(InterHdlr_fptr handler, uint32_t interrupt_num)
{
    // store interrupt handler with interrupt number
    sHandlers[interrupt_num] = handler;
}

void Hal_interrupt_run_handler(void)
{
    uint32_t interrupt_num = GicCpu->interruptack.bits.InterruptID;

    if (sHandlers[interrupt_num] != NULL)
    {
        sHandlers[interrupt_num]();
    }

    GicCpu->endofinterrupt.bits.InterruptID = interrupt_num; // End of interrupt
}
