#include "xfs.h"
#include "ata.h"
#include "../kernel/gui.h"

xfs_file_t filesystem[MAX_FILES];

void xfs_init() {
    for (int i = 0; i < MAX_FILES; i++) filesystem[i].used = 0;
}

int xfs_create(char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used == 0) {
            int j = 0;
            while (name[j] != '\0' && j < MAX_FILENAME - 1) {
                filesystem[i].name[j] = name[j];
                j++;
            }
            filesystem[i].name[j] = '\0';
            filesystem[i].used = 1;
            filesystem[i].size = 0;
            return i;
        }
    }
    return -1;
}

int xfs_write(char* name, char* content) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && strcmp(filesystem[i].name, name) == 0) {
            int j = 0;
            while (content[j] != '\0' && j < MAX_FILE_SIZE - 1) {
                filesystem[i].data[j] = content[j];
                j++;
            }
            filesystem[i].data[j] = '\0';
            filesystem[i].size = j;
            return 0;
        }
    }
    return -1;
}

char* xfs_read(char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && strcmp(filesystem[i].name, name) == 0) {
            return filesystem[i].data;
        }
    }
    return 0;
}

void xfs_list() {
    kprint("--- XFS Files ---\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used) {
            kprint("- "); kprint(filesystem[i].name); kprint("\n");
        }
    }
}

void xfs_remove(char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && strcmp(filesystem[i].name, name) == 0) {
            filesystem[i].used = 0;
            xfs_sync();
            return;
        }
    }
}

void xfs_sync() {
    for (int i = 0; i < MAX_FILES; i++) {
        ata_write_sector(300 + i, (unsigned short*)&filesystem[i]);
    }
}

void xfs_load() {
    for (int i = 0; i < MAX_FILES; i++) {
        ata_read_sector(300 + i, (unsigned short*)&filesystem[i]);
    }
}
