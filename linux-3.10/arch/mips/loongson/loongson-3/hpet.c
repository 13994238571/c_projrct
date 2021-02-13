#include <linux/init.h>
#include <linux/pci.h>
#include <linux/percpu.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <asm/hpet.h>
#include <asm/time.h>
#include <loongson-pch.h>
#include <loongson.h>

#define SMBUS_CFG_BASE		(ht_control_base + 0x0300a000)
#define SMBUS_PCI_REGB4		0xb4
#define SMBUS_PCI_REG40		0x40
#define SMBUS_PCI_REG64		0x64

#define HPET_MIN_CYCLES		16
#define HPET_MIN_PROG_DELTA	(HPET_MIN_CYCLES * 12)

extern int hpet_enabled;
static DEFINE_SPINLOCK(hpet_lock);
DEFINE_PER_CPU(struct clock_event_device, hpet_clockevent_device);
unsigned long long hpet_mmio_addr;
unsigned int hpet_freq;
unsigned int hpet_t0_irq;

static unsigned int smbus_read(int offset)
{
	return *(volatile unsigned int *)(SMBUS_CFG_BASE + offset);
}

static void smbus_write(int offset, int data)
{
	*(volatile unsigned int *)(SMBUS_CFG_BASE + offset) = data;
}

static void smbus_enable(int offset, int bit)
{
	unsigned int cfg = smbus_read(offset);

	cfg |= bit;
	smbus_write(offset, cfg);
}

static int hpet_read(int offset)
{
	return *(volatile unsigned int *)(HPET_MMIO_ADDR + offset);
}

static void hpet_write(int offset, int data)
{
	*(volatile unsigned int *)(HPET_MMIO_ADDR + offset) = data;
}

static void hpet_start_counter(void)
{
	unsigned int cfg = hpet_read(HPET_CFG);

	cfg |= HPET_CFG_ENABLE;
	hpet_write(HPET_CFG, cfg);
}

static void hpet_stop_counter(void)
{
	unsigned int cfg = hpet_read(HPET_CFG);

	cfg &= ~HPET_CFG_ENABLE;
	hpet_write(HPET_CFG, cfg);
}

static void hpet_reset_counter(void)
{
	hpet_write(HPET_COUNTER, 0);
	hpet_write(HPET_COUNTER + 4, 0);
}

static void hpet_restart_counter(void)
{
	hpet_stop_counter();
	hpet_reset_counter();
	hpet_start_counter();
}

static void hpet_enable_legacy_int(void)
{
	unsigned int cfg = hpet_read(HPET_CFG);

	cfg |= HPET_CFG_LEGACY;
#if 0
	/* Workaround: RTC interrupt may be lost if enable legacy int */
	hpet_write(HPET_CFG, cfg);
#endif
}

void clean_comparator_value(void)
{
	int cfg;
	cfg = hpet_read(HPET_T0_CMP);
	cfg &= 0;
	hpet_write(HPET_T0_CMP,cfg);
}
void clean_timer_config(void)
{
	int cfg;
	cfg = hpet_read(HPET_T0_CFG);
	cfg &= 0;
	hpet_write(HPET_T0_CFG,cfg);
}

void soft_reset(void)
{
	hpet_stop_counter();
	hpet_reset_counter();
	clean_timer_config();
	clean_comparator_value();
}

static void hpet_set_mode(enum clock_event_mode mode,
				struct clock_event_device *evt)
{
	int cfg = 0;

	spin_lock(&hpet_lock);
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		printk(KERN_INFO "set clock event to periodic mode!\n");
		if (loongson_pch && loongson_pch->board_type == RS780E) {
			/* stop counter */
			hpet_stop_counter();

			/* enables the timer0 to generate a periodic interrupt */
			cfg = hpet_read(HPET_T0_CFG);
			cfg &= ~HPET_TN_LEVEL;
			cfg |= HPET_TN_ENABLE | HPET_TN_PERIODIC |
					HPET_TN_SETVAL | HPET_TN_32BIT;
			hpet_write(HPET_T0_CFG, cfg);

			/* set the comparator */
			hpet_write(HPET_T0_CMP, HPET_COMPARE_VAL);
			udelay(1);
			hpet_write(HPET_T0_CMP, HPET_COMPARE_VAL);

			/* start counter */
			hpet_start_counter();
		} else {
			soft_reset();
			cfg = hpet_read(HPET_T0_CFG);
			cfg |= 0x48;
			hpet_write(HPET_T0_CFG,cfg);
			hpet_write(HPET_T0_CMP,HPET_COMPARE_VAL);
			cfg = hpet_read(HPET_T0_CFG);
			cfg |= 0xe;
			hpet_write(HPET_T0_CFG,cfg);
			hpet_start_counter();
		}
		break;
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_UNUSED:
		cfg = hpet_read(HPET_T0_CFG);
		cfg &= ~HPET_TN_ENABLE;
		hpet_write(HPET_T0_CFG, cfg);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		printk(KERN_INFO "set clock event to one shot mode!\n");
		cfg = hpet_read(HPET_T0_CFG);
		/* set timer0 type
		 * 1 : periodic interrupt
		 * 0 : non-periodic(oneshot) interrupt
		 */
		cfg &= ~HPET_TN_PERIODIC;
		cfg |= HPET_TN_ENABLE | HPET_TN_32BIT;
		hpet_write(HPET_T0_CFG, cfg);
		break;
	case CLOCK_EVT_MODE_RESUME:
		hpet_enable_legacy_int();
		break;
	}
	spin_unlock(&hpet_lock);
}

static int hpet_next_event(unsigned long delta,
		struct clock_event_device *evt)
{
	u32 cnt;
	s32 res;

	cnt = hpet_read(HPET_COUNTER);
	cnt += (u32) delta;
	hpet_write(HPET_T0_CMP, cnt);

	res = (s32)(cnt - hpet_read(HPET_COUNTER));

	return res < HPET_MIN_CYCLES ? -ETIME : 0;
}

static irqreturn_t hpet_irq_handler(int irq, void *data)
{
	int is_irq;
	struct clock_event_device *cd;
	unsigned int cpu = smp_processor_id();

	is_irq = hpet_read(HPET_STATUS);
	if (is_irq & HPET_T0_IRS) {
		/* clear the TIMER0 irq status register */
		hpet_write(HPET_STATUS, HPET_T0_IRS);
		cd = &per_cpu(hpet_clockevent_device, cpu);
		cd->event_handler(cd);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static struct irqaction hpet_irq = {
	.handler = hpet_irq_handler,
	.flags = IRQF_DISABLED | IRQF_NOBALANCING | IRQF_TIMER,
	.name = "hpet",
};

/*
 * hpet address assignation and irq setting should be done in bios.
 * but pmon don't do this, we just setup here directly.
 * The operation under is normal. unfortunately, hpet_setup process
 * is before pci initialize.
 *
 * {
 *	struct pci_dev *pdev;
 *
 *	pdev = pci_get_device(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SBX00_SMBUS, NULL);
 *	pci_write_config_word(pdev, SMBUS_PCI_REGB4, HPET_ADDR);
 *
 *	...
 * }
 */
static void hpet_setup(void)
{
	/* set hpet base address */
	smbus_write(SMBUS_PCI_REGB4, HPET_ADDR);

	/* enable decodeing of access to HPET MMIO*/
	smbus_enable(SMBUS_PCI_REG40, (1 << 28));

	/* HPET irq enable */
	smbus_enable(SMBUS_PCI_REG64, (1 << 10));

	hpet_enable_legacy_int();
}

void __init setup_hpet_timer(void)
{
	unsigned int cpu = smp_processor_id();
	struct clock_event_device *cd;

#ifndef CONFIG_RS780_HPET
	if (loongson_pch && loongson_pch->board_type == RS780E)
		return;
#endif
#ifndef CONFIG_LS7A_HPET
	if (loongson_pch && loongson_pch->board_type == LS7A)
		return;
#endif

	if (loongson_pch && loongson_pch->board_type != LS7A && loongson_pch->board_type != RS780E)
		return;

	hpet_mmio_addr = (loongson_pch && loongson_pch->board_type == RS780E) ? RS780_MMIO_ADDR : LS7A_MMIO_ADDR;
	hpet_freq = (loongson_pch && loongson_pch->board_type == RS780E) ? RS780_FREQ : LS7A_FREQ;
	hpet_t0_irq = (loongson_pch && loongson_pch->board_type == RS780E) ? RS780_HPET_IRQ : LS7A_IOAPIC_HPET_INT_IRQ;

	if (loongson_pch && loongson_pch->board_type == RS780E)
		hpet_setup();

	hpet_enabled = 1;

	cd = &per_cpu(hpet_clockevent_device, cpu);
	cd->name = "hpet";
	cd->rating = 100;
	cd->features = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT;
	cd->set_mode = hpet_set_mode;
	cd->set_next_event = hpet_next_event;
	cd->irq = HPET_T0_IRQ;
	cd->cpumask = cpumask_of(cpu);
	clockevent_set_clock(cd, HPET_FREQ);
	cd->max_delta_ns = clockevent_delta2ns(0x7fffffff, cd);
	cd->min_delta_ns = clockevent_delta2ns(HPET_MIN_PROG_DELTA, cd);

	clockevents_register_device(cd);
	setup_irq(HPET_T0_IRQ, &hpet_irq);
	printk(KERN_INFO "hpet clock event device register\n");
}

static cycle_t hpet_read_counter(struct clocksource *cs)
{
	return (cycle_t)hpet_read(HPET_COUNTER);
}

static void hpet_suspend(struct clocksource *cs)
{
}

static void hpet_resume(struct clocksource *cs)
{
	if (loongson_pch && loongson_pch->board_type == RS780E)
		hpet_setup();
	hpet_restart_counter();
}

struct clocksource csrc_hpet = {
	.name = "hpet",
	/* mips clocksource rating is less than 300, so hpet is batter. */
	.rating = 300,
	.read = hpet_read_counter,
	.mask = CLOCKSOURCE_MASK(32),
	/* oneshot mode work normal with this flag */
	.flags = CLOCK_SOURCE_IS_CONTINUOUS,
	.suspend = hpet_suspend,
	.resume = hpet_resume,
	.mult = 0,
	.shift = 10,
};

int __init init_hpet_clocksource(void)
{
	if(cpu_guestmode)
		return 0;

	if (!hpet_enabled)
		return -EBUSY;

	csrc_hpet.mult = clocksource_hz2mult(HPET_FREQ, csrc_hpet.shift);
	return clocksource_register_hz(&csrc_hpet, HPET_FREQ);
}

arch_initcall(init_hpet_clocksource);
