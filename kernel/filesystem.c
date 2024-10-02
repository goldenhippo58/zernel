#include "filesystem.h"
#include "memory.h"
#include "string.h"
#include "vga.h"

#define FS_SEEK_SET 0
#define FS_SEEK_CUR 1
#define FS_SEEK_END 2

#define MAX_PATH_LENGTH 256
#define MAX_DIRECTORY_ENTRIES 16

static File files[MAX_FILES];
static int file_count = 0;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    int file_index;
    bool is_directory;
} DirectoryEntry;

typedef struct {
    DirectoryEntry entries[MAX_DIRECTORY_ENTRIES];
    int entry_count;
} Directory;

static Directory root_directory;

void fs_init() {
    vga_writestring("Initializing filesystem...\n");
    memset(files, 0, sizeof(files));
    memset(&root_directory, 0, sizeof(root_directory));
    file_count = 0;
    vga_writestring("Filesystem initialized. Max files: ");
    char max_files_str[10];
    int_to_string(MAX_FILES, max_files_str);
    vga_writestring(max_files_str);
    vga_writestring("\n");
}

int fs_create(const char* filename) {
    vga_writestring("Creating file: ");
    vga_writestring(filename);
    vga_writestring("\n");

    if (file_count >= MAX_FILES) {
        vga_writestring("Error: Maximum number of files reached\n");
        return -1;
    }

    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, filename) == 0) {
            vga_writestring("Error: File already exists\n");
            return -2;
        }
    }

    File* file = &files[file_count];
    strncpy(file->name, filename, MAX_FILENAME_LENGTH - 1);
    file->name[MAX_FILENAME_LENGTH - 1] = '\0';
    file->size = 0;
    file->data = kmalloc(MAX_FILE_SIZE);
    if (!file->data) {
        vga_writestring("Error: Failed to allocate memory for file\n");
        return -3;
    }

    // Add to root directory
    if (root_directory.entry_count < MAX_DIRECTORY_ENTRIES) {
        DirectoryEntry* entry = &root_directory.entries[root_directory.entry_count++];
        strncpy(entry->name, filename, MAX_FILENAME_LENGTH - 1);
        entry->name[MAX_FILENAME_LENGTH - 1] = '\0';
        entry->file_index = file_count;
        entry->is_directory = false;
    } else {
        vga_writestring("Error: Root directory is full\n");
        kfree(file->data);
        return -4;
    }

    vga_writestring("File created successfully\n");
    return file_count++;
}

int fs_write(const char* filename, const void* data, size_t size) {
    vga_writestring("Writing to file: ");
    vga_writestring(filename);
    vga_writestring("\n");

    File* file = fs_open(filename);
    if (!file) {
        vga_writestring("Error: File not found\n");
        return -1;
    }

    if (size > MAX_FILE_SIZE) {
        vga_writestring("Warning: Truncating file to maximum size\n");
        size = MAX_FILE_SIZE;
    }

    memcpy(file->data, data, size);
    file->size = size;

    vga_writestring("Write successful. Bytes written: ");
    char size_str[20];
    int_to_string(size, size_str);
    vga_writestring(size_str);
    vga_writestring("\n");

    return size;
}

int fs_read(const char* filename, void* buffer, size_t size) {
    vga_writestring("Reading from file: ");
    vga_writestring(filename);
    vga_writestring("\n");

    File* file = fs_open(filename);
    if (!file) {
        vga_writestring("Error: File not found\n");
        return -1;
    }

    if (size > file->size) {
        size = file->size;
    }

    memcpy(buffer, file->data, size);

    vga_writestring("Read successful. Bytes read: ");
    char size_str[20];
    int_to_string(size, size_str);
    vga_writestring(size_str);
    vga_writestring("\n");

    return size;
}

int fs_delete(const char* filename) {
    vga_writestring("Deleting file: ");
    vga_writestring(filename);
    vga_writestring("\n");

    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, filename) == 0) {
            kfree(files[i].data);
            if (i < file_count - 1) {
                files[i] = files[file_count - 1];
            }
            file_count--;

            // Remove from root directory
            for (int j = 0; j < root_directory.entry_count; j++) {
                if (strcmp(root_directory.entries[j].name, filename) == 0) {
                    if (j < root_directory.entry_count - 1) {
                        root_directory.entries[j] = root_directory.entries[root_directory.entry_count - 1];
                    }
                    root_directory.entry_count--;
                    break;
                }
            }

            vga_writestring("File deleted successfully\n");
            return 0;
        }
    }

    vga_writestring("Error: File not found\n");
    return -1;
}

void fs_list(char* buffer, size_t buffer_size) {
    vga_writestring("Listing files and directories...\n");
    size_t offset = 0;
    for (int i = 0; i < root_directory.entry_count && offset < buffer_size - 1; i++) {
        DirectoryEntry* entry = &root_directory.entries[i];
        int written = snprintf(buffer + offset, buffer_size - offset,
                               "%s %s (%u bytes)\n", 
                               entry->is_directory ? "DIR" : "FILE",
                               entry->name, 
                               entry->is_directory ? 0 : files[entry->file_index].size);
        if (written > 0) {
            offset += written;
        } else {
            break;
        }
    }
    buffer[offset] = '\0';
    vga_writestring("File and directory list generated\n");
}

File* fs_open(const char* filename) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, filename) == 0) {
            return &files[i];
        }
    }
    return NULL;
}

void fs_close(File* file) {
    // In this simple implementation, we don't need to do anything
    (void)file;
}

int fs_seek(File* file, int offset, int origin) {
    if (!file) {
        return -1;
    }

    uint32_t new_position;
    switch (origin) {
        case FS_SEEK_SET:
            new_position = (uint32_t)offset;
            break;
        case FS_SEEK_CUR:
            new_position = (uint32_t)offset; // Assume current position is always at start
            break;
        case FS_SEEK_END:
            if (offset > 0) {
                return -1; // Can't seek past end of file
            }
            new_position = file->size + (uint32_t)offset;
            break;
        default:
            return -1;
    }

    if (new_position > file->size) {
        return -1;
    }

    return (int)new_position;
}

int fs_tell(File* file) {
    if (!file) {
        return -1;
    }
    return 0; // In this simple implementation, we always return 0
}

int fs_mkdir(const char* dirname) {
    vga_writestring("Creating directory: ");
    vga_writestring(dirname);
    vga_writestring("\n");

    if (root_directory.entry_count >= MAX_DIRECTORY_ENTRIES) {
        vga_writestring("Error: Root directory is full\n");
        return -1;
    }

    for (int i = 0; i < root_directory.entry_count; i++) {
        if (strcmp(root_directory.entries[i].name, dirname) == 0) {
            vga_writestring("Error: Directory or file already exists\n");
            return -2;
        }
    }

    DirectoryEntry* new_dir = &root_directory.entries[root_directory.entry_count++];
    strncpy(new_dir->name, dirname, MAX_FILENAME_LENGTH - 1);
    new_dir->name[MAX_FILENAME_LENGTH - 1] = '\0';
    new_dir->is_directory = true;
    new_dir->file_index = -1;  // Directories don't have a file index

    vga_writestring("Directory created successfully\n");
    return 0;
}
