/* gopipe.c
 *
 * CSC 360, Summer 2022
 *
 * Execute up to four instructions, piping the output of each into the
 * input of the next.
 *
 * Please change the following before submission:
 *
 * Author: Alireza Tahmasebi
 * Login:  alirezatahmasebi1378@uvic.ca 
 */

/* Note: The following are the **ONLY** header files you are
 * permitted to use for this assignment! */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#define MAX_INPUT_LINE 80
#define MAX_NUM_TOKENS 8

int main() {
    char input[320] = ""; // declare an array of chars of size 320 (4 * 80 characters) for entire input from stdin
    char ch; // declare a char for storing characters entered form stdin
    char str[2]; // declare an array of chars for constructing a string from every character entered from stdin
    int count = 0; // declare an int to keep track of our position in an input line/number of characters in the line
    int instructions_count = 0; // declare an int to count the number of instructions/commands
    int num_tokens = 0; // declare an int to count the number of tokens
    
    // read up to 1 byte from stdin into the buffer starting at ch
    while(read(0, &ch, 1) > 0){
        str[0]= ch; // set the first character to be a single character from stdin
        str[1]='\0'; // set the null character to be the second and last character of the string
        strcat(input,str); // concatenate the input array with the string
        if(ch == '\n' && count == 0) {
            // if the user presses enter at the start of the first line (without entering any instructions), the program quits
            if(instructions_count == 0){
                exit(0); // report successful termination of the program
            }
            // if the user presses enter at the start of a line (other than the first line), stop getting input from stdin and start executing the entered instruction(s)
            else{
                break;
            }
        } 
        // if the user presses enter and we are not at the start of a line (an instruction has been typed)
        else if(ch == '\n' && count != 0) {
            instructions_count++; // increment the number of instructions
            // if the user has entered 4 instructions, stop getting input from stdin and start executing the entered instruction(s) 
            if(instructions_count == 4){
                break; 
            }
            count = 0; // reset the number of characters in the line to 0
            continue; // prompt the user to give another instruction
        }
        count++; // increment the number of characters in the line
    }
    
    char first_instruction[MAX_INPUT_LINE]; // declare a char array of size 80 for the first entered instruction/command
    char second_instruction[MAX_INPUT_LINE]; // declare a char array of size 80 for the second entered instruction/command
    char third_instruction[MAX_INPUT_LINE]; // declare a char array of size 80 for the third entered instruction/command
    char fourth_instruction[MAX_INPUT_LINE]; // declare a char array of size 80 for the fourth entered instruction/command
    char *t;
    int i = 0;
    
    t = strtok(input, "\n"); // split the entire input entered by the user into a series of lines, each corresponding to an instruction
    while (t != NULL && i < instructions_count) {
        if (i == 0){
            strncpy(first_instruction, t, MAX_INPUT_LINE); // copy the first line to the char array for the first instruction
        }
        else if (i == 1){
            strncpy(second_instruction, t, MAX_INPUT_LINE); // copy the second line to the char array for the second instruction
        }
        else if (i == 2){
            strncpy(third_instruction, t, MAX_INPUT_LINE); // copy the third line to the char array for the third instruction
        }
        else if (i == 3){
            strncpy(fourth_instruction, t, MAX_INPUT_LINE); // copy the fourth line to the char array for the fourth instruction
        }
        i++;
        t = strtok(NULL, "\n");
    }
    
    int first_num_tokens = 0; // initialize the number of tokens for the first instruction to be 0
    int second_num_tokens = 0; // initialize the number of tokens for the second instruction to be 0
    int third_num_tokens = 0; // initialize the number of tokens for the third instruction to be 0
    int fourth_num_tokens = 0; // initialize the number of tokens for the fourth instruction to be 0
    
    char *first_instructions_tokens[MAX_NUM_TOKENS]; 
    char *second_instructions_tokens[MAX_NUM_TOKENS];
    char *third_instructions_tokens[MAX_NUM_TOKENS];
    char *fourth_instructions_tokens[MAX_NUM_TOKENS];
    char *s; 
    
    for (i = 0; i < instructions_count; i++){
        if (i == 0){
            s = strtok(first_instruction, " ");
        }
        else if (i == 1){
            s = strtok(second_instruction, " ");
        }
        else if (i == 2){
            s = strtok(third_instruction, " ");
        }
        else if (i == 3){
            s = strtok(fourth_instruction, " ");
        }
        
        while (s != NULL && num_tokens < MAX_NUM_TOKENS) {
            if (i == 0) {
                first_instructions_tokens[first_num_tokens] = s;
                first_num_tokens++;
            }
            else if (i == 1) {
                second_instructions_tokens[second_num_tokens] = s;
                second_num_tokens++;
            }
            else if (i == 2) {
                third_instructions_tokens[third_num_tokens] = s;
                third_num_tokens++;
            }
            else if (i == 3) {
                fourth_instructions_tokens[fourth_num_tokens] = s;
                fourth_num_tokens++;
            }
            num_tokens++;
            s = strtok(NULL, " ");
        }
    }
    
    char *arg10 = NULL;
    char *arg11 = NULL;
    char *arg12 = NULL;
    char *arg13 = NULL;
    char *arg14 = NULL;
    char *arg15 = NULL;
    char *arg16 = NULL;
    char *arg17 = NULL;
    
    if(first_num_tokens == 1){
        arg10 = first_instructions_tokens[0];
    }
    else if(first_num_tokens == 2){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
    }
    else if(first_num_tokens == 3){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
    }
    else if(first_num_tokens == 4){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
        arg13 = first_instructions_tokens[3];
    }
    else if(first_num_tokens == 5){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
        arg13 = first_instructions_tokens[3];
        arg14 = first_instructions_tokens[4];
    }
    else if(first_num_tokens == 6){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
        arg13 = first_instructions_tokens[3];
        arg14 = first_instructions_tokens[4];
        arg15 = first_instructions_tokens[5];
    }
    else if(first_num_tokens == 7){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
        arg13 = first_instructions_tokens[3];
        arg14 = first_instructions_tokens[4];
        arg15 = first_instructions_tokens[5];
        arg16 = first_instructions_tokens[6];
    }
    else if(first_num_tokens == 8){
        arg10 = first_instructions_tokens[0];
        arg11 = first_instructions_tokens[1];
        arg12 = first_instructions_tokens[2];
        arg13 = first_instructions_tokens[3];
        arg14 = first_instructions_tokens[4];
        arg15 = first_instructions_tokens[5];
        arg16 = first_instructions_tokens[6];
        arg17 = first_instructions_tokens[7];
    }
    
    char *args1[] = {arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, 0};
    
    char *arg20 = NULL;
    char *arg21 = NULL;
    char *arg22 = NULL;
    char *arg23 = NULL;
    char *arg24 = NULL;
    char *arg25 = NULL;
    char *arg26 = NULL;
    char *arg27 = NULL;
    
    if(second_num_tokens == 1){
        arg20 = second_instructions_tokens[0];
    }
    else if(second_num_tokens == 2){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
    }
    else if(second_num_tokens == 3){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
    }
    else if(second_num_tokens == 4){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
        arg23 = second_instructions_tokens[3];
    }
    else if(second_num_tokens == 5){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
        arg23 = second_instructions_tokens[3];
        arg24 = second_instructions_tokens[4];
    }
    else if(second_num_tokens == 6){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
        arg23 = second_instructions_tokens[3];
        arg24 = second_instructions_tokens[4];
        arg25 = second_instructions_tokens[5];
    }
    else if(second_num_tokens == 7){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
        arg23 = second_instructions_tokens[3];
        arg24 = second_instructions_tokens[4];
        arg25 = second_instructions_tokens[5];
        arg26 = second_instructions_tokens[6];
    }
    else if(second_num_tokens == 8){
        arg20 = second_instructions_tokens[0];
        arg21 = second_instructions_tokens[1];
        arg22 = second_instructions_tokens[2];
        arg23 = second_instructions_tokens[3];
        arg24 = second_instructions_tokens[4];
        arg25 = second_instructions_tokens[5];
        arg26 = second_instructions_tokens[6];
        arg27 = second_instructions_tokens[7];
    }
    
    char *args2[] = {arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, 0};
    
    char *arg30 = NULL;
    char *arg31 = NULL;
    char *arg32 = NULL;
    char *arg33 = NULL;
    char *arg34 = NULL;
    char *arg35 = NULL;
    char *arg36 = NULL;
    char *arg37 = NULL;
    
    if(third_num_tokens == 1){
        arg30 = third_instructions_tokens[0];
    } 
    else if(third_num_tokens == 2){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
    }
    else if(third_num_tokens == 3){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
    }
    else if(third_num_tokens == 4){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
        arg33 = third_instructions_tokens[3];
    }
    else if(third_num_tokens == 5){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
        arg33 = third_instructions_tokens[3];
        arg34 = third_instructions_tokens[4];
    }
    else if(third_num_tokens == 6){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
        arg33 = third_instructions_tokens[3];
        arg34 = third_instructions_tokens[4];
        arg35 = third_instructions_tokens[5];
    }
    else if(third_num_tokens == 7){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
        arg33 = third_instructions_tokens[3];
        arg34 = third_instructions_tokens[4];
        arg35 = third_instructions_tokens[5];
        arg36 = third_instructions_tokens[6];

    }
    else if(third_num_tokens == 8){
        arg30 = third_instructions_tokens[0];
        arg31 = third_instructions_tokens[1];
        arg32 = third_instructions_tokens[2];
        arg33 = third_instructions_tokens[3];
        arg34 = third_instructions_tokens[4];
        arg35 = third_instructions_tokens[5];
        arg36 = third_instructions_tokens[6];
        arg37 = third_instructions_tokens[7];
    }
    
    char *args3[] = {arg30, arg31, arg32, arg33, arg34, arg35, arg36, arg37, 0};
    
    char *arg40 = NULL;
    char *arg41 = NULL;
    char *arg42 = NULL;
    char *arg43 = NULL;
    char *arg44 = NULL;
    char *arg45 = NULL;
    char *arg46 = NULL;
    char *arg47 = NULL;
    
    if(fourth_num_tokens == 1){
        arg40 = fourth_instructions_tokens[0];
    }
    else if(fourth_num_tokens == 2){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
    }
    else if(fourth_num_tokens == 3){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
    }
    else if(fourth_num_tokens == 4){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
        arg43 = fourth_instructions_tokens[3];
    }
    else if(fourth_num_tokens == 5){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
        arg43 = fourth_instructions_tokens[3];
        arg44 = fourth_instructions_tokens[4];
    }
    else if(fourth_num_tokens == 6){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
        arg43 = fourth_instructions_tokens[3];
        arg44 = fourth_instructions_tokens[4];
        arg45 = fourth_instructions_tokens[5];
    }
    else if(fourth_num_tokens == 7){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
        arg43 = fourth_instructions_tokens[3];
        arg44 = fourth_instructions_tokens[4];
        arg45 = fourth_instructions_tokens[5];
        arg46 = fourth_instructions_tokens[6];

    }
    else if(fourth_num_tokens == 8){
        arg40 = fourth_instructions_tokens[0];
        arg41 = fourth_instructions_tokens[1];
        arg42 = fourth_instructions_tokens[2];
        arg43 = fourth_instructions_tokens[3];
        arg44 = fourth_instructions_tokens[4];
        arg45 = fourth_instructions_tokens[5];
        arg46 = fourth_instructions_tokens[6];
        arg47 = fourth_instructions_tokens[7];
    }
    
    char *args4[] = {arg40, arg41, arg42, arg43, arg44, arg45, arg46, arg47, 0};
    
    char *envp[] = {0};
    int pid, pid1, pid2, pid3; // process identifiers
    int status; // status information relating to child processes
    int fd12[2]; // create an array to return 2 file descriptors referring to the ends of the pipe between 1st and 2nd commands' processes
    int fd23[2]; // create an array to return 2 file descriptors referring to the ends of the pipe between 2nd and 3rd commands' processes
    int fd34[2]; // create an array to return 2 file descriptors referring to the ends of the pipe between 3rd and 4th commands' processes

    // print the output of the first and only command
    if (instructions_count == 1) {
        // fork & execve are used to create a child process that runs the first command with zero or more argument(s)
        if ((pid = fork()) == 0) {
            execve(args1[0], args1, envp); // execute the first instruction with any command-line argument(s)
        }
        waitpid(pid, &status, 0); // wait for child process with process id of pid to finish
    }

    // connect the output of first command to the input of the second command
    else if (instructions_count == 2) {
        pipe(fd12); // create the pipe
        // fork & execve are used to create a child process that runs the first command with zero or more argument(s)
        if ((pid1 = fork()) == 0) {
            dup2(fd12[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the pipe
            close(fd12[0]); // close the read end of the pipe
            execve(args1[0], args1, envp); // execute the first instruction with any command-line argument(s)
        }
        // fork & execve are used to create a child process that runs the second command with zero or more argument(s)
        if ((pid2 = fork()) == 0) {
            dup2(fd12[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the pipe
            close(fd12[1]); // close the write end of the pipe
            execve(args2[0], args2, envp); // execute the second instruction with any command-line argument(s)
        }

        close(fd12[0]); // close the read end of the pipe
        close(fd12[1]); // close the write end of the pipe

        waitpid(pid1, &status, 0); // wait for child process with process id of pid1 to finish
        waitpid(pid2, &status, 0); // wait for child process with process id of pid2 to finish
    }
    // connect the output of first command to the input of the second command and then connect the output of the second command to the input of the third command
    else if (instructions_count == 3) {
        pipe(fd12); // create the first pipe
        pipe(fd23); // create the second pipe
        // fork & execve are used to create a child process that runs the first command with zero or more argument(s)
        if ((pid1 = fork()) == 0) {
            dup2(fd12[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the first pipe
            close(fd12[0]); // close the read end of the first pipe
            close(fd12[1]); // close the write end of the first pipe
            execve(args1[0], args1, envp); // execute the first instruction with any command-line argument(s)
        }
        // fork & execve are used to create a child process that runs the second command with zero or more argument(s)
        if ((pid2 = fork()) == 0) {
            dup2(fd12[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the first pipe
            close(fd12[0]); // close the read end of the first pipe
            close(fd12[1]); // close the write end of the first pipe
            dup2(fd23[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the second pipe
            close(fd23[0]); // close the read end of the second pipe
            close(fd23[1]); // close the write end of the second pipe
            execve(args2[0], args2, envp); // execute the second instruction with any command-line argument(s)
        }
        // the parent process uses execve to run the third command with zero or more argument(s)
        else{
            close(fd12[0]); // close the read end of the first pipe
            close(fd12[1]); // close the write end of the first pipe
            dup2(fd23[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the second pipe
            close(fd23[0]); // close the read end of the second pipe
            close(fd23[1]); // close the write end of the second pipe
            execve(args3[0], args3, envp); // execute the third instruction with any command-line argument(s)
        }

        waitpid(pid1, &status, 0); // wait for child process with process id of pid1 to finish
        waitpid(pid2, &status, 0); // wait for child process with process id of pid2 to finish 
        
    }
    // connect the output of first command to the input of the second command, then connect the output of the second command to the input of the third command, and finally connect the output of the third command to the input of the fourth command 
    else if (instructions_count == 4) {
        pipe(fd12); // create the first pipe
        pipe(fd23); // create the second pipe
        pipe(fd34); // create the third pipe
        
        if ((pid1 = fork()) == 0) {
            if ((pid2 = fork()) == 0) {
                if ((pid3 = fork()) == 0) {
                    dup2(fd12[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the first pipe
                    close(fd12[0]); // close the read end of the first pipe
                    close(fd12[1]); // close the write end of the first pipe
                    close(fd23[0]); // close the read end of the second pipe
                    close(fd23[1]); // close the write end of the second pipe
                    close(fd34[0]); // close the read end of the third pipe
                    close(fd34[1]); // close the write end of the third pipe
                    execve(args1[0], args1, envp); // execute the first instruction with any command-line argument(s)
                }
                else{
                    dup2(fd12[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the first pipe
                    dup2(fd23[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the second pipe
                    close(fd12[0]); // close the read end of the first pipe
                    close(fd12[1]); // close the write end of the first pipe
                    close(fd23[0]); // close the read end of the second pipe
                    close(fd23[1]); // close the write end of the second pipe
                    close(fd34[0]); // close the read end of the third pipe
                    close(fd34[1]); // close the write end of the third pipe
                    execve(args2[0], args2, envp); // execute the second instruction with any command-line argument(s)
                }
            }
            else{
                dup2(fd23[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the second pipe
                dup2(fd34[1], 1); // force the file descriptor corresponding to stdout to be connected to the write end of the third pipe
                close(fd12[0]); // close the read end of the first pipe
                close(fd12[1]); // close the write end of the first pipe
                close(fd23[0]); // close the read end of the second pipe
                close(fd23[1]); // close the write end of the second pipe
                close(fd34[0]); // close the read end of the third pipe
                close(fd34[1]); // close the write end of the third pipe
                execve(args3[0], args3, envp); // execute the third instruction with any command-line argument(s)
            }
        }
        else{
            dup2(fd34[0], 0); // force the file descriptor corresponding to stdin to be connected to the read end of the third pipe
            close(fd12[0]); // close the read end of the first pipe
            close(fd12[1]); // close the write end of the first pipe
            close(fd23[0]); // close the read end of the second pipe
            close(fd23[1]); // close the write end of the second pipe
            close(fd34[0]); // close the read end of the third pipe
            close(fd34[1]); // close the write end of the third pipe
            execve(args4[0], args4, envp); // execute the fourth instruction with any command-line argument(s)
        }
        
        waitpid(pid1, &status, 0); // wait for child process with process id of pid1 to finish
        waitpid(pid2, &status, 0); // wait for child process with process id of pid2 to finish
        waitpid(pid3, &status, 0); // wait for child process with process id of pid3 to finish
    
    }
    
}
