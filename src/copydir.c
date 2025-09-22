/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 *
 */

#ifdef _WIN32
#    include <windows.h>
#else
#    define POSIX_C_SOURCE 199309L
#    include <unistd.h>
#    include <sys/sendfile.h>
#    include <linux/limits.h>
#    include <glob.h>
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <errno.h>
#    include <string.h>
#    include <libgen.h>
#    include <stdlib.h>
#endif
#include <stdio.h>
int main(int argc, char** argv)
{
#ifdef _WIN32

    char filename[MAX_PATH], *f;
    HANDLE handle;
    WIN32_FIND_DATA blk;
    if (argc != 3)
    {
        printf("wrong number of args");
        return 1;
    }
    strcpy(filename, argv[1]);
    while ((f = strchr(filename, '%')))
        *f = '*';
    memset(&blk, 0, sizeof(blk));
    if ((handle = FindFirstFile(filename, &blk)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(blk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char newFile[MAX_PATH];
                char existingFile[MAX_PATH], *p;
                strcpy(newFile, argv[2]);
                if (newFile[strlen(newFile) - 1] != '\\')
                    strcat(newFile, "\\");
                strcat(newFile, blk.cFileName);
                strcpy(existingFile, filename);
                p = strrchr(existingFile, '\\');
                if (p)
                    p++;
                else
                    p = existingFile;
                strcpy(p, blk.cFileName);
                printf("copying %s to %s\n", existingFile, newFile);
                CopyFile(existingFile, newFile, FALSE);
            }
        } while (FindNextFile(handle, &blk));
        FindClose(handle);
        printf("Ok");
        return 0;
    }
#else
    char filename[PATH_MAX], *f;
    if (argc != 3)
    {
        printf("wrong number of args");
        return 1;
    }
    strcpy(filename, argv[1]);
    while ((f = strchr(filename, '%')))
        *f = '*';

    glob_t glob_fils;
    int retval = glob(filename, GLOB_MARK | GLOB_TILDE, NULL, &glob_fils);
    if (!retval)
    {
        for (size_t fil = 0; fil < glob_fils.gl_pathc; fil++)
        {
            char* filname = glob_fils.gl_pathv[fil];
            size_t len = strlen(filname);
            // if there's a trailing slash the file is a directory and can be ignored per the WINDOWs behavior
            if (len < PATH_MAX || filename[len - 1] == '/')
            {
                char newFile[PATH_MAX];
                char filname_COPY[PATH_MAX];
                strcpy(filname_COPY, filname);
                strcpy(newFile, argv[2]);
                size_t len = strlen(newFile);
                if (len < PATH_MAX)
                {
                    strcat(newFile, "/");
                }
                strcat(newFile, basename(filname_COPY));
                struct stat filstat = {0};
                stat(filname, &filstat);

                size_t filsize = filstat.st_size;

                int srcfd = open(filname, O_RDWR);
                if (srcfd < 0)
                {
                    int errn = errno;
                    char* err = strerror(errn);
                    fprintf(stderr, "Srcfd is < 0, err: %d, %s\n", errno, err);
                    exit(1);
                }
                int destfd = open(newFile, O_CREAT | O_RDWR, filstat.st_mode);
                if (destfd < 0)
                {
                    int errn = errno;
                    char* err = strerror(errn);
                    fprintf(stderr, "destfd is < 0, err: %d, %s\n", errn, err);
                    exit(1);
                }
                ssize_t ret = sendfile(destfd, srcfd, NULL, filsize);
                if (ret < 0)
                {
                    int err = errno;
                    fprintf(stderr, "sendfile failed with: %d, %s\n", err, strerror(err));
                    exit(1);
                }
                close(srcfd);
                close(destfd);
            }
        }
        printf("Ok\n");
        return 0;
    }
#endif
    printf("No files\n");
    return 1;
}