#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

char *output_dir_path;
struct ext2_super_block super;
int fd;

struct ext2_dir_entry_2 *next_one(size_t *offset, void *data_block)
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

char *get_name_entry(struct ext2_dir_entry_2 *entry, char *buffer)
{
    strncpy(buffer, entry->name, entry->name_len);
    buffer[entry->name_len] = 0;
    return buffer;
}

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


char* get_path_id(int i)
{
    char *name = calloc(sizeof(char), 512);
    sprintf(name, "./%s/file-%d.jpg", output_dir_path, i);
    return name;
}
/**
 * recursive get data blocks and write to the file
 */
void write_output_indirect(FILE *file, int block_no, unsigned int indirect_level)
{
    if (block_no == 0 && indirect_level > 0)
        return;
    // init buffer
    char buffer[1024];
    // read block
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    read(fd, buffer, 1024);

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
    fclose(file);
    int filefd = open(dirName, O_RDWR);
    ftruncate(filefd, inode.i_size);
    close(filefd); 
}


void write_details(struct ext2_inode *inode, int inode_num){

		char *output_path = malloc(strlen(output_dir_path) + strlen("file-") + 5 + strlen("-details.txt"));
		int outputfd;
		char *output_details = malloc(1024);

		sprintf(output_path, "%s%cfile-%d-details.txt",output_dir_path, '/', inode_num);
		//printf("%s\n", output_path);
		outputfd = open(output_path, O_CREAT | O_WRONLY, 0666);

		sprintf(output_details, "%d\n%d\n%d", inode->i_links_count, inode->i_size, inode->i_uid);	
		
		write(outputfd, output_details, strlen(output_details));

		close(outputfd);
		free(output_path);
		free(output_details);
}

int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    /* This is some boilerplate code to help you get started, feel free to modify
       as needed! */

    fd = open(argv[1], O_RDONLY);    /* open disk image */
    output_dir_path = argv[2];

    // create dir
    mkdir(output_dir_path, 0777);

    ext2_read_init(fd);

    struct ext2_group_desc group;
    struct ext2_inode inode;
    off_t inode_table_start;

    __u32 first_block;
    char buffer[1024];
    // example read first the super-block and group-descriptor
    read_super_block(fd, 0, &super);

    for(size_t ng = 0; ng < num_groups; ng ++)
    {
        
        read_group_desc(fd, ng, &group);
        inode_table_start = locate_inode_table(ng, &group);

        // iterate each inode
        for(unsigned int i = 0; i <= inodes_per_group; i ++)
        {
            // read inode
            read_inode(fd, inode_table_start, i, &inode, 256);
            first_block = inode.i_block[0];

            //escape all bad file
            if(first_block == 0 && !S_ISREG(inode.i_mode))
                continue;

            // off_t data_block_start = locate_data_blocks(ng, &group);
            
            lseek(fd,  BLOCK_OFFSET(first_block) , SEEK_SET);
            read(fd, buffer, 1024);
            // find all JPG file
            if (buffer[0] == (char)0xff &&
                buffer[1] == (char)0xd8 &&
                buffer[2] == (char)0xff &&
                (buffer[3] == (char)0xe0 ||
                 buffer[3] == (char)0xe1 ||
                 buffer[3] == (char)0xe8))
            {
                char *name = get_path_id(i);
                // write in the file 
                write_output(inode, name);
                write_details(&inode, i);
                free(name);
                // for(unsigned j = 1; j < super.s_inodes_per_group; j++){
                // {
                    
                // }
            }
            memset(buffer, 0, 1024);
        }
    }

    for(size_t ng = 0; ng < num_groups; ng ++)
    {
        
        read_group_desc(fd, ng, &group);
        inode_table_start = locate_inode_table(ng, &group);
        // iterate each inode
        for(unsigned int i = 0; i <= inodes_per_group; i ++)
        {
            // read inode
            read_inode(fd, inode_table_start, i, &inode, 256);
            
            int file_ingroup;
            int local_inode_number;
            
            off_t inode_offset;
            __u32 first_block;
            struct ext2_dir_entry_2 *entry;
            struct ext2_group_desc group;
            struct ext2_inode file_inode;

            char db_buffer[1024];
            char buffer[1024];
            char name[EXT2_NAME_LEN];

            // print_inode_pointers(inode);
            // if inode is not directory
            if (!S_ISDIR(inode.i_mode))
                continue;;

            // read dir
            for (size_t block = 0; block < EXT2_NDIR_BLOCKS; block++)
            {
                int blockno = inode.i_block[block];
                // not valid block blocks
                if (blockno == 0)
                    continue;
                lseek(fd, BLOCK_OFFSET(inode.i_block[block]), SEEK_SET);
                read(fd, db_buffer, block_size);
                // for every entry in the data block
                for (size_t offset = 0; offset < block_size;)
                {
                    // get the entry
                    entry = next_one(&offset, db_buffer);
                    if (entry == NULL)
                        break;

                    get_name_entry(entry, name);
                    // don't handle self and parent
                    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
                        continue;
                    // if (strlen(name) > 0 && strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
                    //     printf("%s (inode: %d)\n", name, entry->inode);

                    // find group of file inode
                    file_ingroup = (entry->inode - 1) / inodes_per_group;
                    read_group_desc(fd, file_ingroup, &group);
                    inode_offset = locate_inode_table(file_ingroup, &group);
                    local_inode_number = entry->inode - file_ingroup * inodes_per_group;

                    read_inode(fd, inode_offset, local_inode_number, &file_inode, super.s_inode_size);
                    first_block = file_inode.i_block[0];

                    if (first_block == 0)
                    {
                        // printf("file %s don't have data block? (%d)\n", name, first_block);
                        continue;
                    }

                    lseek(fd, BLOCK_OFFSET(first_block), SEEK_SET);
                    read(fd, buffer, 1024);

                    if (buffer[0] == (char)0xff &&
                    buffer[1] == (char)0xd8 &&
                    buffer[2] == (char)0xff &&
                    (buffer[3] == (char)0xe0 ||
                    buffer[3] == (char)0xe1 ||
                    buffer[3] == (char)0xe8))
                    {
                        char *path = calloc(sizeof(char), 256);
                        sprintf(path, "./%s/%s", output_dir_path, name);
                        write_output(file_inode, path);
                        path = calloc(sizeof(char), 512);
                        sprintf(path, "./%s/file-%d-details.txt", output_dir_path, entry->inode);
                        write_base_info(file_inode, path);
                    }
                }
            }
        }
    }
    
    return 0;
}


