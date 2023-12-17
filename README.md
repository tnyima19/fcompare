# This Project was created so that I could compare date of modifications of files that I wanted and this is the man page of the system program.

## NAME
fcompare -- compare various properties of a list of files
## SYNOPSIS
### fcompare <operation> [-rl] file file ...
where <operation> is exactly one of:
-a -b|-c|-m|-s|-u
## DESCRIPTION
Given the list of file pathnames on the command line, this outputs the name of the file in this list whose
metadata is least in the ordering specified by the operation. If any file is a symbolic link, it uses the metadata
of the link itself. The operations determine the metadata by which the files are compared, as follows:

-a By their times of last access
-b By their birth times
-c By their times of last status change
-m By their times of last modification
-s By their apparent size
-u By their disk usage

At most one of these operations can be specified. If more than one is specified, it is an error and the command
terminates without any output, setting its status to 1. The apparent size is the number of bytes reportedly
used, including any file holes. The disk usage is the total number of bytes allocated on disk to the file. For
any file argument whose birth time is not available, that file’s name is reported on standard error to be
unavailable for comparison.

## OPTIONS
-r Reverses the sense of the comparison, so that the command outputs the name of the file whose
metadata is greatest in the ordering rather than least. Multiple occurrences of -r on the command
line are treated as a single occurrence.
-l For any argument that is a symbolic link, use the metadata of the target of the link

## OUTPUT
The command outputs the pathname of the file identified by the preceding operations and options,
on standard output
