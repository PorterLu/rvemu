#include "types.h"

#define EI_NIDENT 16      // size of e_ident
#define ELFMAG "\177ELF"  // \177 to avoid 'E' misinterpret as hex

#define EM_RISCV 243      // MACHINE NUMBER

#define EI_CLASS     4    // offset of class in e_ident
#define ELFCLASSNONE 0    
#define ELFCLASS32   1
#define ELFCLASS64   2    // the type we need in this emulator
#define ELFCLASSNUM  3

#define PT_LOAD 1

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

typedef struct {
    u8 e_ident[EI_NIDENT];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u64 e_entry;          // program entry
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} elf64_ehdr_t;

typedef struct {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
} elf64_phdr_t;

