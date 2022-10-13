/* getstats.c 
 *
 * CSC 360, Summer 2022
 *
 * - If run without an argument, dumps information about the PC to STDOUT.
 *
 * - If run with a process number created by the current user, 
 *   dumps information about that process to STDOUT.
 *
 * Please change the following before submission:
 *
 * Author: Alireza Tahmasebi
 * Login:  alirezatahmasebi1378@uvic.ca 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 500

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */

void print_process_info(char * process_num) {
    FILE *infile; // file pointer variable for reading from files
    char line[MAX_LINE_LEN]; // array of chars of size 500 for lines read from infile
    char name[MAX_LINE_LEN]; // array of chars of size 500 for the name of the process
    char file_name[MAX_LINE_LEN]; // array of chars of size 500 for the filename of the process
    char cmdline[MAX_LINE_LEN]; // array of chars of size 500 for the console command that started the process (if any)
    char threads[MAX_LINE_LEN]; // array of chars of size 500 for the number of threads running in the process 
    int voluntary_ctxt_switches; // number of voluntary context switches that have occurred during the running of the process 
    int nonvoluntary_ctxt_switches; // number of nonvoluntary context switches that have occurred during the running of the process 
    int name_read = 0; // use for checking if "Name:" has been read as a token
    int threads_read = 0; // use for checking if "Threads:" has been read as a token 
    int voluntary_ctxt_switches_read = 0; // use for checking if "voluntary_ctxt_switches:" has been read as a token
    int nonvoluntary_ctxt_switches_read = 0; // use for checking if "nonvoluntary_ctxt_switches:" has been read as a token 
    char status_path[MAX_LINE_LEN] = "/proc/"; // array of chars for the first part of path to a process's status file
    char cmdline_path[MAX_LINE_LEN] = "/proc/"; // array of chars for the first part of path to a process's cmdline file 
    char comm_path[MAX_LINE_LEN] = "/proc/"; // array of chars for the first part of path to a process's comm file 

    strcat(status_path, process_num); // concatenate the process number as the middle part of the path to the status file
    strcat(status_path, "/status"); // concatenate the final part of path to the status file 
    infile = fopen(status_path, "r"); // open the status file for a process in read mode
    
    // check if unable to open the status file
    if (infile == NULL) { 
        fprintf(stderr, "Process number %s not found.\n", process_num); // output error message since the process number must be invalid 
        exit(1); // report abnormal termination of the program
    }
    
    // read lines until we reach end of infile
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {         
        char *t = strtok(line, "\t\n"); // separate lines into tokens with delimiter set as "\t\n"
        // keep getting tokens 
        while (t != NULL) {
            // check if the token is "Name:" and that we haven't read the noted string before
            if ((strcmp(t, "Name:") == 0) && (name_read == 0)){ 
                name_read = 1; 
            }
            // copy the process name into the name array if the previous token is "Name:"
            else if (name_read == 1){
                strncpy(name, t, MAX_LINE_LEN);
                name_read = 2;
            }
            // check if the token is "Threads:" and that we haven't read the noted string before
            else if ((strcmp(t, "Threads:") == 0) && (threads_read == 0)){
                threads_read = 1;
            }
            // copy the number of threads into the threads array if the previous token is "Threads:"
            else if (threads_read == 1){
                strncpy(threads, t, MAX_LINE_LEN);
                threads_read = 2;
            }
            // check if the token is "voluntary_ctxt_switches:" and that we haven't read the noted string before 
            else if ((strcmp(t, "voluntary_ctxt_switches:") == 0) && (voluntary_ctxt_switches_read == 0))
            {
                voluntary_ctxt_switches_read = 1;
            }
            // get the number of voluntary context switches if the previous token is "voluntary_ctxt_switches:" 
            else if (voluntary_ctxt_switches_read == 1){
                voluntary_ctxt_switches = atoi(t); // convert token t to an int
                voluntary_ctxt_switches_read = 2;
            }
            // check if the token is "nonvoluntary_ctxt_switches:" and that we haven't read the noted string before 
            else if ((strcmp(t, "nonvoluntary_ctxt_switches:") == 0) && (nonvoluntary_ctxt_switches_read == 0)){
                nonvoluntary_ctxt_switches_read = 1;
            }
            // get the number of nonvoluntary context switches if the previous token is "nonvoluntary_ctxt_switches:" 
            else if (nonvoluntary_ctxt_switches_read == 1){
                nonvoluntary_ctxt_switches = atoi(t); // convert token t to an int
                nonvoluntary_ctxt_switches_read = 2; 
            }
            t = strtok(NULL, "\t\n");
        }
    }    
    
    fclose(infile); // close the stream pointed to by infile

    strcat(cmdline_path, process_num); // concatenate the process number as the middle part of the path to the cmdline file
    strcat(cmdline_path, "/cmdline"); // concatenate the final part of path to the cmdline file
    infile = fopen(cmdline_path, "r"); // open the cmdline file for a process in read mode

    // obtain the command line that was used to start the process
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        strncpy(cmdline, line, MAX_LINE_LEN); 
    }
    
    fclose(infile); // close the stream pointed to by infile
    
    strcat(comm_path, process_num); // concatenate the process number as the middle part of the path to the comm file
    strcat(comm_path, "/comm"); // concatenate the final part of path to the comm file
    infile = fopen(comm_path, "r"); // open the comm file for a process in read mode
    
    // obtain the file name of the process
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        strncpy(file_name, line, MAX_LINE_LEN); 
    }
    
    fclose(infile); // close the stream pointed to by infile
    
    // Print the number of the process
    printf("%s %s\n", "Process number:", process_num);    
        
    // Print the name of the process
    printf("%s %s\n", "Name:", name);
    
    // Print the filename of the process (if any)
    printf("%s %s", "Filename (if any):", file_name);

    // Print the console command that started the process (if any)
    printf("%s %s\n", "Console command (if any):", cmdline);
    
    // Print the number of threads running in the process
    printf("%s %s\n", "Threads:", threads);

    // Print the total number of context switches that have occurred during the running of the process.
    printf("%s %d\n", "Total context switches:", nonvoluntary_ctxt_switches + voluntary_ctxt_switches);
    
} 

void print_full_info() {
    FILE *infile; // file pointer variable for reading from files
    char line[MAX_LINE_LEN]; // array of chars of size 500 for lines read from infile
    int model_name_read = 0; // use for checking if "model name" has been read
    char model_name[MAX_LINE_LEN]; // array of chars of size 500 for the model name of the CPU
    int cpu_cores_read = 0; // use for checking if "cpu cores" has been read
    char cpu_cores[MAX_LINE_LEN]; // array of chars of size 500 for the number of cores  
    int available_memory_read = 0; // use for checking if "MemTotal:" has been read
    char available_memory[MAX_LINE_LEN]; // array of chars of size 500 for the total memory available to the system of the CPU
    int uptime_seconds; // the uptime (amount of time the system has been running in seconds)
    int uptime_read = 0; // use for checking if the uptime value has been read
    
    infile = fopen("/proc/cpuinfo", "r"); // open the cpuinfo file in read mode
    
    // read lines until we reach end of infile
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        char *t = strtok(line, "\t:"); // separate lines into tokens with delimiter set as "\t:"
        // keep getting tokens 
        while (t != NULL) {
            // check if the token is "cpu cores" and that we haven't read the noted string before
            if ((strcmp(t, "cpu cores") == 0) && (cpu_cores_read == 0)){
                cpu_cores_read = 1;
            }
            // copy the number of cores into the cpu_cores array if the previous token is "cpu cores"
            else if (cpu_cores_read == 1){
                strncpy(cpu_cores, t, MAX_LINE_LEN);
                cpu_cores_read = 2;
            }
            // check if the token is "model name" and that we haven't read the noted string before
            else if ((strcmp(t, "model name") == 0) && (model_name_read == 0)){
                model_name_read = 1;
            }
            // copy the model name of the CPU into the model_name array if the previous token is "model name"
            else if (model_name_read == 1){
                strncpy(model_name, t, MAX_LINE_LEN);
                model_name_read = 2;
            }
            t = strtok(NULL, "\t:");
        }
    }
    fclose(infile); // close the stream pointed to by infile

    printf("%s%s", "model name:", model_name); // Print the model name of the CPU
    
    printf("%s%s", "cpu cores:", cpu_cores); // Print the number of cores
    
    infile = fopen("/proc/version", "r"); // open the version file in read mode
    
    // read lines until we reach end of infile
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        printf("%s", line);  // Print the version of Linux running in the environment   
    }
    fclose(infile); // close the stream pointed to by infile
    
    infile = fopen("/proc/meminfo", "r"); // open the meminfo file in read mode
    
    // read lines until we reach end of infile
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        char *t = strtok(line, " "); // separate lines into tokens with delimiter set as " "
        // keep getting tokens 
        while (t != NULL) {
            // check if the token is "MemTotal:"
            if (strcmp(t, "MemTotal:") == 0){
                available_memory_read = 1;
            }
            // copy the total memory available to the system into the available_memory array if the previous token is "MemTotal:"
            else if (available_memory_read == 1){
                strncpy(available_memory, t, MAX_LINE_LEN);
                available_memory_read = 2;
            }
            t = strtok(NULL, " ");
        }
    }
    fclose(infile); // close the stream pointed to by infile
    
    printf("%s %s %s\n", "MemTotal:", available_memory, "kB"); // Print the total memory available to the system, in kilobytes

    infile = fopen("/proc/uptime", "r"); // open the uptime file in read mode
    
    while (fgets(line, MAX_LINE_LEN-1, infile) != NULL) {        
        char *t = strtok(line, " "); // separate lines into tokens with delimiter set as " "
        while (t != NULL) {
            // get the first number in the file (uptime of the system (including time spent in suspend) in seconds) 
            if (uptime_read == 0){
                uptime_seconds = atoi(t); // convert token t to an int
                uptime_read = 1;
            }
            t = strtok(NULL, " ");
        }
    }
    
    fclose(infile); // close the stream pointed to by infile
    
    int days = uptime_seconds / 86400; // get the # of days -> uptime_seconds / (24 hours/day * 60 minutes/hour * 60 seconds/minute)
    uptime_seconds = uptime_seconds % 86400; // get the remaining seconds
    int hours = uptime_seconds / 3600; // get the # of hours -> uptime_seconds / (60 minutes/hour * 60 seconds/minute)
    uptime_seconds = uptime_seconds % 3600; // get the remaining seconds   
    int minutes = uptime_seconds / 60; // get the # of minutes -> uptime_seconds / (60 seconds/minute) 
    uptime_seconds = uptime_seconds % 60; // get the remaining seconds
    // Print the 'uptime' (the amount of time the system has been running, expressed in terms of days, hours, minutes and seconds)
    printf("%s %d %s %d %s %d %s %d %s", "Uptime:", days, "days,", hours, "hours,", minutes, "minutes,", 
            uptime_seconds, "seconds\n");
}

int main(int argc, char ** argv) {  
    // if run without arguments, print to the console interesting statistics about the environment in which the program is run
    if (argc == 1) {
        print_full_info();
    } 
    // if run with a numerical argument, print to the console statistic about the corresponding process running in the environment
    else {
        print_process_info(argv[1]);
    }
}
