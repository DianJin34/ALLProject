#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int fd;
char *output_dir;
struct ext2_super_block super;

void read_data_block(int fd, int block_no, void *buffer, size_t buffer_size)
{
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    read(fd, buffer, buffer_size);
}

/**
 * (DEBUG)
 * print the pointers in the inode
 */
void print_inode_pointers(struct ext2_inode inode)
{
    for (size_t block = 0; block < 15; block++)
    {
        printf("%d ", inode.i_block[block]);
    }
    printf("\n");
}

/**
 * Check a file is jpg
 *
 * @param buffer    The first data chunk
 */
int is_jpg(char *buffer)
{
    return (buffer[0] == (char)0xff && buffer[1] == (char)0xd8 && buffer[2] == (char)0xff &&
            (buffer[3] == (char)0xe0 || buffer[3] == (char)0xe1 || buffer[3] == (char)0xe8));
}

/**
 * get the entry name of a directory entry
 *
 * @param entry     directory entry
 * @param buffer    Where to put the name
 */
char *get_entry_name(struct ext2_dir_entry_2 *entry, char *buffer)
{
    strncpy(buffer, entry->name, entry->name_len);
    buffer[entry->name_len] = 0;
    return buffer;
}

/**
 * get full path name of the output file by inode number
 *
 * @param inode_number  inode number
 */
char *get_full_path_by_id(int inode_number)
{
    char *dirName = calloc(sizeof(char), 512);
    sprintf(dirName, "./%s/file-%d.jpg", output_dir, inode_number);
    return dirName;
}

/**
 * get full path name of the output file by the old file name
 *
 * @param name original entry name
 */
char *get_full_path_by_name(char *name)
{
    char *dirName = calloc(sizeof(char), 256);
    sprintf(dirName, "./%s/%s", output_dir, name);
    return dirName;
}

/**
 * get full path name of the base info output file by inode number
 *
 * @param inode_number  inode number
 */
char *get_base_info_path(int inode_number)
{
    char *dirName = calloc(sizeof(char), 512);
    sprintf(dirName, "./%s/file-%d-details.txt", output_dir, inode_number);
    return dirName;
}

/**
 * recursive get data blocks and write to the file
 *
 * @param file              The file prints to
 * @param block_no          The block number to look at
 * @param indirect_level    Depth of the indirect node (0 for direct)
 */
void write_output_indirect(FILE *file, int block_no, unsigned int indirect_level)
{
    // invalid indirect block
    // TODO: understant that why direct block with id 0 is part of the file
    if (block_no == 0 && indirect_level > 0)
        return;
    // init buffer
    char buffer[1024];
    // read block
    read_data_block(fd, block_no, buffer, block_size);
    // direct
    if (indirect_level == 0)
    {
        // just write the block data to file
        fwrite(buffer, sizeof(char), 1024, file);
        return;
    }
    // indirect
    // reduce level
    indirect_level--;
    // loop through all entires
    for (size_t i = 0; i < block_size / sizeof(uint_least32_t); i++)
    {
        uint_least32_t child_blockno = ((uint_least32_t *)buffer)[i];
        // not a blocks
        if (child_blockno == 0)
            continue;
        // printf("%lu %u\n", i, child_blockno);
        // reading data from the block
        write_output_indirect(file, child_blockno, indirect_level);
    }
}

/**
 * write output to file
 *
 * @param inode         Inode
 * @param dirName       Name of the file
 *
 */
void write_output(struct ext2_inode inode, char *dirName)
{
    // print_inode_pointers(inode);
    FILE *file = fopen(dirName, "w+");
    // int fsize_in_block = inode.i_fsize / block_size;
    if (file == NULL)
    {
        printf("cannot create file %s\n", dirName);
        exit(1);
    }

    // direct blocks
    for (size_t i = 0; i < EXT2_NDIR_BLOCKS; i++)
    {
        write_output_indirect(file, inode.i_block[i], 0);
    }
    // single indirect
    if (inode.i_block[EXT2_IND_BLOCK] != 0)
    {
        write_output_indirect(file, inode.i_block[EXT2_IND_BLOCK], 1);
    }
    // double indirect
    if (inode.i_block[EXT2_DIND_BLOCK] != 0)
    {
        write_output_indirect(file, inode.i_block[EXT2_DIND_BLOCK], 2);
    }
    // triple indirect
    if (inode.i_block[EXT2_TIND_BLOCK] != 0)
    {
        // I don't think this would really happen
        write_output_indirect(file, inode.i_block[EXT2_TIND_BLOCK], 3);
    }

    ftruncate(fileno(file), inode.i_size);
    fclose(file);
}

/**
 * write the base info of the inode
 *
 * @param inode         Inode
 * @param dirName       Name of the file
 *
 */
void write_base_info(struct ext2_inode inode, char *dirName)
{
    FILE *file = fopen(dirName, "w+");
    // int fsize_in_block = inode.i_fsize / block_size;
    if (file == NULL)
    {
        printf("cannot create file %s\n", dirName);
        exit(1);
    }

    fprintf(file, "%u\n%u\n%u", inode.i_links_count, inode.i_size, inode.i_uid);
    fclose(file);
}

/**
 * get next entry in the directory data block
 *
 * @param offset        The current offset
 * @param data_block    The directory data block
 *
 * @return pointer to the next directory entry
 */
struct ext2_dir_entry_2 *next_entry(size_t *offset, void *data_block)
{
    int actual_length;
    struct ext2_dir_entry_2 *entry;

    entry = ((void *)data_block + *offset);
    actual_length = 8 + entry->name_len;
    // round to upper multiple of 4
    actual_length = (actual_length + 3) >> 2 << 2;
    *offset += actual_length;
    // printf("%d,%d\n", actual_length, entry->rec_len);
    // end of nodes?
    if (actual_length == 8)
        return NULL;
    // end of nodes?
    if (entry->inode == 0)
        return NULL;

    return entry;
}

/**
 * Open the directory inode and find any child is jpg
 *
 * @param inode the inode believed to be an directory
 */
void open_dir(struct ext2_inode inode)
{
    int file_inode_ngroup;
    int local_inode_number;

    off_t inode_offset;
    __u32 first_block;
    struct ext2_dir_entry_2 *entry;
    struct ext2_group_desc group;
    struct ext2_inode file_inode;

    char dir_block_buffer[1024];
    char file_block_buffer[1024];
    char name[EXT2_NAME_LEN];

    // print_inode_pointers(inode);
    // if inode is not directory
    if (!S_ISDIR(inode.i_mode))
        return;

    // read dir
    for (size_t block = 0; block < EXT2_NDIR_BLOCKS; block++)
    {
        int blockno = inode.i_block[block];
        // not valid block blocks
        if (blockno == 0)
            continue;

        read_data_block(fd, inode.i_block[block], dir_block_buffer, block_size);
        // for every entry in the data block
        for (size_t offset = 0; offset < block_size;)
        {
            // get the entry
            entry = next_entry(&offset, dir_block_buffer);
            if (entry == NULL)
                break;

            get_entry_name(entry, name);
            // don't handle self and parent
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
                continue;
            // if (strlen(name) > 0 && strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
            //     printf("%s (inode: %d)\n", name, entry->inode);

            // find group of file inode
            file_inode_ngroup = (entry->inode - 1) / inodes_per_group;
            read_group_desc(fd, file_inode_ngroup, &group);
            inode_offset = locate_inode_table(file_inode_ngroup, &group);
            local_inode_number = entry->inode - file_inode_ngroup * inodes_per_group;

            read_inode(fd, inode_offset, local_inode_number, &file_inode, super.s_inode_size);
            first_block = file_inode.i_block[0];

            if (first_block == 0)
            {
                // printf("file %s don't have data block? (%d)\n", name, first_block);
                continue;
            }

            read_data_block(fd, first_block, file_block_buffer, 1024);
            if (is_jpg(file_block_buffer))
            {
                // printf("traversal found jpg %s at inode %d \n", name, entry->inode);
                // print_inode_pointers(file_inode);
                char *path = get_full_path_by_name(name);
                write_output(file_inode, path);
                path = get_base_info_path(entry->inode);
                write_base_info(file_inode, path);
            }
            else
            {
                // printf("file %s is not jpg (%d: %x)\n", name, first_block, *(int *)(void *)file_block_buffer);
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    char buffer[1024];
    unsigned int global_inode_number;
    struct ext2_inode inode;
    struct ext2_group_desc group;
    off_t offset_to_inode_table;
    __u32 first_block;

    output_dir = argv[2];
    fd = open(argv[1], O_RDONLY); /* open disk image */

    // create output folder
    mkdir(output_dir, 0777);

    ext2_read_init(fd);

    // example read first the super-block and group-descriptor
    read_super_block(fd, 0, &super);

    // for all groups (first enumerate)
    for (size_t ngroup = 0; ngroup < num_groups; ngroup++)
    {
        // first read group
        read_group_desc(fd, ngroup, &group);
        offset_to_inode_table = locate_inode_table(ngroup, &group);
        // off_t offset_to_data_block = locate_data_blocks(ngroup, &group);
        // enumerate through the block that contains inodes
        for (unsigned int inode_number = 1; inode_number <= inodes_per_group; inode_number++)
        {
            global_inode_number = inode_number + ngroup * inodes_per_group;

            read_inode(fd, offset_to_inode_table, inode_number, &inode, super.s_inode_size);
            first_block = inode.i_block[0];
            if (first_block == 0)
                continue;

            read_data_block(fd, first_block, buffer, 1024);
            if (is_jpg(buffer))
            {
                // printf("inode offset: %lu\n", offset_to_inode_table);
                // printf("fo0und jpg at inode %u, (%u)\n", global_inode_number, inode.i_blocks);
                char *dirName = get_full_path_by_id(global_inode_number);
                write_output(inode, dirName);
                free(dirName);
            }
            else
            {
                // printf("file is not jpg (%d: %x)\n", global_inode_number, *(int *)(void *)buffer);
            }
            memset(buffer, 0, 1024);
        }
    }

    // printf("--- Start all dir travers ---\n");
    // for all groups (second enumerate)
    for (size_t ngroup = 0; ngroup < num_groups; ngroup++)
    {
        // first read group
        read_group_desc(fd, ngroup, &group);
        offset_to_inode_table = locate_inode_table(ngroup, &group);
        // enumerate through the block that contains inodes
        for (unsigned int inode_number = 1; inode_number <= inodes_per_group; inode_number++)
        {
            // read dir
            struct ext2_inode inode;
            read_inode(fd, offset_to_inode_table, inode_number, &inode, super.s_inode_size);

            if (!S_ISDIR(inode.i_mode))
                continue;

            open_dir(inode);
        }
    }

    return 0;
}
