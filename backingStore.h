#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include <stdio.h>

#define PAGE_SIZE 256

typedef struct {
    FILE *file;
} BackingStore;

// Initialize the Backing Store
void BackingStore_init(BackingStore *backing_store, const char *file_name);

// Free any resources allocated by the Backing Store
void BackingStore_free(BackingStore *backing_store);

// Read a page from the Backing Store
void BackingStore_read_page(BackingStore *backing_store, int page_number, unsigned char *buffer);

#endif // BACKINGSTORE_H
