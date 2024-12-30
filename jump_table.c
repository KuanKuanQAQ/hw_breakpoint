#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kuan");
MODULE_DESCRIPTION("A kernel module with a switch-case jump table");

static int process_code(int code)
{
    int result = 0;

    switch (code) {
    case 0:
        result = 10;
        break;
    case 1:
        result = 20;
        break;
    case 2:
        result = 30;
        break;
    case 3:
        result = 40;
        break;
    case 4:
        result = 50;
        break;
    default:
        result = -1;
        break;
    }

    pr_info("Code: %d, Result: %d\n", code, result);
    return result;
}

/*search symbol addr*/
unsigned long kaddr_lookup_name(const char *fname_raw)
{
	int i;
	unsigned long kaddr;
	char *fname_lookup, *fname;

	fname_lookup = kzalloc(NAME_MAX, GFP_KERNEL);
	if (!fname_lookup)
		return 0;

	fname = kzalloc(strlen(fname_raw) + 4, GFP_KERNEL);
	if (!fname)
		return 0;

	/*
   * We have to add "+0x0" to the end of our function name
   * because that's the format that sprint_symbol() returns
   * to us. If we don't do this, then our search can stop
   * prematurely and give us the wrong function address!
   */
	strcpy(fname, fname_raw);
	strcat(fname, "+0x0");

	/*获取内核代码段基地址*/
	kaddr = (unsigned long)&sprint_symbol;
	kaddr &= 0xffffffffff000000;

	/*内核符号不会超过0x100000*16的大小，所以按4字节偏移，挨个找*/
	for (i = 0x0; i < 0x400000; i++) {
		/*寻找地址对应的符号名称*/
		sprint_symbol(fname_lookup, kaddr);
		/*对比寻找的符号名字*/
		if (strncmp(fname_lookup, fname, strlen(fname)) == 0) {
			/*找到了就返回地址*/
			kfree(fname_lookup);
			kfree(fname);
			return kaddr;
		}
		/*偏移4字节*/
		kaddr += 0x04;
	}
	/*没找到地址就返回0*/
	kfree(fname_lookup);
	kfree(fname);
	return 0;
}

static int __init jump_table_module_init(void)
{
    unsigned long (*kallsyms_lookup_name)(const char *name) = 
        (void *)kaddr_lookup_name("kallsyms_lookup_name");

    char *start = kallsyms_lookup_name("_stext");   // 内核代码段的起始地址
    char *end = kallsyms_lookup_name("_etext");    // 内核代码段的结束地址
    size_t size = end - start;
    size_t i;

    pr_info("Kernel code segment: _text=%px, _etext=%px, size=%zu bytes\n", start, end, size);

    // 读取前 16 字节的代码段内容
    pr_info("Reading first 16 bytes of the kernel text segment:\n");
    for (i = 0; i < 16 && i < size; i++) {
        pr_info("Byte %02zu: 0x%02x\n", i, (unsigned char)start[i]);
    }

    pr_info("Initializing jump table module.\n");

    // 测试代码
    for (i = 0; i < 6; i++) {
        process_code(i);
    }

    return 0;
}

static void __exit jump_table_module_exit(void)
{
    pr_info("Exiting jump table module.\n");
}

module_init(jump_table_module_init);
module_exit(jump_table_module_exit);
