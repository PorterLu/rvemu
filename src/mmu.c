#include "rvemu.h"

static void load_phdr(elf64_phdr_t *phdr, elf64_ehdr_t *ehdr, i64 i, FILE *file) {
  if (fseek(file, ehdr->e_phoff + ehdr->e_phentsize * i, SEEK_SET) != 0) {
    fatal("seek file failed");
  }

  if (fread((void*) phdr, 1, sizeof(elf64_phdr_t), file) != sizeof(elf64_phdr_t)) {
    fatal("file too small");
  }
}

static int flags_to_mmap_prot(u32 flags) {
  return (flags & PF_R ? PROT_READ : 0) |
          (flags & PF_W ? PROT_WRITE : 0) |
          (flags & PF_X ? PROT_EXEC: 0);
}

static void mmu_load_segment(mmu_t *mmu, elf64_phdr_t *phdr, int fd) {
  // get system page size, which is normally 4KB.
  int page_size = getpagesize();
  
  // get the program segment offset in the elf file
  u64 offset = phdr->p_offset;

  // get the program segment virtual address, and convert it to an address on the host, which base is 0x088800000000
  u64 vaddr = TO_HOST(phdr->p_vaddr);

  // we use mmap to map our program to host, so the address should be aligned.
  // |----4k----|----4k----|----4k----|----4k----|
  // |                                           |
  // |          |-align-@----file------@-mem-@   |
  // |          |-------newfilesz------|         |
  // |          |-----------newmemsz---------|   |
  // ---------------------------------------------
  u64 aligned_vaddr = ROUNDDOWN(vaddr, page_size);

  // compensate the size we add for aligning
  u64 filesz = phdr->p_filesz + (vaddr - aligned_vaddr);
  u64 memsz = phdr->p_memsz + (vaddr - aligned_vaddr);

  // transfer flag in ELF to flag in PAGE
  int prot = flags_to_mmap_prot(phdr->p_flags);

  // mapping the address of this segment
  u64 addr = (u64)mmap((void*)aligned_vaddr, filesz, prot, MAP_PRIVATE | MAP_FIXED,
                        fd, ROUNDDOWN(offset, page_size));
  assert(addr == aligned_vaddr);

  // on this above figure case, remaining_bss = 0
  u64 remaining_bss = ROUNDUP(memsz, page_size) - ROUNDUP(filesz, page_size);
  if (remaining_bss > 0) {
    u64 addr = (u64)mmap((void*)(aligned_vaddr + ROUNDUP(filesz, page_size)), 
            remaining_bss, prot, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    assert(addr == aligned_vaddr + ROUNDUP(filesz, page_size));
  }

  // host_alloc means the init value where can be allocated as heap memory
  mmu->host_alloc = MAX(mmu->host_alloc, (aligned_vaddr + ROUNDUP(memsz, page_size)));

  // base is const value, alloc is dynamic value, and we convert them to the host_addr
  mmu->base = mmu->alloc = TO_GUEST(mmu->host_alloc);
}

void mmu_load_elf(mmu_t *mmu, int fd) {
  u8 buf[sizeof(elf64_ehdr_t)];
  FILE *file = fdopen(fd, "rb");
  if (fread(buf, 1, sizeof(elf64_ehdr_t), file) != sizeof(elf64_ehdr_t)) {
    fatal("file too small");
  }

  elf64_ehdr_t *ehdr = (elf64_ehdr_t *)buf;

  //check elf magic
  if (*(u32 *) ehdr != *(u32 *)ELFMAG) {
    fatal("bad elf file");
  }

  //we use riscv64 machine program
  if (ehdr->e_machine != EM_RISCV || ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
    fatal("only riscv64 elf file is supported");
  }

  //set mmu entry
  mmu->entry = (u64)ehdr->e_entry;

  elf64_phdr_t phdr;
  for(i64 i = 0; i < ehdr->e_phnum; i++) {
    load_phdr(&phdr, ehdr, i, file);

    if (phdr.p_type == PT_LOAD) {
      mmu_load_segment(mmu, &phdr, fd);
    }
  }
  /* program header || offset || filesize || memsize || virtAddr || flags || align*/
}
