/*
 * io.c
 *
 *  Created on: 28 Aug 2017
 *      Author: jmoodliar
 */

#include <rte_io.h>
#include "F28x_Project.h"
#include <stdint.h>
#include "rte_timer.h"

static bool loss_of_osc = false;
//static uint32_t fan_timer;

//static long calc_fan_freq(struct ECAP_REGS volatile * const p_ecap);
static long fan_freq[2];

void io_init(void)
{
	fan_freq[0] = fan_freq[1] = 0;
//	fan_timer = timer_get_tick() + DELAY_SECOND(1);
}

void io_task(void)
{
//	if(timer_is_expired(fan_timer))
//	{
//		fan_timer += DELAY_SECOND(1);
//
//		fan_freq[0] = calc_fan_freq(&ECap1Regs);
//		fan_freq[1] = calc_fan_freq(&ECap2Regs);
//	}
}
//
//uint16_t io_get_node_id(void)
//{
//	uint16_t node_id;
//
//	node_id = GpioDataRegs.GPADAT.bit.GPIO27 << 3;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO19 << 2;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO13 << 1;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO28;
//	node_id = (~node_id) & 0xFU;
//
//	return node_id;
//}
//
//void io_self_discharge_relay_on(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO6 = 1U;
//}
//
//void io_self_discharge_relay_off(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO6 = 1U;
//}
//
//bool io_self_discharge_relay_get(void)
//{
//	bool bit = false;
//
//	if (!GpioDataRegs.GPADAT.bit.GPIO6)
//		bit = true;
//
//	return bit;
//}

//void io_discharge_contactor_1_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO9 = 1U;
//}
//
//void io_discharge_contactor_1_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1U;
//}
//
//bool io_discharge_contactor_1_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO9)
//		bit = true;
//
//	return bit;
//}
//
//void io_discharge_contactor_2_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO2 = 1U;
//}
//
//void io_discharge_contactor_2_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1U;
//}
//
//bool io_discharge_contactor_2_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO2)
//		bit = true;
//
//	return bit;
//}
//
//void io_charge_relay_1_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO4 = 1U;
//}
//
//void io_charge_relay_1_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1U;
//}
//
//bool io_charge_relay_1_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO4)
//		bit = true;
//
//	return bit;
//}
//
//void io_charge_relay_2_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO8 = 1U;
//}
//
//void io_charge_relay_2_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO8 = 1U;
//}
//
//bool io_charge_relay_2_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO4)
//		bit = true;
//
//	return bit;
//}

void io_fan_relay_on(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1U;
}

void io_fan_relay_off(void)
{
	GpioDataRegs.GPASET.bit.GPIO10 = 1U;
}

//void io_output_enable(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO1 = 1U;
//}
//
//void io_output_disable(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1U;
//}
//
//void io_charge_stop_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO0 = 1U;
//}
//
//void io_charge_stop_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1U;
//}
//
//void io_disable_act_bal_fault(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO5 = 1U;
//}
//
//void io_enable_act_bal_fault(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1U;
//}
//
//uint16_t io_get_cell_over_volt(void)
//{
//	uint16_t bits;
//
//	bits = GpioDataRegs.GPADAT.bit.GPIO22;
//	bits |= GpioDataRegs.GPADAT.bit.GPIO11 << 1U;
//	bits |= GpioDataRegs.GPADAT.bit.GPIO21 << 2U;
//	bits |= GpioDataRegs.GPADAT.bit.GPIO16 << 3U;
//	bits = (~bits) & 0xFU;
//
//	return bits;
//}


#ifdef 1
static long calc_fan_freq(struct ECAP_REGS volatile * const p_ecap)
{
	long freq;
	int32 tick_count;
	Uint16 samples;
	long period;

#define COUNT_FOR_1_SECOND		(0x4D00000UL)
	samples = 0;
	tick_count = 0;
	if (p_ecap->ECFLG.bit.CTROVF)
	{
		p_ecap->ECCLR.bit.CTROVF = 1;
		p_ecap->ECCTL1.bit.CAPLDEN = 0;
		p_ecap->ECCLR.bit.CEVT1 = 1;
		p_ecap->ECCLR.bit.CEVT2 = 1;
		p_ecap->ECCLR.bit.CEVT3 = 1;
		p_ecap->ECCLR.bit.CEVT4 = 1;
		p_ecap->ECCTL1.bit.CAPLDEN = 1;
		p_ecap->ECCTL2.bit.TSCTRSTOP = 1;
		p_ecap->ECCTL2.bit.REARM = 1;
		freq = 0;
	}
	else
	{
		if (p_ecap->ECFLG.bit.CEVT1)
		{
			if (p_ecap->CAP1 < COUNT_FOR_1_SECOND)
			{
				tick_count += (int32)p_ecap->CAP1;
				samples++;
			}
			p_ecap->ECCLR.bit.CEVT1 = 1;
		}
		if (p_ecap->ECFLG.bit.CEVT2)
		{
			if (p_ecap->CAP2 < COUNT_FOR_1_SECOND)
			{
				tick_count += (int32)p_ecap->CAP2;
				samples++;
			}
			p_ecap->ECCLR.bit.CEVT2 = 1;
		}
		if (p_ecap->ECFLG.bit.CEVT3)
		{
			if (p_ecap->CAP3 < COUNT_FOR_1_SECOND)
			{
				tick_count += (int32)p_ecap->CAP3;
				samples++;
			}
			p_ecap->ECCLR.bit.CEVT3 = 1;
		}
		if (p_ecap->ECFLG.bit.CEVT4)
		{
			if (p_ecap->CAP4 < COUNT_FOR_1_SECOND)
			{
				tick_count += (int32)p_ecap->CAP4;
				samples++;
			}
			p_ecap->ECCLR.bit.CEVT4 = 1;
		}

		if (samples > 0)
		{
	//		period = _IQ2div(_IQ2(tick_count), _IQ2(samples));
            period = ((tick_count)/(samples));

			period = period << 2;	// Convert to IQ4
			//period = _IQ4div(period, _IQ4(1000));
            period = (period/1000);

			period = period << 8;	// Convert to IQ12
		//	period = _IQ12div(period, _IQ12(1000));
		    period = (period/1000);
		//	freq = _IQ12div(_IQ12(80), period);
            freq = (80/period);

			p_ecap->ECCTL1.bit.CAPLDEN = 0;
			p_ecap->ECCLR.bit.CEVT1 = 1;
			p_ecap->ECCLR.bit.CEVT2 = 1;
			p_ecap->ECCLR.bit.CEVT3 = 1;
			p_ecap->ECCLR.bit.CEVT4 = 1;
			p_ecap->ECCTL1.bit.CAPLDEN = 1;
			p_ecap->ECCTL2.bit.TSCTRSTOP = 1;
			p_ecap->ECCTL2.bit.REARM = 1;
		}
		else
		{
			// At least 1 event should get captured in a 1 second interval to have > 1 Hz frequency
			freq = 0;
		}
	}

	return freq;
}
#endif


long io_get_fan_freq(uint16_t ch)
{
	if (ch < 2)
	{
		return fan_freq[ch];
	}

	return 0;
}

void enable_loss_of_osc(void)
{
	loss_of_osc = true;
}

void disable_loss_of_osc(void)
{
	loss_of_osc = false;
}

__attribute__((always_inline))
void loss_of_osc_toggle(void)
{
	if (loss_of_osc)
	{
		GpioDataRegs.GPATOGGLE.bit.GPIO17 = 1;
	}
}
