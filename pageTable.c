#include "PageTable.h"

void PageTable_init(PageTable *page_table) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table->entries[i].frame_number = -1;
        page_table->entries[i].loaded = 0;
    }
}

int PageTable_lookup(PageTable *page_table, int page_number) {
    if (page_table->entries[page_number].loaded) {
        return page_table->entries[page_number].frame_number;
    }
    return -1;
}

void PageTable_update(PageTable *page_table, int page_number, int frame_number) {
    page_table->entries[page_number].frame_number = frame_number;
    page_table->entries[page_number].loaded = 1;
}
