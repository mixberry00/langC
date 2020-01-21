#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

int len,temp;

char *msg;
//changed func result type int to ssize_t
ssize_t read_proc(struct file *filp,char *buf,size_t count,loff_t *offp )
{
    if(count>temp)
        count=temp;
        
    temp=temp-count;
    raw_copy_to_user(buf,msg, count);
    if(count==0)
        temp=len;

    return count;
}
//changed func result type int to ssize_t
ssize_t write_proc(struct file *filp,const char *buf,size_t count,loff_t *offp)
{
    raw_copy_from_user(msg,buf,count);
    len=count;
    temp=len;
    return count;
}

struct file_operations proc_fops = {
read:   read_proc,
write: write_proc
};

void create_new_proc_entry(void)  //use of void for no arguments is compulsory now
{
    proc_create("hello",0,NULL,&proc_fops);
    msg=kmalloc(10*sizeof(char), GFP_KERNEL);
}


int proc_init (void) {
    create_new_proc_entry();
    return 0;
}

void proc_cleanup(void) {
    remove_proc_entry("hello",NULL);
    kfree(msg);
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
