#include "TLB.h"
#include <stdlib.h>

void TLB_init(TLB *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb->entries[i].logical_page_number = -1;
        tlb->entries[i].physical_frame_number = -1;
    }
    tlb->next_index = 0;
}

int TLB_lookup(TLB *tlb, int logical_page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].logical_page_number == logical_page_number) {
            return tlb->entries[i].physical_frame_number;
        }
    }
    return -1; // Not found
}

void TLB_add_entry(TLB *tlb, int logical_page_number, int physical_frame_number) {
    tlb->entries[tlb->next_index].logical_page_number = logical_page_number;
    tlb->entries[tlb->next_index].physical_frame_number = physical_frame_number;
    tlb->next_index = (tlb->next_index + 1) % TLB_SIZE;
}
