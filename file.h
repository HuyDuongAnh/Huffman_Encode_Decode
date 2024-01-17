#ifndef FILE_H
#define FILE_H
int open_file(int, const char *);
void header_gen(const uint32_t *, int);
void get_c_to_arr(int, int *);
void body_gen(char **, int, int);
void buff_n_write(uint8_t , char **, uint8_t *, int); 
void reset_buff(uint8_t *, int);
#endif
