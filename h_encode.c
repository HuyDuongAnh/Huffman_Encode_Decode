#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "htable_func.h"
#include "file.h"
int main(const int argc, const char *argv[]){
    unsigned int fd_r = 0, fd_w = 0;
    int array[ASCII_SIZE] = {0};
    char **h_table;
    node *list, *tree;
    fd_r = open_file('r', argv[1]);
    fd_w = open_file('w', argv[2]);
    get_c_to_arr(fd_r, array);
    list = array_to_list(array);
    if(list){  
        tree = sort_tree(list);
        h_table = tree_to_h_table(tree);
        if(fd_w){
            header_gen((const uint32_t *)array, fd_w); 
            body_gen(h_table, fd_w, fd_r);
            clean_tree(tree);
        }
    }
    close(fd_r);
    close(fd_w);
    return 0;
}
