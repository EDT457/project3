import sys

DEFAULT_FRAMES = 256
DEFAULT_PRA = 'fifo'
PAGE_SIZE = 256
OFFSET_MASK = 0xFF

class TLB:
    def __init__(self, size=16):
        self.size = size
        self.entries = []
        self.next_index = 0

    def lookup(self, logical_page_number):
        for entry in self.entries:
            if entry['logical_page_number'] == logical_page_number:
                return entry['physical_frame_number']
        return -1

    def add_entry(self, logical_page_number, physical_frame_number):
        if len(self.entries) < self.size:
            self.entries.append({'logical_page_number': logical_page_number, 'physical_frame_number': physical_frame_number})
        else:
            self.entries[self.next_index] = {'logical_page_number': logical_page_number, 'physical_frame_number': physical_frame_number}
            self.next_index = (self.next_index + 1) % self.size

class PageTable:
    def __init__(self, size=256):
        self.size = size
        self.entries = [{'frame_number': -1, 'loaded': 0} for _ in range(size)]

    def lookup(self, page_number):
        entry = self.entries[page_number]
        if entry['loaded']:
            return entry['frame_number']
        return -1

    def update(self, page_number, frame_number):
        self.entries[page_number] = {'frame_number': frame_number, 'loaded': 1}

class PhysicalMemory:
    def __init__(self, total_frames):
        self.total_frames = total_frames
        self.frame_size = PAGE_SIZE
        self.memory = [bytearray(self.frame_size) for _ in range(total_frames)]
        self.frame_usage = [0] * total_frames
        self.next_frame = 0

    def read_frame(self, frame_number):
        return self.memory[frame_number]

    def write_frame(self, frame_number, data):
        self.memory[frame_number][:] = data
        self.frame_usage[frame_number] = 1

    def get_next_frame(self, pra):
        if 0 in self.frame_usage:
            return self.frame_usage.index(0)
        if pra == 'fifo':
            frame = self.next_frame
            self.next_frame = (self.next_frame + 1) % self.total_frames
            return frame
        elif pra == 'lru':
            # Implement LRU page replacement here
            pass
        elif pra == 'opt':
            # Implement OPT page replacement here
            pass
        return -1  # Should not reach here if PRA is implemented

class BackingStore:
    def __init__(self, file_name):
        self.file_name = file_name

    def read_page(self, page_number):
        with open(self.file_name, 'rb') as f:
            f.seek(page_number * PAGE_SIZE)
            return f.read(PAGE_SIZE)

def extract_page_number(logical_address):
    return (logical_address >> 8) & 0xFF

def extract_offset(logical_address):
    return logical_address & OFFSET_MASK

def print_usage():
    print("Usage: memSim <reference-sequence-file.txt> <FRAMES> <PRA>")

def main():
    if len(sys.argv) < 2:
        print_usage()
        return

    ref_file = sys.argv[1]
    frames = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_FRAMES
    pra = sys.argv[3] if len(sys.argv) > 3 else DEFAULT_PRA

    tlb = TLB()
    page_table = PageTable()
    physical_memory = PhysicalMemory(frames)
    backing_store = BackingStore('BACKING_STORE.bin')

    with open(ref_file, 'r') as file:
        addresses = [int(line.strip()) for line in file]

    page_faults = 0
    tlb_hits = 0
    tlb_misses = 0

    for logical_address in addresses:
        page_number = extract_page_number(logical_address)
        offset = extract_offset(logical_address)

        frame_number = tlb.lookup(page_number)
        if frame_number == -1:
            tlb_misses += 1
            frame_number = page_table.lookup(page_number)
            if frame_number == -1:
                page_faults += 1
                frame_number = physical_memory.get_next_frame(pra)
                page_data = backing_store.read_page(page_number)
                physical_memory.write_frame(frame_number, page_data)
                page_table.update(page_number, frame_number)
            tlb.add_entry(page_number, frame_number)
        else:
            tlb_hits += 1

        physical_address = (frame_number * PAGE_SIZE) + offset
        value = physical_memory.memory[frame_number][offset]

        frame_data = physical_memory.read_frame(frame_number)
        frame_data_hex = ''.join(f'{byte:02x}' for byte in frame_data)

        print(f"{logical_address},{value},{frame_number},{frame_data_hex}")

    total_addresses = len(addresses)
    page_fault_rate = (page_faults / total_addresses) * 100
    tlb_miss_rate = (tlb_misses / total_addresses) * 100

    print(f"Total number of page faults: {page_faults}, Page fault rate: {page_fault_rate:.2f}%")
    print(f"Total number of TLB hits: {tlb_hits}, Total number of TLB misses: {tlb_misses}, TLB miss rate: {tlb_miss_rate:.2f}%")

if __name__ == "__main__":
    main()
