#ifndef TLB_H
#define TLB_H

#define TLB_SIZE 16

typedef struct {
    int logical_page_number;
    int physical_frame_number;
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
    int next_index;
} TLB;

// Initialize
void TLB_init(TLB *tlb);

// Free any resources allocated by the TLB
void TLB_free(TLB *tlb);

// Lookup a logical page number in the TLB
// Returns the physical frame number if found, otherwise -1
int TLB_lookup(TLB *tlb, int logical_page_number);

// Add a new entry to the TLB
// Uses FIFO replacement if the TLB is full
void TLB_add_entry(TLB *tlb, int logical_page_number, int physical_frame_number);

#endif
