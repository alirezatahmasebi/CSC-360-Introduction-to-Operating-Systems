#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

typedef struct FAT_entry FAT_entry_t; // declare a variable of type struct FAT_entry

// create structure for File Allocation Table (FAT) entries
struct FAT_entry {
    unsigned int fat_block; // declare an int since FAT entries are four-byte long (32 bit) integers
} __attribute__ ((packed));

int main(int argc, char *argv[]) {
    superblock_entry_t sb; // use the superblock_entry_t variable to create a variable of type struct superblock_entry
    FAT_entry_t ft; // use the FAT_entry_t variable to create a variable of type struct FAT_entry
    int  i; // counter variable for iterating for loops
    char *imagename = NULL; // char array (string) for storing name of image file received as an argument
    FILE  *f; // file pointer variable for reading from image files
    int free = 0; // # of available file-system blocks corresponding to entries in FAT
    int resv = 0; // # of reserved file-system blocks corresponding to entries in FAT
    int alloc = 0; // # of allocated file-system blocks corresponding to entries in FAT

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL) {
        fprintf(stderr, "usage: stat360fs --image <imagename>\n");
        exit(1);
    }
    
    f = fopen(imagename, "rb"); // open the file (read information from .img files, which contain binary data)
    
    // read information from superblock (superblock contains all the metadata about the filesystem)
    fread(&sb.magic, FILE_SYSTEM_ID_LEN, 1, f); // read file-system identifier (magic value: 360fs)
    fread(&sb.block_size, 2, 1, f); // read blocksize
    fread(&sb.num_blocks, 4, 1, f); // read file-system size (in blocks)
    fread(&sb.fat_start, 4, 1, f); // read block where FAT starts
    fread(&sb.fat_blocks, 4, 1, f); // read # of blocks in FAT
    fread(&sb.dir_start, 4, 1, f); // read block where root directory starts
    fread(&sb.dir_blocks, 4, 1, f); // read # of blocks in root directory
    
    fseek(f, 30, SEEK_SET); // set the file position indicator 30 bytes from the beginning of the file
                            // size of the superblock is always 30 bytes
    
    // convert superblock values from network byte order (big endian) to host byte order (little endian)
    // in the image file, the format is big endian and in jupyterhub, the format is little endian
    sb.block_size = ntohs(sb.block_size);
    sb.num_blocks = ntohl(sb.num_blocks);
    sb.fat_start = ntohl(sb.fat_start);
    sb.fat_blocks = ntohl(sb.fat_blocks);
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);
    
    for(int i = 0; i < sb.num_blocks; i++) {
        fread(&ft.fat_block, 4, 1, f); // read entries in the FAT (an array of 4-byte integers)
        // check if the corresponding file-system block is available
        if (ft.fat_block == htons(FAT_AVAILABLE)){
            free++;
        }
        // check if the corresponding file-system block is reserved 
        else if (ft.fat_block == htons(FAT_RESERVED)){
            resv++;
        }
        // check if the corresponding file-system block is allocated to some file        
        else {
            alloc++;
        }
    }
    
    // print out the data in the superblock
    printf("-------------------------------------------------\n");
    printf("  Bsz   Bcnt  FATst FATcnt  DIRst DIRcnt\n");
    printf("%5hi %6d %6d %6d %6d %6d\n", sb.block_size, sb.num_blocks, sb.fat_start, sb.fat_blocks, sb.dir_start, sb.dir_blocks);
    
    // print out how many of the file-system blocks are available, reserved, and allocated 
    printf("-------------------------------------------------\n");
    printf(" Free   Resv  Alloc\n");
    printf("%5d %6d %6d\n", free, resv, alloc);

    fclose(f); // close the file

    return 0;
}
