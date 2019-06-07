/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

/*
 * Device: Disk
 */
#define DEV_DISK_ADDRESS         0x13000000
#define DEV_DISK_OFFSET          0x0000
#define DEV_DISK_OFFSET_HIGH32   0x0008
#define DEV_DISK_ID              0x0010
#define DEV_DISK_START_OPERATION 0x0020
#define DEV_DISK_STATUS          0x0030
#define DEV_DISK_BUFFER          0x4000

#define DEV_DISK_BUFFER_LEN 0x200

/*  Operations:  */
#define DEV_DISK_OPERATION_READ  0
#define DEV_DISK_OPERATION_WRITE 1

#define PHYSADDR_OFFSET ((signed int)0xA0000000)
#define DISK_ADDRESS    (PHYSADDR_OFFSET + DEV_DISK_ADDRESS)

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurred during read the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
    // 0x200: the size of a sector: 512 bytes.
    int    offset_begin = secno * 0x200;
    int    offset_end   = offset_begin + nsecs * 0x200;
    int    offset;
    u_char status;
    u_char op      = DEV_DISK_OPERATION_READ;
    u_int  ptr_dst = (u_int)dst;

    // select disk
    // *((volatile int *)(DISK_ADDRESS + DEV_DISK_ID)) = diskno;
    syscall_write_dev((u_int)&diskno, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(int));

    for (offset = offset_begin; offset < offset_end; offset += 0x200)
    {
        // set offset
        // *((volatile int *)(DISK_ADDRESS + DEV_DISK_OFFSET)) = offset;
        syscall_write_dev((u_int)&offset, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(int));
        // operate
        // *((volatile int *)(DISK_ADDRESS + DEV_DISK_START_OPERATION)) =
        //     DEV_DISK_OPERATION_READ;
        syscall_write_dev((u_int)&op, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(u_char));
        // fetch status and check
        // status = *((volatile int *)(DISK_ADDRESS + DEV_DISK_STATUS));
        syscall_read_dev((u_int)&status, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(u_char));
        if (status == 0)
            user_panic("IDE disk read failed.\n");
        // dump sector
        syscall_read_dev(ptr_dst, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, 0x200);
        ptr_dst += 0x200;
    }
}

// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurred during read the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
    // Your code here
    int    offset_begin = secno * 0x200;
    int    offset_end   = offset_begin + nsecs * 0x200;
    int    offset;
    u_char status;
    u_char op      = DEV_DISK_OPERATION_WRITE;
    u_int  ptr_src = (u_int)src;

    // select disk
    // *((volatile int *)(DISK_ADDRESS + DEV_DISK_ID)) = diskno;
    syscall_write_dev((u_int)&diskno, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(int));
    writef("diskno: %d\n", diskno);

    for (offset = offset_begin; offset < offset_end; offset += 0x200)
    {
        // set offset
        // *((volatile int *)(DISK_ADDRESS + DEV_DISK_OFFSET)) = offset;
        syscall_write_dev((u_int)&offset, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(int));
        // write to buffer
        syscall_write_dev(ptr_src, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, 0x200);
        // operate
        // *((volatile int *)(DISK_ADDRESS + DEV_DISK_START_OPERATION)) =
        //     DEV_DISK_OPERATION_WRITE;
        syscall_write_dev((u_int)&op, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(u_char));
        // fetch status and check
        // status = *((volatile int *)(DISK_ADDRESS + DEV_DISK_STATUS));
        syscall_read_dev((u_int)&status, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(u_char));
        if (status == 0)
            user_panic("IDE disk write failed.\n");
        ptr_src += 0x200;
    }
}
