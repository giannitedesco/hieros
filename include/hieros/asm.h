#pragma once

#define ASM_LF ;
#define ASM_ALIGNED .align 4, 0x90
#define ASM_GLOBAL(name) .globl name
#define ASM_WEAK(name) .weak name
#define ASM_LOCAL(name)

#define ASM_SYM_START(_type, _name, _linkage, _align...) \
	_align ASM_LF \
	.type _name, _type ASM_LF \
	_linkage(_name) ASM_LF \
	_name:

#define ASM_SYM_END(_name) \
	.size _name, . - _name ASM_LF

#define ASM_FUNC_BEGIN(_name) \
	ASM_SYM_START(@function, _name, ASM_GLOBAL, ASM_ALIGNED)

#define ASM_LOCAL_FUNC_BEGIN(_name) \
	ASM_SYM_START(@function, _name, ASM_LOCAL, ASM_ALIGNED)

#define ASM_VAR_BEGIN(_name) \
	ASM_SYM_START(@object, _name, ASM_GLOBAL)

#define ASM_LOCAL_VAR_BEGIN(_name) \
	ASM_SYM_START(@object, _name, ASM_LOCAL)

#define ASM_VAR_END(_name) \
	ASM_SYM_END(_name)

#define ASM_FUNC_END(_name) \
	ASM_SYM_END(_name)

#ifndef __ASM__
void hang(void);
#endif
