#include <assert.h>
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
    directory_entry_t dt; // use the directory_entry_t variable to create a variable of type struct directory_entry
    FAT_entry_t ft; // use the FAT_entry_t variable to create a variable of type struct FAT_entry
    int  i; // counter variable for iterating for loops
    char *imagename = NULL; // char array (string) for storing name of image file received as an argument
    char *filename  = NULL; // char array (string) for storing name of directory entry file received as an argument
    FILE *f; // file pointer variable for reading from image files
    int checkfile = 0; // use like a boolean variable to check if file is found in the root directory
    
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: cat360fs --image <imagename> " \
            "--file <filename in image>");
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

    // convert superblock values from network byte order (big endian) to host byte order (little endian)
    // in the image file, the format is big endian and in jupyterhub, the format is little endian
    sb.block_size = ntohs(sb.block_size);
    sb.num_blocks = ntohl(sb.num_blocks);
    sb.fat_start = ntohl(sb.fat_start);
    sb.fat_blocks = ntohl(sb.fat_blocks);
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);   
    
    // set the file position indicator to the beginning of the root directory
    fseek(f, sb.block_size * sb.dir_start, SEEK_SET); 

    char datablock[sb.block_size+1]; // declare a variable for reading content from data block as a string
    
    // iterate through no more than 64 root directory entries (max # of entries is fixed for 360fs)
    for(int i = 0; i < MAX_DIR_ENTRIES; i++) {
        fread(&dt.status, 1, 1, f); // read status of file 
        if (dt.status == 1){
            fread(&dt.start_block, 4, 1, f); // read starting block (*block S*)
            fread(&dt.num_blocks, 4, 1, f); // read # of blocks in file
            fread(&dt.file_size, 4, 1, f); // read file size (in bytes)
            fread(&dt.create_time, DIR_TIME_WIDTH, 1, f); // read create time
            fread(&dt.modify_time, DIR_TIME_WIDTH, 1, f); // read modify time
            fread(&dt.filename, DIR_FILENAME_MAX, 1, f); // read filename
            fread(&dt._padding, 6, 1, f); // read unused portion (set to 0xff)
            // check if the filename received as an argument and the entry's filename is the same
            if (strcmp(dt.filename, filename) == 0){
                checkfile = 1; // indicate that the file is found in the root directory
                dt.start_block = ntohl(dt.start_block); // convert value of root directory's start block from big endian to little endian
                // loop until reaching the last block in a file
                while(ft.fat_block != 0xffffffff){
                    // set the file position indicator to the beginning of the data block section
                    fseek(f, sb.block_size * dt.start_block, SEEK_SET); 
                    fread(datablock, sb.block_size, 1, f); // read content of data block
                    datablock[sb.block_size]='\0'; // mark the end of the string with a null terminator
                    printf("%s", datablock); // print the content of the the data block that was read
                    fseek(f, sb.block_size + dt.start_block * 4, SEEK_SET); // set the file position indicator to entry S of the FAT
                    fread(&ft.fat_block, 4, 1, f); // read next entry in the FAT (T = FAT[S])
                    dt.start_block = ntohl(ft.fat_block); // value in FAT is the next block in the file (set S to T)
                }
            }
        }
        else{
            continue;
        }
    }

    // print a “file not found” message and exit the program if the file is not in the root directory
    if (checkfile == 0){
        printf("file not found\n");
        exit(1);
    }
        
    fclose(f); // close the file
    
    return 0; 
}
