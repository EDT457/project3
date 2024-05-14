#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TLB.h"
#include "PageTable.h"
#include "PhysicalMemory.h"
#include "BackingStore.h"
#include "utils.h"

#define DEFAULT_FRAMES 256
#define DEFAULT_PRA "fifo"
#define PAGE_SIZE 256
#define OFFSET_MASK 0xFF

void print_usage() {
    printf("Usage: memSim <reference-sequence-file.txt> <FRAMES> <PRA>\n");
}

int extract_page_number(int logical_address) {
    return (logical_address >> 8) & 0xFF;
}

int extract_offset(int logical_address) {
    return logical_address & OFFSET_MASK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char *ref_file = argv[1];
    int frames = (argc > 2) ? atoi(argv[2]) : DEFAULT_FRAMES;
    char *pra = (argc > 3) ? argv[3] : DEFAULT_PRA;

    TLB tlb;
    PageTable page_table;
    PhysicalMemory physical_memory;
    BackingStore backing_store;

    TLB_init(&tlb);
    PageTable_init(&page_table);
    PhysicalMemory_init(&physical_memory, frames);
    BackingStore_init(&backing_store, "BACKING_STORE.bin");

    FILE *file = fopen(ref_file, "r");
    if (!file) {
        perror("Failed to open reference file");
        return 1;
    }

    int logical_address;
    int page_faults = 0;
    int tlb_hits = 0;
    int tlb_misses = 0;
    int total_addresses = 0;
    unsigned char buffer[PAGE_SIZE];

    while (fscanf(file, "%d", &logical_address) == 1) {
        total_addresses++;
        int page_number = extract_page_number(logical_address);
        int offset = extract_offset(logical_address);

        int frame_number = TLB_lookup(&tlb, page_number);
        if (frame_number == -1) {
            tlb_misses++;
            frame_number = PageTable_lookup(&page_table, page_number);
            if (frame_number == -1) {
                page_faults++;
                frame_number = PhysicalMemory_get_next_frame(&physical_memory, pra);
                BackingStore_read_page(&backing_store, page_number, buffer);
                PhysicalMemory_write_frame(&physical_memory, frame_number, buffer);
                PageTable_update(&page_table, page_number, frame_number);
            }
            TLB_add_entry(&tlb, page_number, frame_number);
        } else {
            tlb_hits++;
        }

        int physical_address = (frame_number * PAGE_SIZE) + offset;
        int value = physical_memory.memory[physical_address];

        printf("%d,%d,%d,", logical_address, value, frame_number);
        PhysicalMemory_read_frame(&physical_memory, frame_number, buffer);
        for (int i = 0; i < PAGE_SIZE; i++) {
            printf("%02x", buffer[i]);
        }
        printf("\n");
    }

    fclose(file);

    double page_fault_rate = ((double)page_faults / total_addresses) * 100.0;
    double tlb_miss_rate = ((double)tlb_misses / total_addresses) * 100.0;

    printf("Total number of page faults: %d, Page fault rate: %.2f%%\n", page_faults, page_fault_rate);
    printf("Total number of TLB hits: %d, Total number of TLB misses: %d, TLB miss rate: %.2f%%\n", tlb_hits, tlb_misses, tlb_miss_rate);

    TLB_free(&tlb);
    PageTable_free(&page_table);
    PhysicalMemory_free(&physical_memory);
    BackingStore_free(&backing_store);

    return 0;
}
