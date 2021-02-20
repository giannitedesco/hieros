#!/usr/bin/python

KERNEL_VMA = 0xffffffff80000000
RAM_MAP = 0xffff800000000000
KERNEL_LMA = 0x100000

def pg(addr):
    print(f'Address {addr:16x} breakdown')
    print(f'L4 offset {(addr >> (12 + 9 + 9 + 9))& 0x1ff:x}')
    print(f'L3 offset {(addr >> (12 + 9 + 9))& 0x1ff:x}')
    print(f'L2 offset {(addr >> (12 + 9))& 0x1ff:x}')
    print(f'L1 offset {(addr >> 12)& 0x1ff:x}')
    print(f'pg offset {addr & 0xfff:x}')
    print()

def main():
    pg(KERNEL_VMA)
    pg(RAM_MAP)
    pg(KERNEL_LMA)

if __name__ == '__main__':
    main()
