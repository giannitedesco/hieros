#include <hieros/asm.h>

.balign 4096
ASM_VAR_BEGIN(apboot_blob)
.incbin "obj/apboot.bin"
ASM_VAR_END(apboot_blob)
