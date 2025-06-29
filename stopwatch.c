#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/timekeeping.h>


#define DEVICE_NAME "stopwatch"
#define DEVICE_CLASS "stopwatch_class"
#define BUFFER_SIZE 256
#define DEVICE_NAME "stopwatch"
#define CLASS_NAME "stopwatch_class"


MODULE_AUTHOR("JEYAPRASATH THEVAR ");
MODULE_DESCRIPTION(" JUST A STOP WATCH TIMER");
MODULE_LICENSE("GPL");

static bool isStopped = true;
static bool isStarted = false;

static ktime_t start_time;
static ktime_t stop_time;

static char message_buffer[BUFFER_SIZE];
static int message_buffer_length;

static int usage_count =0 ;
static int major_number;

static struct class*  stopwatch_class = NULL;
static struct device* stopwatch_device = NULL;


static int dev_open(struct inode *inodep, struct file *filep){
printk("The device is open \n");

return 0;

}

static int dev_release(struct inode *inodep, struct file *filep)
{
printk("the device is close \n");

return 0;

}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset){

if( isStarted == isStopped ){
snprintf(message_buffer , BUFFER_SIZE , "The timer has a undefined state \n" );
copy_to_user(buffer, message_buffer,strlen(message_buffer)); 
return -1;
}

snprintf(message_buffer , BUFFER_SIZE , "The timer is working \n" );

if(copy_to_user(buffer, message_buffer,strlen(message_buffer) != 0)){
printk(KERN_ERR " unable to access userspace \n");
return -1;
} 

return 0;

}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset){

s64 time_diff;

char input[BUFFER_SIZE] = {0};

if (len >= BUFFER_SIZE)
    len = BUFFER_SIZE - 1;

if (copy_from_user(input, buffer, len) != 0) {
    printk(KERN_ERR "unable to copy from userspace\n");
    return -1;
}

input[len] = '\0';



if(strncmp(input,"start",5) == 0){

isStopped = false;
isStarted = true;

start_time = ktime_get();

usage_count++;

}

else if (strncmp(input,"stop",4) == 0){

isStopped = true;
isStarted = false;


stop_time = ktime_get();

usage_count++;

}

else if(strncmp(input,"elapsed",7) == 0){
 
if(isStarted == isStopped){
 printk(KERN_ERR "the clock is in undefined state\n");
 return -1;
 }
 
else if( usage_count <= 0 || ( usage_count % 2  != 0 ) ){
 printk(KERN_ERR "elapsed has been called in an undefined state i.e neither clock still didnt stop or the clock is not started \n");
 return -1;
 }
 
  
time_diff = ktime_to_ms(ktime_sub(stop_time,start_time));
printk(KERN_INFO "the value elapsed is %lld ms ", time_diff);

}


else{
printk(KERN_ERR "input is invalid , choose anyone of these start,stop,elapsed \n");
}


return len;

}

struct file_operations fops = {

.owner = THIS_MODULE,
.open=dev_open,
.read=dev_read,
.write=dev_write,
.release=dev_release,

};

static int __init dev_init(void){
major_number = register_chrdev(0,DEVICE_NAME,&fops);

if( major_number < 0 )
{
printk(KERN_ERR "unable to create device");
return -1;
}

stopwatch_class =  class_create(CLASS_NAME);

if(IS_ERR(stopwatch_class)){

unregister_chrdev(major_number,DEVICE_NAME);
printk(KERN_ERR "unable to create class");
return -1;

}

stopwatch_device = device_create(stopwatch_class,NULL,MKDEV(major_number,0),NULL,DEVICE_NAME);

if(IS_ERR(stopwatch_device)){

class_destroy(stopwatch_class);
unregister_chrdev(major_number,DEVICE_NAME);
printk(KERN_ERR "unable to create device");
return -1;
}
 
printk(KERN_INFO "the device is created using the major number %d " , major_number);

return 0;

}

static void __exit dev_exit(void){
device_destroy(stopwatch_class,MKDEV(major_number, 0));
class_unregister(stopwatch_class);
class_destroy(stopwatch_class);
unregister_chrdev(major_number,DEVICE_NAME);

}

module_init(dev_init);
module_exit(dev_exit);















