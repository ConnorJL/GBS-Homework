#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int hexdump (FILE *, char *, int);

#define BLOCK_SIZE		4096
#define SUPERBLOCK		0
#define SUPERBLOCK_OFFSET	1024
#define ROOT_DIR_INODE          2

/* super block offsets */
#define N_INODES		0x00
#define N_BLOCKS		0x04
#define FREE_INODES		0x10
#define FREE_BLOCKS		0x0c
#define FIRST_DATA_BLOCK	0x14
#define LOG_BLOCKSIZE		0x18
#define BLOCKS_PER_GROUP	0x20
#define INODES_PER_GROUP	0x28
#define FIRST_INODE		0x54
#define INODE_SIZE		0x58
#define INCOMPAT		0x60
#define COMPAT			0x64
#define VOLUME_LABEL		0x78
#define PREALLOC_FILE		0xcc
#define RESERVED_GDT		0xce

/* block group offsets */
#define INODE_TABLE		0x08

/* inode offsets */
#define INODE_FILE_SIZE         0x04
#define INODE_FLAGS		0x20
#define INODE_LINK_COUNT        0x1a
#define INODE_N_BLOCKS		0x1c
#define INODE_FILE_BLOCKS	0x28

/* file blocks in ext4 can be managed via extent trees */
#define EXTENT_MAGIC		0x00
#define EXTENT_ENTRIES		0x02
#define	EXTENT_DEPTH		0x06

/* directory entries */
#define DIR_INODE		0x00
#define DIR_REC_LEN		0x04
#define DIR_NAME_LEN		0x06
#define DIR_FILE_TYPE		0x07
#define DIR_FILE_NAME		0x08

int	block_size = 4096;
int	inode_size, inodes_per_group, inode_table, sb_extents;

int read_block (int fd, char *buffer, int block_no) {
    int			len;
    unsigned long	offset = (unsigned long) block_no * (unsigned long) block_size;
    
    /* position read pointer at the beginning of the block, blocks count from zero */
    if (lseek (fd, offset, SEEK_SET) == -1) {
	perror ("lseek failed");
	return -1;
    }
    /* read the block contents */
    if ((len = read (fd, buffer, block_size)) == -1) {
	perror ("Cannot read root directory inode");
	return -1;
    }
    return len;
}

void extract_superblock_info (char *buffer) {
    char    *s;

    s = buffer + SUPERBLOCK_OFFSET;
    printf ("#i-nodes =\t%d\n", *((int *) (s + N_INODES)));
    printf ("#blocks =\t%d\n", *((int *) (s + N_BLOCKS)));
    printf ("#free inodes =\t%d\n", *((int *) (s + FREE_BLOCKS)));
    printf ("#free blocks =\t%d\n", *((int *) (s + FREE_BLOCKS)));
    block_size = 1 << (10 + *((int *) (s + LOG_BLOCKSIZE)));
    printf ("#block size =\t%d\n", block_size);
    printf ("#blocks/group =\t%d\n", *((int *) (s + BLOCKS_PER_GROUP)));
    inodes_per_group =  *((int *) (s + INODES_PER_GROUP));
    printf ("#inodes/group =\t%d\n", inodes_per_group);
    printf ("first inode # =\t%d\n", *((int *) (s + FIRST_INODE)));
    inode_size = *((short *) (s + INODE_SIZE));
    printf ("inode size =\t%d\n", inode_size);
    // printf ("incompat features =\t%x\n", *((int *) (s + INCOMPAT)));
    sb_extents = *((int *) (s + INCOMPAT)) & 0x40;
    // printf ("compat features =\t%x\n", *((int *) (s + COMPAT)));
    printf ("volume label =\t%s\n", s + VOLUME_LABEL);
    printf ("prealloc file =\t%d\n", *(s + PREALLOC_FILE));
    printf ("reserved GDT =\t%d\n", *((short *) (s + RESERVED_GDT)));
}

char *read_inode (int fd, char *buffer, int inode) {
    int    block_no;
    int    offset;
    int    len;

    block_no = inode_table + (inode - 1) * inode_size / block_size;
    offset = ((inode - 1) * inode_size) % block_size;

    if ((len = read_block (fd, buffer, block_no)) == -1) {
	perror ("Cannot read inode");
	return NULL;
    }
    return buffer + offset;
}

int get_data_blocks (char *buffer, int *blocks, int *nblocks, int inode) {
    char	*s = buffer;
    int		file_size, extents, i, j, k;
    int         entries, n;

    file_size = *((int *) (s + INODE_FILE_SIZE));
    extents = *((int *) (s + INODE_FLAGS)) & 0x80000;

    if (!extents) {
        /* old structure using just direct and indirect blocks */
        for (i = 0; i < file_size / block_size && i < *nblocks; i++) {
            blocks [i] = *((int *) (s + INODE_FILE_BLOCKS + 4*i));
        }
	*nblocks = i;
    } else {
    	/* new structure using extents -> lazy and just read the first block for now */
        s += INODE_FILE_BLOCKS;
        printf ("\textent magic number = %x\n", *((short *) (s + EXTENT_MAGIC)) & 0x00ffff);
	entries = *((short *) (s + EXTENT_ENTRIES));
        printf ("\textent # entries = %x\n", entries);
        printf ("\textent depth = %x\n", *((short *) (s + EXTENT_DEPTH)));
        s += 12; /* extent tree header size */
        /* leaf nodes */
	i = 0;
	for (j = 0; j < entries && i < *nblocks; j++) {
            printf ("\textent start block = %d\n", *((int *) (s)));
	    n = *((short *) (s + 4));
            printf ("\textent # blocks = %d\n", n);
            printf ("\tstart block number = %d\n", *((int *) (s + 8)));
	    /* add n consecutive blocks starting at the indicated block number */
	    for (k = 0; k < n && i < *nblocks; k++)
                blocks [i++] = *((int *) (s + 8)) + k;
	    s += 12; /* next extent block */
	}
	*nblocks = i;
    }
    printf ("Data blocks of inode %d:", inode);
    for (i = 0; i < *nblocks; i++) {
        printf ("\t%s%d%s", i > 11 ? "[" : "", blocks [i], i > 11 ? "]" : "");
    }
    printf ("\n");
    return file_size;
}

void print_inode (char *s, int inode) {
    int		extents, file_size; 

    printf ("i-node %d\n", inode);
    extents = *((int *) (s + INODE_FLAGS)) & 0x80000;
    printf ("\tinode flags = \%x\n", *((int *) (s + INODE_FLAGS)));
    printf ("\tmode = %o\n", *((short *) s) & 00777);
    printf ("\ttype = %x\n", *((short *) s) & 0x0f000);
    file_size = *((int *) (s + INODE_FILE_SIZE));
    printf ("\tfile size = %d\n", file_size);
    printf ("\tlinks = %d\n", *((short *) (s + INODE_LINK_COUNT)));
    printf ("\textent = %s\n", extents ? "YES" : "NO");
}

int search_dir (int fd, int *blocks, int nblocks, char *target) {
    int		inode = 0, len, i;
    char	buffer [BLOCK_SIZE], *s;
    char	fn [256];

    for (i = 0; i < nblocks; i++) {
    	if ((len = read_block (fd, buffer, blocks [i])) == -1) {
	    close (fd);
	    return -1;
	}
    	/* print directory contents */
        s = buffer;
        while (s < buffer + len) {
            strncpy (fn, s + DIR_FILE_NAME, *((unsigned char *) (s + DIR_NAME_LEN)));
            fn [*((unsigned char *) (s + DIR_NAME_LEN))] = '\0';
            printf ("\t%d\t%x\t%s\n", *((int *) s), *(s + DIR_FILE_TYPE), fn);
	    /* we want to look at /etc, so check when we find this directory */
	    if (!strcmp (fn, target))
		inode = *((int *) s);
            s += *((short *) (s + DIR_REC_LEN));
        }
    }
    return inode;
}

int main() {
    int blocks[1024];
    int fd = 0;
    int ret = search_dir(fd, blocks, 1023, "/mnt");
    printf("ret%d\n", ret);
    // fd = open mit rdonly
}