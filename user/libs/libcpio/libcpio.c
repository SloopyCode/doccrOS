/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: libcpio.c
 *
 */

#include "libcpio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//most stuff is just copied from the kernel itself lol, cuz i need all that anyways lol

#define CPIO_TRAILER "TRAILER!!!"
#define CPIO_TYPE_MASK 0xF000
#define CPIO_TYPE_DIR 0x4000
#define CPIO_TYPE_REG 0x8000
#define CPIO_PATH_MAX 256 //vfs is limited

typedef struct __attribute__((packed)) {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
} cpio_header_t;

static unsigned int hex_val(const char *s, int len)
{
    unsigned int val = 0;

    for (int i = 0; i < len; i++)
    {
        char c = s[i];
        val <<= 4;

        if (c >= '0' && c <= '9') val |= (unsigned int)(c - '0' );
        else if (c >= 'a' && c <= 'f') val |= (unsigned int)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val |= (unsigned int)(c - 'A' + 10);
    }

    return val;
}

static int magic_ok(const char *m)
{
    return
        m[0] == '0' && m[1] == '7' && m[2] == '0' &&
        m[3] == '7' && m[4] == '0' &&
        (m[5] == '1' || m[5] == '2')
    ;
}

static unsigned long align4(unsigned long x)
{
    return (x + 3) & ~3UL;
}
static void join_path(
    char *out,
    int outsz,
    const char *dir,
    const char *name
) {
    int i = 0;
    int j = 0;
    int dlen = 0;

    while (dir[dlen]) dlen++;
    int has_slash = (dlen > 0 && dir[dlen - 1] == '/');

    while (dir[i] && i < outsz - 1)
    {
        out[i] = dir[i];
        i++;
    }

    if (!has_slash && i < outsz - 1) out[i++] = '/';

    while (name[j] && i < outsz - 1) out[i++] = name[j++];

    out[i] = '\0';
}

int cpio_extract_mem(
    const void *archive,
    unsigned long size,
    const char *dest_dir,
    cpio_extract_stats_t *out_stats
) {
    cpio_extract_stats_t local_stats;
    cpio_extract_stats_t *stats = out_stats ? out_stats : &local_stats;

    stats->total_entries = 0;
    stats->files_written = 0;
    stats->dirs_created = 0;

    if (!archive || !dest_dir || size < sizeof(cpio_header_t)) return -1;

    const unsigned char *base = (const unsigned char *)archive;
    unsigned long off = 0;

    mkdir(dest_dir, 0);

    while (off + sizeof(cpio_header_t) <= size)
    {
        const cpio_header_t *hdr = (const cpio_header_t *)(base + off);
        const char *name = (const char *)(base + off + sizeof(cpio_header_t));

        if (!magic_ok(hdr->magic)) break;

        unsigned int namesize = hex_val(hdr->namesize, 8);
        unsigned int filesize = hex_val(hdr->filesize, 8);
        unsigned int mode = hex_val(hdr->mode, 8);
        unsigned long data_off = align4(off + sizeof(cpio_header_t) + namesize);
        unsigned long next_off = align4(data_off + filesize);

        if (strcmp(name, CPIO_TRAILER) == 0) break;

        if (next_off > size) break;

        if (name[0] == '.' && name[1] == '/') name += 2;

        if (name[0] != '\0' && strcmp(name, ".") != 0)
        {
            char full_path[CPIO_PATH_MAX];
            unsigned int type = mode & CPIO_TYPE_MASK;

            join_path(full_path, sizeof(full_path), dest_dir, name);

            if (type == CPIO_TYPE_DIR)
            {
                mkdir(full_path, 0);
                stats->dirs_created++;
            }
            else if (type == CPIO_TYPE_REG)
            {
                int fd = (int)open(full_path, O_WRONLY | O_CREAT);
                if (fd >= 0)
                {
                    if (filesize > 0) write(fd, base + data_off, filesize);
                    close(fd);
                    stats->files_written++;
                }
            }
            //TODO:
            // dont ignore symlinks/devices
            stats->total_entries++;
        }

        off = next_off;
    }

    return 0;
}

int cpio_extract_file(
    const char *cpio_path,
    const char *dest_dir,
    cpio_extract_stats_t *out_stats
) {
    if (!cpio_path || !dest_dir) return -1;

    int fd = (int)open(cpio_path, O_RDONLY);
    if (fd < 0) return -1;

    long fsize = lseek(fd, 0, SEEK_END);
    if (fsize <= 0)
    {
        close(fd);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);

    void *buf = malloc((size_t)fsize);
    if (!buf)
    {
        close(fd);
        return -1;
    }

    long total_read = 0;
    while (total_read < fsize)
    {
        long r = read(fd, (char *)buf + total_read, (size_t)(fsize - total_read));
        if (r <= 0) break;
        total_read += r;
    }
    close(fd);

    if (total_read != fsize)
    {
        free(buf);
        return -1;
    }

    int rc = cpio_extract_mem(buf, (unsigned long)fsize, dest_dir, out_stats);
    free(buf);

    return rc;
}