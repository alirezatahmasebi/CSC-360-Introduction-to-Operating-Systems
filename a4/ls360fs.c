#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}

void unpack_datetime(unsigned char *time, short *year, short *month, short *day, short *hour, short *minute, short *second) {
    assert(time != NULL);

    memcpy(year, time, 2);
    *year = htons(*year);

    *month = (unsigned short)(time[2]);
    *day = (unsigned short)(time[3]);
    *hour = (unsigned short)(time[4]);
    *minute = (unsigned short)(time[5]);
    *second = (unsigned short)(time[6]);
}

int main(int argc, char *argv[]) {
    superblock_entry_t sb; // use the superblock_entry_t variable to create a variable of type struct superblock_entry
    directory_entry_t dt; // use the directory_entry_t variable to create a variable of type struct directory_entry
    int  i; // counter variable for iterating for loops
    char *imagename = NULL; // char array (string) for storing name of image file received as an argument
    FILE *f; // file pointer variable for reading from image files
    // declare variables for storing information about when the file was modified
    short year = 0;
    short month = 0;
    short day = 0;
    short hour = 0;
    short minute = 0;
    short second = 0;
    char *str_month;
    
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL) {
        fprintf(stderr, "usage: ls360fs --image <imagename>\n");
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
    
    // iterate through no more than 64 root directory entries (max # of entries is fixed for 360fs)
    for(int i = 0; i < MAX_DIR_ENTRIES; i++) {
        fread(&dt.status, 1, 1, f); // read status of file 
        // check if the entry is for a normal file
        if (dt.status == 1){
            fread(&dt.start_block, 4, 1, f); // read starting block
            fread(&dt.num_blocks, 4, 1, f); // read # of blocks in file
            fread(&dt.file_size, 4, 1, f); // read file size (in bytes)
            fread(&dt.create_time, DIR_TIME_WIDTH, 1, f); // read create time
            fread(&dt.modify_time, DIR_TIME_WIDTH, 1, f); // read modify time
            fread(&dt.filename, DIR_FILENAME_MAX, 1, f); // read filename
            fread(&dt._padding, 6, 1, f); // read unused portion (set to 0xff)
            // call by reference to store values in year, month, day, hour, minute, and second from the following form: YYYYMMDDHHMMSS  
            unpack_datetime(dt.modify_time, &year, &month, &day, &hour, &minute, &second);
            str_month = month_to_string(month); // get name of the month as a string
            dt.file_size = ntohl(dt.file_size); // convert file size from big endian to little endian
            printf("%8d %hi-%s-%hi %hi:%hi:%hi %s\n", dt.file_size, year, str_month, day, hour, minute, second, dt.filename); // print the date/time of when the file was modified
        }
        else{
            continue;
        }
    }
        
    fclose(f); // close the file
    
    return 0; 
}
