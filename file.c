#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "htable_func.h"
#define FOUR_BYTE 4
#define ONE_BYTE 1
#define ERR_CODE -1
#define BUFF_LENGTH 2
#define ONE_BYTE_BIT_NUM 8

/* The first argument of open_file is 'r' or 'w', which stand for read or 
 * write option.
 * */
unsigned int open_file(int option, const char *file_name){
    char path[PATH_MAX] = {0};
    unsigned int ret_fd = 0;
    if(option == 'r'){
        if(!getcwd(path, PATH_MAX)){
            perror("getcwd in open_file");
            exit(1);
        }
        else{
            strcat(path, "/");
            strcat(path, file_name);
            if((ret_fd = open(path, O_RDONLY)) == -1){
                perror(file_name);
                exit(1);
            }
        }
    }
    if(option == 'w'){
        if(!getcwd(path, PATH_MAX)){
            perror("getcwd in open_file");
            exit(1);
        }
        else{
            if(!file_name)
                ret_fd = 1;
            else{
                strcat(path, "/");
                strcat(path, file_name);
                if((ret_fd = creat(path, S_IRUSR | S_IWUSR)) == -1){
                    perror(file_name);
                    exit(1);
                }
            }
        }
    }
    return ret_fd;
}

void header_gen(const uint32_t *array, int fd_w){
    uint32_t c_count = 0;
    uint8_t ch = 0;
    for(int i = 0; i < ASCII_SIZE; ++i){
        if(array[i])
            c_count++;
    }
    write(fd_w, &c_count, FOUR_BYTE);
    for(int i = 0; i < ASCII_SIZE; ++i){
        if(array[i]){
            write(fd_w, &ch, ONE_BYTE);
            write(fd_w, &array[ch], FOUR_BYTE);
        }
        ++ch;
    }
}

/* The function takes a character to find the hcode in h_table, and write
 * that code into buf_w. At the end, the function returns the number of 
 * byte written in buf_w.
 * */
void buff_n_write(uint8_t ch, char **h_table, uint8_t *buf_w, int fd_w){
    static uint8_t mask = 128;
    static int bit_pos = 0;
    static uint32_t byte_num = 0;
    char *h_code = h_table[ch];
    for(int i = 0; i < strlen(h_code); ++i){
        if(h_code[i] == '1')
            buf_w[byte_num] |= mask;
        mask >>= 1;
        bit_pos++;
        if(!mask)
            mask = 128;
        if(!(bit_pos%ONE_BYTE_BIT_NUM))
            byte_num++;
        if(bit_pos == BUFF_LENGTH){
            write(fd_w, buf_w, BUFF_LENGTH);
            reset_buff(buf_w, BUFF_LENGTH);
        }
   }
}


void reset_buff(uint8_t *buf_w, int length){
}

/* The function reset the cursor inside file fd_r. Then, it reads 256 
 * character from read file. For each read character, the corresponding hcode
 * is written into the buffer. When the buffer is full, the entire buffer
 * is flushed into write file and is reset. If the buffer is not full and the
 * read file is done reading, the remain byte is written out to write file.
 * */
void body_gen(char **h_table, int fd_w, int fd_r){
    lseek(fd_r, 0, SEEK_SET);
    uint8_t ch = 0, buf_r[ASCII_SIZE] = {0}, buf_w[BUFF_LENGTH] = {0};
    unsigned int byte_r = 0, byte_w = 0;
    while((byte_r = read(fd_r, buf_r, ASCII_SIZE))){
        for(unsigned int i = 0; i < byte_r; ++i){
            if(buf_r[i]){
                ch = buf_r[i];
                buff_n_write(ch, h_table, buf_w, fd_w);
            }
        }
    }
}

void get_c_to_arr(int fd_r, int *array){
    char buf[ASCII_SIZE] = {0};
    unsigned int byte_read = 0;
    while((byte_read = read(fd_r, buf, ASCII_SIZE))){
        for(int i = 0; i < byte_read; ++i){
            if(buf[i])
                char_to_array(buf[i], array);
        }
    }
}

