#ifndef PAGETABLE_H
#define PAGETABLE_H

#define PAGE_TABLE_SIZE 256

typedef struct {
    int frame_number;
    int loaded;
} PageTableEntry;

typedef struct {
    PageTableEntry entries[PAGE_TABLE_SIZE];
} PageTable;

// Initialize
void PageTable_init(PageTable *page_table);

// Lookup a page number in the Page Table
// Returns the physical frame number if found and loaded, otherwise -1
int PageTable_lookup(PageTable *page_table, int page_number);

// Update the Page Table with a new page to frame mapping
void PageTable_update(PageTable *page_table, int page_number, int frame_number);

#endif
