#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>

#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 4096
#define MAX_FILES 64

// Custom seek constants
#define FS_SEEK_SET 0
#define FS_SEEK_CUR 1
#define FS_SEEK_END 2

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint8_t* data;
} File;

void fs_init();
int fs_create(const char* filename);
int fs_write(const char* filename, const void* data, size_t size);
int fs_read(const char* filename, void* buffer, size_t size);
int fs_delete(const char* filename);
void fs_list(char* buffer, size_t buffer_size);
File* fs_open(const char* filename);
void fs_close(File* file);
int fs_seek(File* file, int offset, int origin);
int fs_tell(File* file);

#endif // FILESYSTEM_H
