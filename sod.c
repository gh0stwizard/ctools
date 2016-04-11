/*-
 * Copyright (c) 2016 Vitaliy Tokarev. All rights reserved.
 * Copyright (c) 2003 David O'Brien.  All rights reserved.
 * Copyright (c) 2001 Jake Burkholder
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#if defined(__linux__)
#include <elf.h>
#elif defined(__FreeBSD__)
#include <sys/elf32.h>
#include <sys/elf64.h>
#include <sys/endian.h>
#else
#error unsupported OS
#endif
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	elf_get_addr    elf_get_quad
#define	elf_get_off     elf_get_quad
#define	elf_get_size    elf_get_quad

enum elf_member {
	D_TAG = 1, D_PTR, D_VAL,

	E_CLASS, E_DATA, E_OSABI, E_TYPE, E_MACHINE, E_VERSION, E_ENTRY,
	E_PHOFF, E_SHOFF, E_FLAGS, E_EHSIZE, E_PHENTSIZE, E_PHNUM, E_SHENTSIZE,
	E_SHNUM, E_SHSTRNDX,

	N_NAMESZ, N_DESCSZ, N_TYPE,

	P_TYPE, P_OFFSET, P_VADDR, P_PADDR, P_FILESZ, P_MEMSZ, P_FLAGS,
	P_ALIGN,

	SH_NAME, SH_TYPE, SH_FLAGS, SH_ADDR, SH_OFFSET, SH_SIZE, SH_LINK,
	SH_INFO, SH_ADDRALIGN, SH_ENTSIZE,

	ST_NAME, ST_VALUE, ST_SIZE, ST_INFO, ST_SHNDX,

	R_OFFSET, R_INFO,

	RA_OFFSET, RA_INFO, RA_ADDEND
};

typedef enum elf_member elf_member_t;

static int elf32_offsets[] = {
	0,

	offsetof(Elf32_Dyn, d_tag), offsetof(Elf32_Dyn, d_un.d_ptr),
	offsetof(Elf32_Dyn, d_un.d_val),

	offsetof(Elf32_Ehdr, e_ident[EI_CLASS]),
	offsetof(Elf32_Ehdr, e_ident[EI_DATA]),
	offsetof(Elf32_Ehdr, e_ident[EI_OSABI]),
	offsetof(Elf32_Ehdr, e_type), offsetof(Elf32_Ehdr, e_machine),
	offsetof(Elf32_Ehdr, e_version), offsetof(Elf32_Ehdr, e_entry),
	offsetof(Elf32_Ehdr, e_phoff), offsetof(Elf32_Ehdr, e_shoff),
	offsetof(Elf32_Ehdr, e_flags), offsetof(Elf32_Ehdr, e_ehsize),
	offsetof(Elf32_Ehdr, e_phentsize), offsetof(Elf32_Ehdr, e_phnum),
	offsetof(Elf32_Ehdr, e_shentsize), offsetof(Elf32_Ehdr, e_shnum),
	offsetof(Elf32_Ehdr, e_shstrndx),

	offsetof(Elf_Note, n_namesz), offsetof(Elf_Note, n_descsz),
	offsetof(Elf_Note, n_type),

	offsetof(Elf32_Phdr, p_type), offsetof(Elf32_Phdr, p_offset),
	offsetof(Elf32_Phdr, p_vaddr), offsetof(Elf32_Phdr, p_paddr),
	offsetof(Elf32_Phdr, p_filesz), offsetof(Elf32_Phdr, p_memsz),
	offsetof(Elf32_Phdr, p_flags), offsetof(Elf32_Phdr, p_align),

	offsetof(Elf32_Shdr, sh_name), offsetof(Elf32_Shdr, sh_type),
	offsetof(Elf32_Shdr, sh_flags), offsetof(Elf32_Shdr, sh_addr),
	offsetof(Elf32_Shdr, sh_offset), offsetof(Elf32_Shdr, sh_size),
	offsetof(Elf32_Shdr, sh_link), offsetof(Elf32_Shdr, sh_info),
	offsetof(Elf32_Shdr, sh_addralign), offsetof(Elf32_Shdr, sh_entsize),

	offsetof(Elf32_Sym, st_name), offsetof(Elf32_Sym, st_value),
	offsetof(Elf32_Sym, st_size), offsetof(Elf32_Sym, st_info),
	offsetof(Elf32_Sym, st_shndx),

	offsetof(Elf32_Rel, r_offset), offsetof(Elf32_Rel, r_info),

	offsetof(Elf32_Rela, r_offset), offsetof(Elf32_Rela, r_info),
	offsetof(Elf32_Rela, r_addend)
};

static int elf64_offsets[] = {
	0,

	offsetof(Elf64_Dyn, d_tag), offsetof(Elf64_Dyn, d_un.d_ptr),
	offsetof(Elf64_Dyn, d_un.d_val),

	offsetof(Elf32_Ehdr, e_ident[EI_CLASS]),
	offsetof(Elf32_Ehdr, e_ident[EI_DATA]),
	offsetof(Elf32_Ehdr, e_ident[EI_OSABI]),
	offsetof(Elf64_Ehdr, e_type), offsetof(Elf64_Ehdr, e_machine),
	offsetof(Elf64_Ehdr, e_version), offsetof(Elf64_Ehdr, e_entry),
	offsetof(Elf64_Ehdr, e_phoff), offsetof(Elf64_Ehdr, e_shoff),
	offsetof(Elf64_Ehdr, e_flags), offsetof(Elf64_Ehdr, e_ehsize),
	offsetof(Elf64_Ehdr, e_phentsize), offsetof(Elf64_Ehdr, e_phnum),
	offsetof(Elf64_Ehdr, e_shentsize), offsetof(Elf64_Ehdr, e_shnum),
	offsetof(Elf64_Ehdr, e_shstrndx),

	offsetof(Elf_Note, n_namesz), offsetof(Elf_Note, n_descsz),
	offsetof(Elf_Note, n_type),

	offsetof(Elf64_Phdr, p_type), offsetof(Elf64_Phdr, p_offset),
	offsetof(Elf64_Phdr, p_vaddr), offsetof(Elf64_Phdr, p_paddr),
	offsetof(Elf64_Phdr, p_filesz), offsetof(Elf64_Phdr, p_memsz),
	offsetof(Elf64_Phdr, p_flags), offsetof(Elf64_Phdr, p_align),

	offsetof(Elf64_Shdr, sh_name), offsetof(Elf64_Shdr, sh_type),
	offsetof(Elf64_Shdr, sh_flags), offsetof(Elf64_Shdr, sh_addr),
	offsetof(Elf64_Shdr, sh_offset), offsetof(Elf64_Shdr, sh_size),
	offsetof(Elf64_Shdr, sh_link), offsetof(Elf64_Shdr, sh_info),
	offsetof(Elf64_Shdr, sh_addralign), offsetof(Elf64_Shdr, sh_entsize),

	offsetof(Elf64_Sym, st_name), offsetof(Elf64_Sym, st_value),
	offsetof(Elf64_Sym, st_size), offsetof(Elf64_Sym, st_info),
	offsetof(Elf64_Sym, st_shndx),

	offsetof(Elf64_Rel, r_offset), offsetof(Elf64_Rel, r_info),

	offsetof(Elf64_Rela, r_offset), offsetof(Elf64_Rela, r_info),
	offsetof(Elf64_Rela, r_addend)
};


static void usage (void);
static u_int64_t elf_get_quarter (Elf32_Ehdr *, void *, elf_member_t);
static u_int64_t elf_get_word (Elf32_Ehdr *, void *, elf_member_t);
static u_int64_t elf_get_quad (Elf32_Ehdr *, void *, elf_member_t);
static void elf_print_dynamic (Elf32_Ehdr *, void *);


/*
 * Helpers for ELF files with shnum or shstrndx values that don't fit in the
 * ELF header.  If the values are too large then an escape value is used to
 * indicate that the actual value is found in one of section 0's fields.
 */
static uint64_t
elf_get_shnum(Elf32_Ehdr *e, void *sh)
{
	uint64_t shnum;

	shnum = elf_get_quarter(e, e, E_SHNUM);
	if (shnum == 0)
		shnum = elf_get_word(e, (char *)sh, SH_SIZE);
	return shnum;
}

static uint64_t
elf_get_shstrndx(Elf32_Ehdr *e, void *sh)
{
	uint64_t shstrndx;

	shstrndx = elf_get_quarter(e, e, E_SHSTRNDX);
	if (shstrndx == SHN_XINDEX)
		shstrndx = elf_get_word(e, (char *)sh, SH_LINK);
	return shstrndx;
}


static char *shstrtab;
static char *dynstr;
static char *strtab;

/*
 * sod: shared object dumper
 */
int
main (int argc, char *argv[])
{
    Elf32_Ehdr *e;
    int fd;
    int i;
    struct stat sb;

    u_int64_t phoff;
    u_int64_t shoff;
    u_int64_t phentsize;
    u_int64_t phnum;
    u_int64_t shentsize;
    u_int64_t shnum;
    u_int64_t shstrndx;
    u_int64_t offset;
    u_int64_t name;
    u_int64_t type;

	void *p;
	void *sh;
	void *v;


    if (argc != 2)
        usage ();

    if ((fd = open (argv[1], O_RDONLY)) < 0 ||
        fstat (fd, &sb) < 0)
        err (1, "open: %s", argv[1]);

    e = mmap (NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (e == MAP_FAILED)
        err (1, "mmap");

    if (!IS_ELF(*(Elf32_Ehdr *)e))
        errx (1, "not an elf file");

	phoff = elf_get_off(e, e, E_PHOFF);
	shoff = elf_get_off(e, e, E_SHOFF);
	phentsize = elf_get_quarter(e, e, E_PHENTSIZE);
	phnum = elf_get_quarter(e, e, E_PHNUM);
	shentsize = elf_get_quarter(e, e, E_SHENTSIZE);
	p = (char *)e + phoff;

	if (shoff > 0) {
		sh = (char *)e + shoff;
		shnum = elf_get_shnum(e, sh);
		shstrndx = elf_get_shstrndx(e, sh);
		offset = elf_get_off(e, (char *)sh + shstrndx * shentsize,
		    SH_OFFSET);
		shstrtab = (char *)e + offset;
	} else {
		sh = NULL;
		shnum = 0;
		shstrndx = 0;
		shstrtab = NULL;
	}

	for (i = 0; (u_int64_t)i < shnum; i++) {
		name = elf_get_word(e, (char *)sh + i * shentsize, SH_NAME);
		offset = elf_get_off(e, (char *)sh + i * shentsize, SH_OFFSET);
		if (strcmp(shstrtab + name, ".strtab") == 0)
			strtab = (char *)e + offset;
		if (strcmp(shstrtab + name, ".dynstr") == 0)
			dynstr = (char *)e + offset;
	}

	for (i = 0; (u_int64_t)i < shnum; i++) {
		v = (char *)sh + i * shentsize;
		type = elf_get_word(e, v, SH_TYPE);
		if (type == SHT_DYNAMIC)
				elf_print_dynamic(e, v);
    }

    return 0;
}

static void
elf_print_dynamic(Elf32_Ehdr *e, void *sh)
{
	u_int64_t offset;
	u_int64_t entsize;
	u_int64_t size;
	int64_t tag;
	u_int64_t ptr;
	u_int64_t val;
	void *d;
	int i;

	offset = elf_get_off (e, sh, SH_OFFSET);
	entsize = elf_get_size (e, sh, SH_ENTSIZE);
	size = elf_get_size (e, sh, SH_SIZE);

	for (i = 0; (u_int64_t)i < size / entsize; i++) {
		d = (char *)e + offset + i * entsize;
		tag = elf_get_size (e, d, D_TAG);
		ptr = elf_get_size (e, d, D_PTR);
		val = elf_get_addr (e, d, D_VAL);
        if (tag == DT_NEEDED)
            fprintf (stdout, "%s\n", dynstr + val);
	}
}

static u_int64_t
elf_get_quarter(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch (e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be16dec(base);
			break;
		case ELFDATA2LSB:
			val = le16dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be16dec(base);
			break;
		case ELFDATA2LSB:
			val = le16dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASSNONE:
		errx(1, "invalid class");
	}

	return val;
}

static u_int64_t
elf_get_word(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch (e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASSNONE:
		errx(1, "invalid class");
	}

	return val;
}

static u_int64_t
elf_get_quad(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch (e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch (e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be64dec(base);
			break;
		case ELFDATA2LSB:
			val = le64dec(base);
			break;
		case ELFDATANONE:
			errx(1, "invalid data format");
		}
		break;
	case ELFCLASSNONE:
		errx(1, "invalid class");
	}

	return val;
}

static void
usage (void)
{
    fprintf (stderr, "usage: sod file\n");
    exit (1);
}
