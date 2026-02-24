#ifndef XFS_H
#define XFS_H

#define MAX_FILES 8
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 468 // 32(name) + 4(size) + 4(used) + 4(unused) + 468(data) = 512

typedef struct {
    char name[MAX_FILENAME];
    int size;
    int used;
    int reserved; 
    char data[MAX_FILE_SIZE];
} xfs_file_t;

extern xfs_file_t filesystem[MAX_FILES];

void xfs_init();
int xfs_create(char* name);
int xfs_write(char* name, char* content);
char* xfs_read(char* name);
void xfs_list();
void xfs_sync();
void xfs_load();
void xfs_remove(char* name);

#endif
