#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 500
#define MAX_FILE 10000

int create_dir(char *path);
int create_file(char *path);
int get_command();
int gnc(char** command, char** next_command, char** remaining);
int get_input(char* command, char** filepath, char* next_identifier, char*pre_identifier);
int cat(char* filepath);
int insert(char *filepath, char *str, int line_no, int start_pos);
void path_validation(char *path, char** second_path);
void string_validation(char initial[]);
int removef(char *filepath, int line_no, int start_pos, int size);
int removeb(char *filepath, int line_no, int start_pos, int size);


int main(){
    mkdir("root", 0777);
    while(1){
        if(!get_command()) printf("invalid command\n");
    }
}

int create_dir(char *path){
    char *remained_path;
    char *last_dir = NULL;

    last_dir = (char*) malloc(sizeof(path));
    remained_path = (char*) malloc(sizeof(path));
    last_dir = strrchr(path, '/');
    if(last_dir == NULL) {
        mkdir(path ,0777);
        return 1;
    }
    strncpy(remained_path, path, strlen(path) - strlen(last_dir));
    int result = create_dir(remained_path);
    if(*(path) == '/') mkdir(path + 1 ,0777);
    else mkdir(path, 0777);
    free(remained_path);
    return result;
}

int create_file(char *path){
    if(*path == '/') path = path + 1;
    else if(*path == '\"' && *(path + 1) == '/'){
        path = path + 2;
        path[strlen(path) - 2] = '\0';
    }
    else if(*path == '\"'){
        path = path + 1;
        path[strlen(path) - 1] = '\0';
    }
    path[strlen(path) - 1] = '\0';
    if(strchr(path, '/') == NULL){
        FILE *file = fopen(path, "w");
        if(file != NULL){
            fclose(file);
            return 1;
        }else{
            return 0;
        }
    }else{
        char *filename = strrchr(path, '/');
        char *dirpath;
        struct stat buff;
        dirpath = (char*) malloc(sizeof(path));
        strncpy(dirpath, path, strlen(path) - strlen(filename));
        create_dir(dirpath);
        FILE *file = fopen(path, "r");
        if(file == NULL){
            file = fopen(path, "w");
            if(file == NULL){
                printf("File creation failed.\n");
                return 1;
            }
            fclose(file);
            return 1;
        }
        else{
            printf("The given file already exists\n");
        }
        return 0;
    }
}

int get_command(){
    char *input;
    input = (char*) malloc(sizeof(char) * MAX_SIZE);
    fgets(input, MAX_SIZE, stdin);
    char *initial_command, *remaining;
    if(gnc(&input, &initial_command, &remaining) == 0) return 0;

    if(!strcmp(initial_command, "createfile")){
        char* filepath;
        char* input_format;
        if(gnc(&remaining, &input_format, &filepath) == 0) return 0;
        if(strcmp(input_format, "--file")) return 0;
        else{
            create_file(filepath);
            return 1;
        }
    }
    else if(!strcmp(initial_command, "cat")){
        char *filepath;
        if(!get_input(input, &filepath, "\n", "--file ")) return 0;
        cat(filepath);
        return 1;
    }
    else if(!strcmp(initial_command, "insertstr")){
        char *filepath, *str, *str_line, *str_start;
        int line_no;
        int start_pos;
        if(!get_input(input, &filepath, " --str", "--file ")) return 0;
        if(!get_input(input, &str, " --pos", "--str ")) return 0;
        if(!get_input(input, &str_line, ":", "--pos ")) return 0;
        if(!get_input(input, &str_start, "\n", ":")) return 0;
        line_no = atoi(str_line);
        start_pos = atoi(str_start);
        insert(filepath, str, line_no, start_pos);
        return 1;
    }
    else if(!strcmp(initial_command, "removestr")){
        char *filepath, *line_str, *start_str, *size_str;
        int line_no, start_pos, size;
        if(!get_input(input, &filepath, " --pos", "--file ")) return 0;
        if(!get_input(input, &line_str, ":", "--pos ")) return 0;
        if(!get_input(input, &start_str, " -size", ":")) return 0;
        if(!get_input(input, &size_str, " -f\n", "-size ") && !get_input(input, &size_str, " -b\n", "-size ")) return 0;
        char mode = input[strlen(input) - 2];
        if(mode != 'f' && mode != 'b') return 0;
        line_no = atoi(line_str);
        start_pos = atoi(start_str);
        size = atoi(size_str);
        if(mode == 'f') removef(filepath, line_no, start_pos, size);
        if(mode == 'b') removeb(filepath, line_no, start_pos, size);
        return 1;
    }

    return 0;
}

int gnc(char** command, char** next_command, char** remaining){
    *remaining = (char*) malloc(sizeof(*command));
    *next_command = (char*) malloc(sizeof(*command));
    char* a = strchr(*command, ' ');
    if(a == NULL) return 0;
    *remaining = a;
    strncpy(*next_command, *command, strlen(*command) - strlen(*remaining));
    (*remaining) ++;
    return 1;
}

int get_input(char* command, char** text, char* next_identifier, char*pre_identifier){
    int len;
    char *pre, *next;
    pre = (char*) malloc(sizeof(command));
    next = (char*) malloc(sizeof(command));
    pre = strstr(command, pre_identifier);
    if(pre == NULL) return 0;
    pre = pre + strlen(pre_identifier);
    next = strstr(command, next_identifier);
    if(next == NULL) return 0;
    if(next < pre) return 0;
    *text = (char*) malloc((strlen(pre) - strlen(next) - 1) * sizeof(char));
    strncpy(*text, pre, strlen(pre) - strlen(next));
    return 1;
}

int cat(char* filepath){
    if(*filepath == '/') filepath = filepath + 1;
    else if(*filepath == '\"' && *(filepath + 1) == '/'){
        filepath = filepath + 2;
        filepath[strlen(filepath) - 1] = '\0';
    }
    else if(*filepath == '\"'){
        filepath = filepath + 1;
        filepath[strlen(filepath) - 1] = '\0';
    }
    FILE* file = fopen(filepath, "r");
    if(file == NULL) {
        printf("File can't be opened\n");
        return 0;
    }
    char c = fgetc(file);
    while(c != EOF){
        printf("%c", c);
        c = fgetc(file);
    }
    fclose(file);
    return 1;
}

int insert(char *filepath, char *str, int line_no, int start_pos){
    string_validation(str);
    char* new_content;
    new_content = (char*) malloc(sizeof(char) * MAX_FILE);
    int line_counter=0, index_counter = 0;
    if(*filepath == '/') filepath = filepath + 1;
    else if(*filepath == '\"' && *(filepath + 1) == '/'){
        filepath = filepath + 2;
        filepath[strlen(filepath) - 1] = '\0';
    }
    else if(*filepath == '\"'){
        filepath = filepath + 1;
        filepath[strlen(filepath) - 1] = '\0';
    }

    FILE *file = fopen(filepath, "r");
    if(file == NULL) {
        printf("The given file doesn't exist\n");
        return 0;
    }

    char c = fgetc(file);
    while(c != EOF && line_counter != (line_no - 1)){
        new_content[strlen(new_content)] = c;
        if(c == '\n') line_counter ++;
        c = fgetc(file);
    }
    if(c == EOF && line_counter != (line_no - 1)){
        while(line_counter != (line_no)){
            new_content[strlen(new_content)] = '\n';
            line_counter++;
        }
    }
    while(c != EOF && index_counter != (start_pos)){
        new_content[strlen(new_content)] = c;
        index_counter++;
        c = fgetc(file);
    }
    if(c == EOF && index_counter != start_pos){
        while(index_counter != (start_pos)){
            new_content[strlen(new_content)] = ' ';
            index_counter++;
        }
    }
    for(int i=0; i<strlen(str); i++){
        new_content[strlen(new_content)] = str[i];
    }
    while(c != EOF){
        new_content[strlen(new_content)] = c;
        c = fgetc(file);
    }
    fclose(file);
    FILE *file1 = fopen(filepath, "w");
    fprintf(file1, "%s",new_content);
    fclose(file1);
    return 1;
}

void string_validation(char initial[]){
    int size = strlen(initial);
    if(initial[0] == '\"'){
        for(int i=0; i<size; i++) initial[i] = initial[i+1];
        initial[size - 2] = '\0';
        size -= 2;
    }
    for(int i=0; i<size - 1; i++){
        if(initial[i] == '\\' && initial[i+1] == 'n'){
            if(i == 0 || initial[i-1] != '\\'){
                for(int j=i; j<size; j++){
                    initial[j] = initial[j+1];
                }
                size--;
                initial[i] = '\n';
            }
        }
        else if(initial[i] == '\\' && initial[i+1] == '\"'){
            if(i == 0 || initial[i-1] != '\\'){
                for(int j=i; j<size; j++){
                    initial[j] = initial[j+1];
                }
                size--;
                initial[i] = '\"';
            }
        }
        else if(initial[i] == initial[i+1] && initial[i] == '\\'){
            for(int j=i; j<size; j++){
                initial[j] = initial[j+1];
            }
            size--;
        }
    }
}

int removef(char *filepath, int line_no, int start_pos, int size){
    if(*filepath == '/') filepath = filepath + 1;
    else if(*filepath == '\"' && *(filepath + 1) == '/'){
        filepath = filepath + 2;
        filepath[strlen(filepath) - 1] = '\0';
    }
    else if(*filepath == '\"'){
        filepath = filepath + 1;
        filepath[strlen(filepath) - 1] = '\0';
    }
    char* new_content;
    new_content = (char*) malloc(sizeof(char) * MAX_FILE);
    int line_counter = 0, index_counter = 0, del_counter = 0;
    FILE *file = fopen(filepath, "r");
    if(file == NULL) {
        printf("The given file doesn't exist\n");
        return 0;
    }
    char c = fgetc(file);
    while(c != EOF && line_counter != (line_no - 1)){
        new_content[strlen(new_content)] = c;
        if(c == '\n') line_counter ++;
        c = fgetc(file);
    }
    while(c != EOF && index_counter != (start_pos + 1)){
        new_content[strlen(new_content)] = c;
        index_counter++;
        c = fgetc(file);
    }
    while (del_counter < size){
        c = fgetc(file);
        del_counter++;
    }
    while(c != EOF){
        new_content[strlen(new_content)] = c;
        c = fgetc(file);
    }
    fclose(file);
    FILE *file1 = fopen(filepath, "w");
    fprintf(file1, "%s",new_content);
    fclose(file1);
    return 1;
}

int removeb(char *filepath, int line_no, int start_pos, int size){
    if(*filepath == '/') filepath = filepath + 1;
    else if(*filepath == '\"' && *(filepath + 1) == '/'){
        filepath = filepath + 2;
        filepath[strlen(filepath) - 1] = '\0';
    }
    else if(*filepath == '\"'){
        filepath = filepath + 1;
        filepath[strlen(filepath) - 1] = '\0';
    }
    char* new_content;
    new_content = (char*) malloc(sizeof(char) * MAX_FILE);
    int line_counter = 0, index_counter = 0, del_counter = 0;
    FILE *file = fopen(filepath, "r");
    if(file == NULL) {
        printf("The given file doesn't exist\n");
        return 0;
    }
    char c = fgetc(file);
    while(c != EOF && line_counter != (line_no - 1)){
        new_content[strlen(new_content)] = c;
        if(c == '\n') line_counter ++;
        c = fgetc(file);
    }
    while(c != EOF && index_counter != (start_pos)){
        new_content[strlen(new_content)] = c;
        index_counter++;
        c = fgetc(file);
    }
    while(del_counter < size){
        new_content[strlen(new_content) - 1] = '\0';
        del_counter ++;
    }
    printf("%s\n", new_content);
    while(c != EOF){
        new_content[strlen(new_content)] = c;
        c = fgetc(file);
    }
    fclose(file);
    FILE *file1 = fopen(filepath, "w");
    fprintf(file1, "%s",new_content);
    fclose(file1);
    return 1;
}



