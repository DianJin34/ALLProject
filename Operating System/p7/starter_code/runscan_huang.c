#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIRST_INODE 11
#define MIN(a,b) (((a)<(b)) ? (a) : (b))

int isJPG(char a, char b, char c, char d);
void printInode(struct ext2_inode *inode, int inode_num);
void read_data(int fd, int block_no, void *buff, __u16 num_bytes);
void copy_JPG(int fd, struct ext2_inode *inode, int inode_num, char *altname);
void copy_details(struct ext2_inode *inode, int inode_num);


char *output_dir_path;
char *output_file_start = "file-";
char *output_file_end = ".jpg";
char *details = "-details.txt";
mode_t mode = O_CREAT | O_WRONLY;


int main(int argc, char **argv) 
{
    if (argc != 3){
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    /* This is some boilerplate code to help you get started, feel free to modify
       as needed! */
    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

	// check file was opened correctly
	if(fd == -1){
		printf("Unable to open disk image\n");
		exit(1);
	}
	output_dir_path = argv[2];

	// check if directory exists
	if(opendir(output_dir_path) != NULL){
		printf("Directory already exists\n");
		exit(1);
	}
	if(mkdir(output_dir_path, 0777) != 0){
		printf("Unable to make directory\n");
		exit(1);
	}
    ext2_read_init(fd);


    // example read first the super-block and group-descriptor
    struct ext2_super_block super;
    struct ext2_group_desc group;
    read_super_block(fd, 0, &super);
    read_group_desc(fd, 0, &group);

	// my change
    struct ext2_inode inode_f;
    struct ext2_inode inode_d;
	off_t inode_table_offset = locate_inode_table(0, &group);

	// loop through all inodes
	char buff[1024];
	for(unsigned i = 1; i < super.s_inodes_per_group; i++){
		read_inode(fd, inode_table_offset, i, &inode_f, 256);

		// find a regular file
		if(S_ISREG(inode_f.i_mode) && inode_f.i_block[0] != 0){
			unsigned block_no = inode_f.i_block[0];
			read_data(fd, block_no, buff, 4);
			
			// find a JPG file
			if(isJPG(buff[0], buff[1], buff[2], buff[3])){
				copy_JPG(fd, &inode_f, i, NULL);
				copy_details(&inode_f, i);
				
				for(unsigned j = 1; j < super.s_inodes_per_group; j++){
					read_inode(fd, inode_table_offset, j, &inode_d, 256);
					
					// find a potential directory
					if(S_ISDIR(inode_d.i_mode) && inode_d.i_block[0] != 0){
						read_data(fd, inode_d.i_block[0], buff, block_size);

						// scan through the first data block of the directory
						for(unsigned k = 0; k < block_size;){
							struct ext2_dir_entry *dentry = (struct ext2_dir_entry*) & (buff[k]);
							
							//reaches the end of the block
							if((dentry->rec_len & 0xFF) == 0)
								break;
							
							if (dentry->inode == i){
								// this is the file we are looking for
								int name_len = dentry->name_len & 0xFF; 
								char name [EXT2_NAME_LEN];
								strncpy(name, dentry->name, name_len);
								name[name_len] = '\0';
								copy_JPG(fd, &inode_f, i, name);
								break;

							} else {
								// move to the next entry
								k +=  4 * ((dentry->name_len / 4) + 1) + 8;
								if (dentry->name_len % 4 == 0)
									k -= 4;
							}
						}
					}
				}
			}
		}
	}
    return 0;
}

// return 1 if the file is a JPG, 0 otherwise
int isJPG(char a, char b, char c, char d)
{
	if (a == (char)0xff &&
			b == (char)0xd8 &&
			c == (char)0xff &&
			(d == (char)0xe0 ||
			d == (char)0xe1 ||
			d == (char)0xe8)) {
		return 1;	
	}
	return 0;
}

void printInode(struct ext2_inode *inode, int inode_num){
	printf( "\ninode info for inode %d: \n"
			"mode: %hu \n"
			"size: %d \n"
			"group id: %hu \n"
			"links count: %hu \n"
			"blocks count: %d \n"
			"first data block is %d \n"
			,
			inode_num,
			inode->i_mode,
			inode->i_size,
			inode->i_gid,
			inode->i_links_count,
			inode->i_blocks,
			inode->i_block[0]);

		printf("inode is reg file: %d \n", S_ISREG(inode->i_mode));
		printf("inode is dir: %d \n", S_ISDIR(inode->i_mode));
}

// i dont know if this is how you read data blocks
void read_data(int fd, int block_no, void *buff, __u16 num_bytes)
{
        lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET); 
        read(fd, buff, num_bytes);
}

void copy_JPG(int inputfd, struct ext2_inode *inode, int inode_num, char* altname){

	// path of the file '(ouput_dir)/file-(inode_num).jpg
	
	char *output_path;
	char *data_buffer = malloc(block_size);		// stores 1 page of data
	unsigned bytes_left = inode->i_size;		// number of bytes that need to be copied from the file
	int bytes_to_copy = 0;						// current number of bytes that mu
	int outputfd;

	if(altname == NULL){
		output_path = malloc(strlen(output_dir_path) + strlen(output_file_start) + 5 + strlen(output_file_end));
		sprintf(output_path, "%s%c%s%d%s",output_dir_path, '/', output_file_start, inode_num, output_file_end);
	} else {
		output_path = malloc(255 + strlen(output_dir_path) + 2);
		sprintf(output_path, "%s%c%s", output_dir_path, '/', altname);

	}
	
	int *indirect_pointer = malloc(block_size);
	int *double_indirect_pointer = malloc(block_size);

	// printf("%s\n", output_path);

	outputfd = open(output_path, mode, 0666);
	
	// copy from direct pointers	
	for(unsigned i = 0; i < EXT2_NDIR_BLOCKS && bytes_left > 0; i++){
		bytes_to_copy = MIN(bytes_left, block_size);	
		read_data(inputfd, inode->i_block[i], data_buffer, bytes_to_copy);
		write(outputfd, data_buffer, bytes_to_copy);
		bytes_left -= bytes_to_copy;
	}	

	// loop through pointers stored in the indirect pointer	
	if(bytes_to_copy != 0){
		read_data(inputfd, inode->i_block[EXT2_IND_BLOCK], indirect_pointer, block_size);
		for(unsigned i = 0; i < block_size / sizeof(int) && bytes_left > 0; i++){
			bytes_to_copy = MIN(bytes_left, block_size);
			read_data(inputfd, indirect_pointer[i], data_buffer, bytes_to_copy);
			write(outputfd, data_buffer, bytes_to_copy);
			bytes_left -= bytes_to_copy;
		}
	}

	if(bytes_to_copy != 0){
		// loop through data in the double indirect pointer
		read_data(inputfd, inode->i_block[EXT2_DIND_BLOCK], double_indirect_pointer, block_size);
		for(unsigned i = 0; i < block_size / sizeof(int) && bytes_left > 0; i++){
			read_data(inputfd, double_indirect_pointer[i], indirect_pointer, block_size);
			for(unsigned j = 0; j < block_size / sizeof(int) && bytes_left > 0; j++){
				bytes_to_copy = MIN(bytes_left, block_size);
				read_data(inputfd, indirect_pointer[j], data_buffer, bytes_to_copy);
				write(outputfd, data_buffer, bytes_to_copy);
				bytes_left -= bytes_to_copy;
			}
		}
	}
	
	free(output_path);
	free(data_buffer);
	free(indirect_pointer);
	free(double_indirect_pointer);
	close(outputfd);
}


void copy_details(struct ext2_inode *inode, int inode_num){

		char *output_path = malloc(strlen(output_dir_path) + strlen(output_file_start) + 5 + strlen(details));
		int outputfd;
		char *output_details = malloc(1024);

		sprintf(output_path, "%s%c%s%d%s",output_dir_path, '/', output_file_start, inode_num, details);
		//printf("%s\n", output_path);
		outputfd = open(output_path, mode, 0666);

		sprintf(output_details, "%d\n%d\n%d", inode->i_links_count, inode->i_size, inode->i_uid);	
		
		write(outputfd, output_details, strlen(output_details));

		close(outputfd);
		free(output_path);
		free(output_details);
}
		