#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "htable_func.h"
#define ONE_NODE 1

node *new_node(){
    node *new_node = calloc(ONE_NODE, sizeof(node));
    if(!new_node){
        perror("calloc failed\n");
        exit(1);
    }
    new_node->c = 0;
    new_node->freq = 0;
    new_node->h_encode = NULL;
    new_node->next = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

/* The function takes a character as index value and increase array[index]
 * in each encounter in the array. 
 * */
void char_to_array(uint8_t index, int *array){
    assert(array);
    array[index]++; 
}


/* The function converts a frequency array to a linked list, and 
 * return the list. The list is sorted by frequency in ascending order.
 * */
node *array_to_list(int *array){
    node *head = NULL;
    for(int i = 0; i < ASCII_SIZE; ++i){
        if(array[i]){
            if(head == NULL){
                head = new_node();
                head->c = i;
                head->freq = array[i];
            }
            else
                insert_list(&head, array[i], (unsigned char)i);    
        }
    }
    return head;
}

/* Insert node to the right place in the list.
 * */
void insert_list(node **head, int f, unsigned char c){
    assert(head);
    node *prev, *curr;
    curr = prev = *head;
    while(curr->next && curr->next->freq <= f){
        prev = curr;
        curr = curr->next;
    }
    if(curr->freq < f){
        add_list_after(curr, f, c);
    }
    else if(curr->freq == f){
        if(curr->c > c)
            add_list_before(head, prev, curr, f, c);
        else
            add_list_after(curr, f, c);
    }
    else
        add_list_before(head, prev, curr, f, c);
}

void add_list_before(node **head, node *prev, node *curr, 
        int f, unsigned char c){
    assert(head && prev && curr);
    if(prev == curr){
        prev = new_node();
        prev->c = c;
        prev->freq = f;
        prev->next = curr;
        *head = prev;
    }
    else{
        prev->next = new_node();
        prev->next->c = c;
        prev->next->freq = f;
        prev->next->next = curr;
    }
}

void add_list_after(node *curr, int f, unsigned char c){
    assert(curr);
    node *temp = curr->next;
    curr->next = new_node();
    curr->next->c = c;
    curr->next->freq = f;
    curr->next->next = temp;
}

// Function to print binary tree in 2D 
// It does reverse inorder traversal 
void print_tree(node *root, int space) 
{ 
    // Base case 
    if (root == NULL) 
        return; 

    // Increase distance between levels 
    space += COUNT; 

    // Process right child first 
    print_tree(root->right, space); 

    // Print current node after space 
    // count 
    printf("\n"); 
    for (int i = COUNT; i < space; i++) 
        printf(" "); 
    if(isspace(root->c))
        printf("%d", root->c); 
    else
        printf("%c", root->c);
    printf(" %d\n", root->freq);
    // Process left child 
    print_tree(root->left, space); 
} 

/* The function takes the linked list and create a tree. This function calls
 * two_to_one_node function to combine first two nodes together.
 * */
node *sort_tree(node *list){
    assert(list);
    node *new_node, *head, *first, *second;
    head = list;
    while(head->next){
        if(!head->next->next)
            head = two_to_one_node(head, head->next);
        else{
            first = head;
            second = head->next;
            head = head->next->next;
            new_node = two_to_one_node(first, second);
            insert_node(&head, new_node);
        }
    }
    return head;
}

/* The function traverses through the node until it finds the node that has
 * higher frequency then new_node->freq. 
 * */
void insert_node(node **head, node *new_node){
    assert(head && new_node);
    node *prev, *curr, *temp;
    curr = prev = *head;
    int brk_flag = 1;
    while(curr->next && brk_flag){
        if(curr->next->freq > new_node->freq)
            brk_flag = 0;
        else if(curr->next->freq == new_node->freq){
            if(curr->next->c > new_node->c)
                brk_flag = 0;
            else{
                prev = curr;
                curr = curr->next;
            }
        }
        else{
            prev = curr;
            curr = curr->next;
        }
    }
    if(curr == *head && curr->freq > new_node->freq){
        temp = *head;
        *head = new_node;
        new_node->next = temp;
    }
    else{
        temp = curr->next;
        curr->next = new_node;
        new_node->next = temp;
    }
}

/* The function takes in two nodes, and combine them into one node 
 * that has 2 children. The node also contains the left node character 
 * with the added frequency.
 * */
node *two_to_one_node(node *first, node *second){
    assert(first && second);
    node *n_node = new_node();
    first->next = NULL;
    second->next = NULL;
    n_node->freq = first->freq + second->freq;
    if(first->c < second->c)
        n_node->c = first->c;
    else
        n_node->c = second->c;
    if(first->freq == second->freq){
        if(first->c < second->c){
            n_node->left = first;
            n_node->right = second;
        }
        else{
            n_node->left = second;
            n_node->right = first;
        }
    }
    else{
        n_node->left = first;
        n_node->right = second;
    }
    return n_node;
}

/* The function put Huffman code into each node of the tree and convert 
 * the tree to the array of pointer.
 * */
char **tree_to_h_table(node *tree){
    char **h_table = calloc(ASCII_SIZE, sizeof(char*));
    if(!h_table){
        perror("calloc failed\n");
        exit(1);
    }
    tree->h_encode = calloc(1, sizeof(char));
    if(!tree->h_encode){
        perror("calloc failed\n");
        exit(1);
    }
    encode_node(tree);
    table_encode(h_table, tree);
    return h_table;
}


/* The function takes tree input, and trace each leaf. If the leaf is going 
 * to the right, add "1" into the p_array[index]. Else if the leaf is going
 * to the left, add "0" into the p_array[index].
 * */
void encode_node(node *tree_node){
    assert(tree_node);
    if(tree_node->left){
        char *p_code_l = calloc(strlen(tree_node->h_encode)+2, sizeof(char));
        if(!p_code_l){
            perror("calloc failed\n");
            exit(1);
        }
        p_code_l = strcpy(p_code_l, tree_node->h_encode);
        tree_node->left->h_encode = strcat(p_code_l, L_STR);
        encode_node(tree_node->left);
    }
    if(tree_node->right){ 
        char *p_code_r = calloc(strlen(tree_node->h_encode)+2, sizeof(char));
        if(!p_code_r){
            perror("calloc failed\n");
            exit(1);
        }
        p_code_r = strcpy(p_code_r, tree_node->h_encode);
        tree_node->right->h_encode = strcat(p_code_r, R_STR);
        encode_node(tree_node->right);
    }
}

/* The function traverses through the tree to get the code for each character
 * then store the code into the h_table.
 * NOTE: h_table has the address of each tree nodes, at the end, free all 
 * member of tree node is RECOMMENDED. Don't free both htable and tree.
 * */
void table_encode(char **h_table, node *tree){
    assert(h_table && tree);
    if(tree->left)
        table_encode(h_table, tree->left);
    else
        h_table[(int)tree->c] = tree->h_encode;
    if(tree->right)
        table_encode(h_table, tree->right);
    else
        h_table[(int)tree->c] = tree->h_encode;
}

/* The function traverse through pointer array h_table and print non-NULL 
 * string.
 * */
void print_htable(char **h_table){
    for(int i = 0; i < ASCII_SIZE; ++i){
        if(h_table[i])
            printf("0x%02x: %s\n", i, h_table[i]);
    }
}

/* Traverse recursively through the left side and right side of the tree 
 * to free each left and right child.
 * */
void clean_tree(node *t_node){
    if(!t_node->left && !t_node->right){
        free(t_node->h_encode);
        free(t_node);
    }
    else if(t_node->left)
        clean_tree(t_node->left);
    else if(t_node->right)
        clean_tree(t_node->right);
}

void print_list(node *list){
    node *temp = list;
    while(temp){
        printf("%c: %d\n", temp->c, temp->freq);
        temp = temp->next;
    }   
}


