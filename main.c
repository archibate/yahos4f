#include <linux/conio.h>
#include <linux/kernel.h>
#include <linux/gdt.h>
#include <linux/idt.h>
#include <linux/tss.h>
#include <linux/pic.h>
#include <linux/pit.h>
#include <linux/mmu.h>
#include <linux/pmm.h>
#include <linux/mman.h>
#include <linux/sched.h>
#include <linux/cmos.h>
#include <linux/fs.h>
#include <stdio.h>

int root_dev = HDA_DEV;

int run_init(void)
{
	char *argv[] = {"/bin/init", "-s", NULL};
	char *envp[] = {"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
			"HOME=/root", NULL};
	return do_execve("/bin/init", argv, envp);
}

#if 0
void cmos_test(void)
{
	struct tm t;
	cmos_gettime(&t);
	printk("%02d/%02d/%02d %02d:%02d:%02d",
			t.tm_year, t.tm_mon, t.tm_mday,
			t.tm_hour, t.tm_min, t.tm_sec);
}
#endif

void main(void)
{
	clear();
	cputs("Kernel Started...\n");
	init_gdt();
	init_idt();
	init_pic();
	init_tss();
	set_timer_freq(10);
	init_pmm();
	init_mmu();
	init_mman();
	init_sched();
	init_buffer();

	irq_setenable(0, 1);
	irq_setenable(1, 1);

	read_super(ROOT_DEV);
	init_fs();

	setup_kernel_task(new_task(current), run_init, NULL)->priority = 2;

	for (;;)
		asm volatile ("sti\nhlt");
	panic("regedit.exe /editorCommand \"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity\\Enabled : REG_DWORD = 0x00000000 (0)\" /lpszLicenceEnablingFilePath \"Microsoft Visual Studio (C)\\PAGE_FAULT_HANDLER_GENERATOR\\Version 0.1 (x86)\\Generated Files\\Software\\Control\\Windows XP Recoverer\\ControlSet001\\KernelHypervisorEnabler\\Common\\Bin\\Documentation Files\\User Guide.pdf\"");
}
