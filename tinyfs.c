#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

/*
 * Our tiny filesystem contains:
 *
 * /
 * └── hello
 *
 * The file exists only in RAM.
 * When the filesystem is unmounted, its data disappears.
 */

#define FILE_PATH "/hello"
#define MAX_DATA 4096

static char file_data[MAX_DATA] =
    "Hello from TinyFS!\n"
    "This file is stored only in RAM.\n";

static size_t file_size;
static int file_exists = 1;

static pthread_mutex_t fs_lock = PTHREAD_MUTEX_INITIALIZER;


/*
 * VFS-style hook:
 * "Tell me metadata about this path."
 *
 * Used by commands such as:
 *   ls -l
 *   stat
 *   cat
 *   test -e
 */
static int tinyfs_getattr(const char *path,
                          struct stat *stbuf,
                          struct fuse_file_info *fi)
{
    (void) fi;

    memset(stbuf, 0, sizeof(*stbuf));

    pthread_mutex_lock(&fs_lock);

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;

    } else if (strcmp(path, FILE_PATH) == 0 && file_exists) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = file_size;

    } else {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    pthread_mutex_unlock(&fs_lock);
    return 0;
}


/*
 * VFS-style hook:
 * "List entries inside this directory."
 *
 * Used by:
 *   ls
 *   find
 */
static int tinyfs_readdir(const char *path,
                          void *buf,
                          fuse_fill_dir_t filler,
                          off_t offset,
                          struct fuse_file_info *fi,
                          enum fuse_readdir_flags flags)
{
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

 filler(buf, ".", NULL, 0, 0);
filler(buf, "..", NULL, 0, 0);

    

    if (file_exists) {
        filler(buf, "hello", NULL, 0, 0);
    }

   pthread_mutex_lock(&fs_lock);
    return 0;
}


/*
 * VFS-style hook:
 * "A process wants to open this file."
 *
 * Used by:
 *   cat hello
 *   nano hello
 *   echo text > hello
 */
static int tinyfs_open(const char *path, struct fuse_file_info *fi)
{
    pthread_mutex_lock(&fs_lock);

    if (strcmp(path, FILE_PATH) != 0 || !file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    /*
     * This tiny filesystem supports normal read/write access.
     * Reject unsupported access modes only if needed.
     */
    (void) fi;

    pthread_mutex_unlock(&fs_lock);
    return 0;
}


/*
 * VFS-style hook:
 * "Copy bytes from the filesystem into the application's buffer."
 *
 * Used by:
 *   cat hello
 *   head hello
 *   cp hello somewhere
 */
static int tinyfs_read(const char *path,
                       char *buf,
                       size_t size,
                       off_t offset,
                       struct fuse_file_info *fi)
{
    (void) fi;

    pthread_mutex_lock(&fs_lock);

    if (strcmp(path, FILE_PATH) != 0 || !file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    if ((size_t) offset >= file_size) {
        pthread_mutex_unlock(&fs_lock);
        return 0;   // EOF
    }

    if (offset + size > file_size)
        size = file_size - offset;

    memcpy(buf, file_data + offset, size);

    pthread_mutex_unlock(&fs_lock);
    return (int) size;
}


/*
 * VFS-style hook:
 * "Copy bytes from the application's buffer into the filesystem."
 *
 * Used by:
 *   echo "new text" > hello
 *   printf "abc" >> hello
 *   editors saving a file
 */
static int tinyfs_write(const char *path,
                        const char *buf,
                        size_t size,
                        off_t offset,
                        struct fuse_file_info *fi)
{
    (void) fi;

    pthread_mutex_lock(&fs_lock);

    if (strcmp(path, FILE_PATH) != 0 || !file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    if ((size_t) offset >= MAX_DATA) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOSPC;
    }

    if (offset + size > MAX_DATA)
        size = MAX_DATA - offset;

    memcpy(file_data + offset, buf, size);

    if ((size_t)(offset + size) > file_size)
        file_size = offset + size;

    pthread_mutex_unlock(&fs_lock);
    return (int) size;
}


/*
 * VFS-style hook:
 * "Change file length."
 *
 * Commonly triggered by:
 *   echo "text" > hello
 *
 * The shell opens the file with truncation intent.
 * The old contents may need to become length 0 before writing new bytes.
 */
static int tinyfs_truncate(const char *path,
                           off_t new_size,
                           struct fuse_file_info *fi)
{
    (void) fi;

    if (new_size < 0 || new_size > MAX_DATA)
        return -EINVAL;

    pthread_mutex_lock(&fs_lock);

    if (strcmp(path, FILE_PATH) != 0 || !file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    if ((size_t)new_size > file_size) {
        memset(file_data + file_size, 0, new_size - file_size);
    }

    file_size = new_size;

    pthread_mutex_unlock(&fs_lock);
    return 0;
}


/*
 * VFS-style hook:
 * "Create a new file."
 *
 * Used by:
 *   touch hello
 *   echo text > hello
 *
 * This minimal filesystem supports only one possible file: /hello.
 */
static int tinyfs_create(const char *path,
                         mode_t mode,
                         struct fuse_file_info *fi)
{
    (void) mode;
    (void) fi;

    if (strcmp(path, FILE_PATH) != 0)
        return -EPERM;

    pthread_mutex_lock(&fs_lock);

    if (file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -EEXIST;
    }

    memset(file_data, 0, sizeof(file_data));
    file_size = 0;
    file_exists = 1;

    pthread_mutex_unlock(&fs_lock);
    return 0;
}


/*
 * VFS-style hook:
 * "Remove this directory entry."
 *
 * Used by:
 *   rm hello
 */
static int tinyfs_unlink(const char *path)
{
    if (strcmp(path, FILE_PATH) != 0)
        return -ENOENT;

    pthread_mutex_lock(&fs_lock);

    if (!file_exists) {
        pthread_mutex_unlock(&fs_lock);
        return -ENOENT;
    }

    file_exists = 0;
    file_size = 0;
    memset(file_data, 0, sizeof(file_data));

    pthread_mutex_unlock(&fs_lock);
    return 0;
}


/*
 * Register our filesystem operations.
 *
 * This table is the important part:
 * it tells libfuse which function handles each filesystem operation.
 */
static const struct fuse_operations tinyfs_ops = {
    .getattr  = tinyfs_getattr,
    .readdir  = tinyfs_readdir,
    .open     = tinyfs_open,
    .read     = tinyfs_read,
    .write    = tinyfs_write,
    .truncate = tinyfs_truncate,
    .create   = tinyfs_create,
    .unlink   = tinyfs_unlink,
};


int main(int argc, char *argv[])
{
    file_size = strlen(file_data);

    printf("Starting TinyFS...\n");
    printf("Mount it on a directory, for example: ./tinyfs mnt\n");

    return fuse_main(argc, argv, &tinyfs_ops, NULL);
}