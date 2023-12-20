/***********************************************************************
name: Kevin Kinney
	readable -- recursively count readable files.
description:	
	See CS 360 Files and Directories lectures for details.
***********************************************************************/

/* Includes and definitions */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/limits.h> // for PATH_MAX
#include <string.h>

/**********************************************************************
Take as input some path. Recursively count the files that are readable.
If a file is not readable, do not count it. Return the negative value
of any error codes you may get. However, consider what errors you want
to respond to.
**********************************************************************/

int recursiveReadable(char* inputPath) {
	
	// get file info
	struct stat s;
	int fileInf = stat(inputPath, &s); 
	
	if (fileInf == 0) {
		// is it a regular file that the user can read?
		if (S_ISREG(s.st_mode) && (s.st_mode & S_IRUSR)) {
			return 1;
		} else {
			// is it a directory that the user can read?
			if (S_ISDIR(s.st_mode) && (s.st_mode & S_IRUSR)) {
				// open directory
				DIR *dirp = opendir(inputPath);

				// error check
				if (dirp == NULL) {
					write(2, strerror(errno), strlen(strerror(errno)));
				}

				// var inits
				char newPath[PATH_MAX];
				memset(newPath, '\0', PATH_MAX);
				strcpy(newPath, inputPath);
				int count = 0;

				errno = 0;
				struct dirent *entry;

				// read directory contents 
				while ((entry = readdir(dirp)) != NULL) {
					// error check
					if (errno != 0) {
						write(2, strerror(errno), strlen(strerror(errno)));
					}

					// ignore current and previous directory
					if (((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))) { 
						strcat(newPath, "/");

						// recusive call
						count += recursiveReadable(strcat(newPath, entry->d_name));

						// resert inputPath val
						memset(newPath, '\0', PATH_MAX);
						strcpy(newPath, inputPath);
					}
				}

				closedir(dirp);
				return count;
			}

			// for special file 
			return 0;
		}
	} else {
		// error if stat fails
		write(2, strerror(errno), strlen(strerror(errno)));
		return -errno;
	}

	return 0;
}

int readable(char* inputPath) {
	
	if (inputPath == NULL) {
		// set path to cwd
		inputPath = getcwd(inputPath, 128);
	}

	// check if user has perms to read
	if (access(inputPath, R_OK) == -1) {
		write(2, strerror(errno), strlen(strerror(errno)));
		return -errno;
	}

    return recursiveReadable(inputPath);
}
