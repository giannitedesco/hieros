#pragma once

#define KERNEL32_CS 1
#define KERNEL_DS 2
#define KERNEL_CS 3

#define __KERNEL32_CS (KERNEL32_CS << 3)
#define __KERNEL_DS (KERNEL_DS << 3)
#define __KERNEL_CS (KERNEL_CS << 3)
