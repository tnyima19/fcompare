/**************************************************************************************
Title: Fcompare
Author: Tenzing Nyima
Created on : December 13 2023
Description: Create a program that given a list of pathnames on the commandline, this outputs the name of the file in this
list whose metadata is least in the ordering specified by the operation. If any file is a symbolic
link, it uses the metadata of the link itself. The operations determine the metadata by
which the files are compared, as follows:
-a : by their last access
-b : by their birth times.
-c : by their times of last status change
-m : by their times of last modification
-s : by their apparent size
-u : by their disk usage

Optional arguments.
-r : reversse the sense of comparison
-l : for any argument that is a symbolic link , use the metadata of the target link.

Purpose: Demostarte system programming ability
build with: gcc fcompare.c -o fcompare

fcompare <operation> <optional> file1 file2 file3..

******************************************************************************************/


/*
 Narrative

 Libraries I used.
 I utlized "stdio.h" so that I could use the printf() function, perror() and functions.
 I utlized "stdlib.h" so that I could use the exit() functionality
 I utlized "sys/stat.h" so that I could use stat and lstat so that I could find out the file size, file access time, file mofified time etc.
 I utlized "sys/types.h" so that I could use off_t which is a type used to represent file sizes('st_size' in the struct size and I could also use blkcnt_t which is used to count blocks ('st_blocks' in the struct stat)
 I utlized "unistd.h" so that I could handle symbolic links.
 I utlized "fcntl.h" which stands for file control, and it provides functions and macros for manipulating file descriptors.
 I used "time.h" because I use tim_t to record times, and retrieve current time using time(NULL).


 Variables:
 struct stat fileStat: i recorded fileStat to recrod if we should use stat or lstat.
 char* operation: I used operation to record what type of operation we are currently doing either "-a" or "-b" or "-m" etc.
 char buf[PATH_MAX]:
 time_t comparisonTime: To record to time to compare either greater or smaller depending on the presence of "-r"
 off_t comparisonSize: To record the size of the file
 blkcnt_t comparisonUsage: to record the block of file
 int reverseComparison: to record if we should reverse comparison based on the presence of "r" as optional argument.
 int followLinks: to record the if we should use stat or lstat based on the presence of "l" as optional arguemnt.
 char* selectedFile : to record the name of selected file, which matches the requirements.


 My process:
 1. Intial arguemnt Check: I first determined if optional arugments like "-r" or "-l" were present, setting 'reverseComparison'
 and 'followLinks' accordingly.
 2. File name Iteration and Starting point: The presence of optional arguments shifts teh starting index for processing file names.
 3. Operation Handling:
        * -a(last access time): uses 'fileStat.st_atime' for comparison.
        *  -b(birth time) : Attemps to use 'fileStat.st_birthtime' with #ifdef st_birthtime, but faced compatability issuses with Linux.
        *  -c(last status change): uses 'fileStat.st_ctime' considering 'reverseComparison' and 'followLinks'
        *  -m(last modification): Utilizes 'fileStat.st_mtime' based on 'reverseComparison' and 'followLinks'
        *  -s(Apparent size) : compares using 'fileStat.st_size'.
        *  -u(Disk usage) : relies on calculated disk usage ('filestat.st_blocks * 512)
        *

Reflections and challenges.
I had challenges considering if we get other cases where arguments might miss match but didn't have time to consider them.
One major challenge I was unable to solve was finding out the birth time of files as the error message said that birth time is
not available in linux and I was unable to get the birth time.






 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> /* Definiiotn of AT_* constants */
#include <limits.h>
#include<time.h>
int main(int argc, char* argv[]){

        if (argc < 3){
                fprintf(stderr, "Usage: %s <operation> file1 file 2..\n", argv[0]);
                return 1;
        }
        struct stat fileStat;
        char *operation = argv[1];
        char buf[PATH_MAX];
        //iichar* files[argc-2];
        time_t comparisonTime;
        off_t comparisonSize;
        blkcnt_t comparisonUsage;
        int reverseComparison= 0;
        int followLinks = 0;
        int haveOptional = 0;
        char *selectedFile = NULL;
        int opt;
        /*Check for -r and -l*/

        char* optionalArgs;
        /* check if we have optional required arguments */
        if (strcmp(argv[2], "-r")== 0 || strcmp(argv[2],"-l") == 0 || strcmp(argv[2], "-rl") == 0){
                optionalArgs=argv[2];
                haveOptional = 1;
                        if(strcmp(optionalArgs,"-r") == 0)/* incase of "-r" reverseComparison*/
                                reverseComparison = 1;
                        else if(strcmp(optionalArgs,"-l") == 0){ /* incase of "-l" use symbolic links */
                                followLinks = 1;
                                }else if (strcmp(optionalArgs, "-rl") == 0 || strcmp(optionalArgs,"-lr") == 0){
                                /* incase of both allow both*/
                                reverseComparison = 1;
                                followLinks = 1;
                        }

        }

        int start;

        if (reverseComparison == 1 ||followLinks == 1 ){/* if we have optional argument start from 3*/
                start = 3;
        }else{ /* else start from 2 */
                start  = 2;
        }

        //operation = argv[1];
        comparisonTime =reverseComparison ? 0: time(NULL);
        comparisonSize = reverseComparison ? 0: LONG_MAX;
        comparisonUsage = reverseComparison ? 0: LONG_MAX;

        //printf("CURRENT OPERATION: %s\n",operation);
        for (int i =start;i<argc;++i){

                //printf("CURR I:%d\n", i);
                //printf("Curr operation: %s\n", operation);
                int statResult = followLinks ? stat(argv[i], &fileStat) : lstat(argv[i], &fileStat);
                if(statResult < 0){
                        perror(argv[i]);// should I exit instead.
                        printf("I am here \n");
                        continue;
                }



                blkcnt_t diskUsage = fileStat.st_blocks * 512; // Each block 512 bytes.
             // if operation is "-a"
                if(strcmp(operation, "-a")== 0){
                       if ((reverseComparison && fileStat.st_atime >comparisonTime) || (!reverseComparison && fileStat.st_atime < comparisonTime)){
                        //printf("COMparison Time: %ld\n", comparisonTime);
                        comparisonTime = fileStat.st_atime;
                        //printf("ComparisonTime: %ld\n", comparisonTime);
                        selectedFile = argv[i];
                        }
                }
                /*if operation is "-b" */
                #ifdef st_birthtime
                if(strcmp(operation,"-b") == 0){
                       if ((reverseComparison && fileStat.st_birthtime > comparisonTime) || (!reverseComparison && fileStat.st_birthtime < comparisonTime)){
                        comparisonTime=fileStat.st_birthtime;
                        selectedFile=argv[i];
                        }
                }
                #endif
                /* if operation ius "-c"*/
                if(strcmp(operation, "-c") == 0){
                        if ((reverseComparison && fileStat.st_ctime > comparisonTime) || (!reverseComparison && fileStat.st_ctime < comparisonTime)){
                        //printf("Comparison Time: %ld\n", comparisonTime);
                        comparisonTime = fileStat.st_ctime;
                        //printf("Comparison Time: %ld\n", comparisonTime);
                        selectedFile = argv[i];
                        }
                }
                /* if operation is "-m" */
                if(strcmp(operation, "-m") ==0){
                       if((reverseComparison && fileStat.st_mtime > comparisonTime) || (!reverseComparison && fileStat.st_mtime < comparisonTime)){
                        //printf("comparison Time: %ld\n", comparisonTime);
                        comparisonTime = fileStat.st_mtime;
                        //printf("Comparison Time: %ld\n", comparisonTime);
                        selectedFile = argv[i];
                        }
                }
                /* if operation is "-s" */
                if(strcmp(operation, "-s") == 0){
                       if((reverseComparison && fileStat.st_size > comparisonSize) || (!reverseComparison && fileStat.st_size < comparisonSize)){
                        //printf("Comparison Size: %ld\n", comparisonSize);
                        comparisonSize = fileStat.st_size;
                        //printf("comparisonSize: %ld\n", comparisonSize);
                        selectedFile = argv[i];
                        }
                }
                /* if operation is "-u" */
                if(strcmp(operation, "-u") == 0){
                       if((reverseComparison && diskUsage > comparisonUsage) || (!reverseComparison && diskUsage < comparisonUsage)){
                        //printf("comparison Usage: %ld\n", comparisonUsage);
                        comparisonUsage = diskUsage;
                        //printf("comparison Usage: %ld\n", comparisonUsage);
                        selectedFile = argv[i];
                        }
                }
                /* if operation doesn't match any of the required operations */
                if(strcmp(operation,"-u") != 0  && strcmp(operation,"-s") != 0 && strcmp(operation,"-m") != 0 && strcmp(operation,"-c") != 0 && strcmp(operation,"-b") != 0 &&strcmp(operation,"-a") != 0 ){
                        printf("correct option not chosen\n");
                        break;

                }

                //elif argv[1] == 'b';
        }
        if(selectedFile != NULL){
                printf("Selected File: %s\n", selectedFile);
        }else{
                printf("No valid files were provided\n");
                return 1;
        }
        return 0;



}
