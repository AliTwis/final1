#include <ncurses.h>
#include <stdio.h> 
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

#define MAX 1000
#define WIDTH 130

int str_len(char strr[]){
    char ans = strlen(strr);
    if (strr[ans - 1] == '\r') return ans - 1;
    if (strr[ans - 2] == '\r') return ans - 2;
    if (strr[ans - 1] == '\n') return ans - 1;
    else return ans;
}

int str_cmp(char first[], char second[]){
    int t1 = str_len(first);
    int t2 = str_len(second);
    if (t1 != t2) return 0;
    for (int i = 0; i < t1; i++){
        if (first[i] != second[i]) return 0;
    }
    return 1;
}

int check_exist(char file_name[]){
    if (access(file_name, F_OK) != 0){
        return 0;
    }
    else{
        return 1;
    }
}

int save(char file_name[100]){
    char save_file[100] = "saving/";
    for (int i = 0; i < str_len(file_name); i++){
        save_file[7 + i] = file_name[i];
    }
    save_file[7 + str_len(file_name)] = '\0';
    FILE *file = fopen(file_name, "r");
    remove(save_file);
    FILE *filesave = fopen(save_file, "w");
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        if (line[0] != '\0') fprintf(filesave, "%s", line);
    }
    fclose(file);
    fclose(filesave);
    return 1;
}

int undo(char file_name[100]){
    FILE *file = fopen(file_name, "r+");
    char save_file[100] = "saving/";
    for (int i = 0; i < str_len(file_name); i++){
        save_file[7 + i] = file_name[i];
    }
    save_file[7 + str_len(file_name)] = '\0';
    FILE *filesave = fopen(save_file, "r");
    FILE *filetemp = fopen("saving/root/temp_file.txt", "w+");
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    fclose(file);
    file = fopen(file_name, "w");
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, filesave);
        if (line[0] != '\0') fprintf(file, "%s", line);
    }
    fclose(file);
    fclose(filesave);
    fclose(filetemp);
    remove(save_file);
    rename("saving/root/temp_file.txt", save_file);
    return 1;
}

int make_dir(char dir_name[]){
    int check = mkdir(dir_name);
    if (check == -1) return -1;
    else return 1;
}

int dir_addresses(char file_address[], char address[][100]){
    int t = 0, j = 0;
    char letter = file_address[0];
    int letter_index = 0;
    while (letter != '\0'){
        if (letter == '/') t++;
        letter = file_address[++letter_index];
    }
    t++;
    letter = file_address[0];
    letter_index = 0;
    int start = 0;
    while (letter != '\0'){
        if (letter != '/'){
            for (int i = start; i < t; i++){
                address[i][j] = letter;
            }
        }
        else{
            address[start][j] = '\0';
            start++;
            for (int i = start; i < t; i++){
                address[i][j] = letter;
            }
        }
        j++;
        letter = file_address[++letter_index];
        
    }
    return t;
}

int create_file(char address[100]){
    if (access(address, F_OK) == 0){
        return 0;
    }
    FILE *newfile = fopen(address, "w");
    fclose(newfile);
    return 1;
}

int make_message(char old_message[], char new_message[]){//making \n and making " from \"
    int head = 0;
    for (int i = 0; i < 100; i++){
        if(old_message[i] == '\0') break;
        if (old_message[i] != '\\'){
            new_message[head] = old_message[i];
            head++;
        }
        else{
            if (old_message[i + 1] == 'n'){
                new_message[head] = '\n';
                head++;
                i++;
            }
            else{
                new_message[head] = old_message[i + 1];
                head++;
                i++;
            }
        }
    }
    new_message[head] = '\0';
    return head;
}

char inp(char start, char input[], WINDOW *command){//input without "
    input[0] = start;
    int index = 1;
    char letter;
    letter = wgetch(command);
    while (letter != ' ' && letter != '\n'){
        input[index++] = letter;
        letter = wgetch(command);
    }
    input[index] = '\0';
    if (letter == ' '){
        return ' ';
    }
    else return '\n';
}

int inp_q(char input[], WINDOW *command){//input with "
    char a1 = 'a';
    char a2 = 'a';
    char a3 = 'a';
    char index = 0;
    a1 = wgetch(command);
    while (!((a1 == '"' && a2 != '\\') || (a1 == '"' && a2 == '\\' && a3 == '\\'))){
        input[index++] = a1;
        a3 = a2;
        a2 = a1;
        a1 = wgetch(command);
    }
    if (a1 == '"' && a2 == '\\' && a3 == '\\'){
        input[index] = '\0';
        index--;
    }
    else input[index] = '\0';
    return index;
}

void insert(char file_name[100], char message[500], int ln, int pos){
    save(file_name);
    FILE *file = fopen(file_name, "r+");
    FILE *filetemp = fopen("temp_file.txt", "w");
    //declaring line for getting each line of the file. rest is for saving rest of the file from the given position.
    char line[300]; line[0] = '`';
    char rest[100][300];
    //jumping to the line
    for (int l = 0; l < ln - 1; l++){
        fgets(line, 100, file);
        fprintf(filetemp, "%s", line);
    }
    if (ln == 1){
        fseek(file, 0, SEEK_SET);
    }
    //goint to pos
    char letter;
    for (int i = 0; i < pos; i++){
        fscanf(file, "%c", &letter);
        fprintf(filetemp, "%c", letter);
    }
    //inserting
    fprintf(filetemp, "%s", message);
    //printing rest of the file
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 100, file);
        fprintf(filetemp, "%s", line);
    }
    //changing names
    fclose(file);
    fclose(filetemp);
    remove(file_name);
    rename("temp_file.txt", file_name);
}

void cat(char file_name[100]){
    FILE *file = fopen(file_name, "r");
    FILE *fileans = fopen("ans.txt", "w");
    char line[100];
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 100, file);
        fprintf(fileans, "%s", line);
    }
    fclose(file);
    fclose(fileans);
}

void rmv(char file_name[100], int ln, int pos, int size, char dir){
    save(file_name);
    FILE *file = fopen(file_name, "r+");
    //declaring line for getting each line of the file. rest is for saving rest of the file from the given position.
    char line[300]; line[0] = '`';
    char rest[100][300];
    //jumping to the line
    for (int l = 0; l < ln - 1; l++){
        fgets(line, 300, file);
    }

    //goint to pos
    if (ln == 1) fseek(file, pos + 1, SEEK_SET);
    else fseek(file, pos + 1, SEEK_CUR);
    // if (dir == 'f') fseek(file, size, SEEK_CUR);
    // if (dir == 'f') fseek(file, -1, SEEK_CUR);
    if (dir == 'b') fseek(file, -size - 1, SEEK_CUR);
    //saving this place in 'where' variable
    int where = ftell(file);
    //saving rest of the file
    //removing
    // until where - size - 1
    FILE *filetemp = fopen("root/temp_file.txt", "w");
    fseek(file, 0, SEEK_SET);
    char letter;
    for (int i = 0; i < where; i++){
        fscanf(file, "%c", &letter);
        fprintf(filetemp, "%c", letter);
    }
    fseek(file, size, SEEK_CUR);
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 100, file);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    fclose(filetemp);
    fclose(file);
    remove(file_name);
    rename("root/temp_file.txt", file_name);

}

int cpy(char file_name[100], int ln, int pos, int size, char dir){
    FILE *file = fopen(file_name, "r");
    char line[300];//saving lines
    for (int i = 0; i < ln - 1; i++){
        fgets(line, 300, file);
    }
    char letter;//skipping letters until pos
    for (int i = 0; i <= pos; i++){
        fscanf(file, "%c", &letter);
    }
    char ans[300];
    if (dir == 'b') fseek(file, -size - 1, SEEK_CUR);
    for (int i = 0; i < size; i++){
        fscanf(file, "%c", &ans[i]);
    }
    ans[size] = '\0';
    fclose(file);
    //copying
    FILE *filecpy = fopen("clipboard.txt", "w");
    fprintf(filecpy, "%s", ans);
    fclose(filecpy);
}

int cut(char file_name[100], int ln, int pos, int size, char dir){
    cpy(file_name, ln, pos, size, dir);
    rmv(file_name, ln, pos, size, dir);

}

int ppaste(char file_name[100], int ln, int pos){
    save(file_name);
    FILE *file = fopen(file_name, "r");
    FILE *filetemp = fopen("root/temp_file.txt", "w");
    char line[300]; line[0] = '`';//saving each line of the file until the target
    for (int i = 0; i < ln - 1; i++){
        fgets(line, 300, file);
        fprintf(filetemp, "%s", line);
    }
    char letter;//letters in ln until pos
    for (int i = 0; i < pos; i++){
        fscanf(file, "%c", &letter);
        fprintf(filetemp, "%c", letter);
    }
    //paste
    FILE *filecpy = fopen("clipboard.txt", "r");
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 100, filecpy);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 100, file);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    fclose(filecpy);
    fclose(file);
    fclose(filetemp);
    remove(file_name);
    rename("root/temp_file.txt", file_name);
}

int star(char word[]){
    int size = str_len(word);
    if (word[0] == '*') return -1;
    if ((word[size - 1] == '*' && word[size - 2] != '\\')|| (word[size - 1] == '*' && word[size - 2] == '\\' && word[size - 3] == '\\')){
        return 1;
    }
    else return 0;
}

int split_space(char line[], char final_line[][100]){
    int size = str_len(line);
    int head = 0;
    int index = 0;
    for (int i = 0; i < size + 1; i++){
        if (line[i] != ' ' && line[i] != '\0' && line[i] != '\n' && line[i] != '\r'){
            final_line[head][index] = line[i];
            index++;
        }
        else {
            final_line[head][index] = '\0';
            head++;
            index = 0;
        }
        
        if (line[i] == '\0' || line[i] == '\n') break;
    }
    return head;
}

int findd_cmp(char target[], int t_star, char check[]){
    int size_tar = str_len(target);
    int size_check = str_len(check);
    if (t_star == -1){
        if (size_check < size_tar) return 0;
        for (int i = size_check - size_tar; i < size_check; i++){
            if (target[i - (size_check - size_tar)] != check[i]) return 0;
        }
        return 1;
    }
    if (t_star == 0){
        if (size_check != size_tar) return 0;
        for (int i = 0; i < size_check; i++){
            if (target[i] != check[i]) return 0;
        }
        return 1;
    }
    if (t_star == 1){
        if (size_check < size_tar) return 0;
        for (int i = 0; i < size_tar; i++){
            if (target[i] != check[i]) return 0;
        }
        return 1;
    }
}

int make_message_findd(char old_message[], char new_message[]){//making \n and making " from \"
    int head = 0;
    for (int i = 0; i < 500; i++){
        if(old_message[i] == '\0') break;
        if (old_message[i] != '\\' && old_message[i] != '*'){
            new_message[head] = old_message[i];
            head++;
        }
        else if(old_message[i] == '*'){
        }
        else{
            if (old_message[i + 1] == 'n'){
                new_message[head] = '\n';
                head++;
                i++;
            }
            else{
                new_message[head] = old_message[i + 1];
                head++;
                i++;
            }
        }
    }
    new_message[head] = '\0';
    return head;
}

int findd(char target[][100], int target_size, char file_name[], int star_list[], int chand/*which one OR -1:print all, -2: return tedad*/, int results[]){
    FILE *file = fopen(file_name, "r");
    char word[50];
    int line_pos = 0;//saving the SEEK_CUR for end of each line until we find a match
    int tedad = 0;//amount of matches
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        char words[300][100];
        int word_tedad = split_space(line, words);
        for (int index = 0; index <= word_tedad - target_size; index++){
            int match = 1;//checking if this location is a match
            for (int i = 0; i < target_size; i++){
                if (!findd_cmp(target[i], star_list[i], words[index + i])){
                    match = 0;
                    break;
                }
            }
            if (match){
                tedad++;
                int space_amounts = 0;
                char letter = line[0];
                int j = 0;
                while(space_amounts != index){
                    if (letter == ' ') space_amounts++;
                    letter = line[++j];
                }
                if (tedad == chand){//finding where we are in the line
                    return line_pos + j;
                }
                if (chand == -1){
                    results[tedad - 1] = line_pos + j;
                }
                

            }
            else{
                // printf("%s %s %d\n", words[index], target[0], target_size);
                // printf("%d\n", word_tedad - target_size);
            }
        }
        line_pos = ftell(file);
    }
    fclose(file);
    if (chand < 0) return tedad;
    return -1;
}

int findd_word(char target[][100], int target_size, char file_name[], int star_list[], int chand/*which one OR -1:print all, -2: return tedad*/, int results[]){
    FILE *file = fopen(file_name, "r");
    char word[50];
    int line_pos = 0;//saving the SEEK_CUR for end of each line until we find a match
    int tedad = 0;//amount of matches
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        char words[300][100];
        int word_tedad = split_space(line, words);
        for (int index = 0; index <= word_tedad - target_size; index++){
            int match = 1;//checking if this location is a match
            for (int i = 0; i < target_size; i++){
                if (!findd_cmp(target[i], star_list[i], words[index + i])){
                    match = 0;
                    break;
                }
            }
            if (match){
                tedad++;
                int space_amounts = 0;
                char letter = line[0];
                int j = 0;
                while(space_amounts != index){
                    if (letter == ' ') space_amounts++;
                    letter = line[++j];
                }
                if (tedad == chand){//finding where we are in the line
                    return line_pos + index + 1;
                }
                if (chand == -1){
                    results[tedad - 1] = line_pos + index + 1;
                }
                

            }
        }
        line_pos += word_tedad;
    }
    fclose(file);
    if (chand < 0) return tedad;
    return -1;
}

int replace(char file_name[], char target[][100],  int target_size, char new[], int chand/*-1: all*/, int star_list[]){
    save(file_name);
    FILE *file = fopen(file_name, "r");
    FILE *filetemp = fopen("temp_file.txt", "w");
    char word[50];
    int tedad = 0;//amount of matches
    int which_line = 0;//which line are we in file
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        if (which_line && line[0] != '\0') fprintf(filetemp, "\n");//going to next line in temp file
        char words[300][100];
        int word_tedad = split_space(line, words);
        for (int index = 0; index < word_tedad ; index++){
            int match = 1;//checking if this location is a match
            if (index <= word_tedad - target_size) {
                for (int i = 0; i < target_size; i++){
                    if (!findd_cmp(target[i], star_list[i], words[index + i])){
                        match = 0;
                        break;
                    }
                }
                if (!match){
                    fprintf(filetemp, "%s", words[index]);
                }
                else{
                    tedad++;
                    if (tedad == chand || chand == -1) {
                        fprintf(filetemp, "%s", new);
                        index += target_size - 1;
                    }
                    else{
                        fprintf(filetemp, "%s", words[index]);
                    }
                }
            }
            else{
                fprintf(filetemp, "%s", words[index]);
            }
            if (match && index < word_tedad - 1) fprintf(filetemp, " ");
            else if(!match && index < word_tedad - 1) fprintf(filetemp, " ");
        }
        
        which_line++;
    }
    fprintf(filetemp, '\0');
    fclose(file);
    fclose(filetemp);
    remove(file_name);
    rename("temp_file.txt", file_name);
    if (tedad == 0) return 0;
    else return 1;
}

int grep(char file_name[], char target[], int t_size, int option/*0:printing lines 1:tedad 2:if a file has it*/){
    int tedad = 0;
    FILE *file = fopen(file_name, "r+");
    FILE *fileans = fopen("ans.txt", "a");
    //declaring line for getting each line of the file.
    char line[300]; line[0] = '`';
    while(line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        //counting how many characters does this line have
        char temp = line[0];
        int line_size = 0;
        while (temp != '\0'){
            temp = line[++line_size];
        }
        for (int letter = 0; letter <= line_size; letter++){    
            int is_match = 1;
            for (int ch = 0; ch < t_size; ch++){
                if (target[ch] != line[letter + ch]){
                    is_match = 0;
                    break;
                }
            }
            if (is_match){
                tedad++;
                if (option == 1) break;
                if (option == 0){
                    fprintf(fileans, "%s", line);
                    if (line[line_size - 1] != '\n') fprintf(fileans, "\n");
                    break;
                }
                if (option == 2){
                    fclose(file);
                    return 1;
                }
            }   
        }
    }
    fclose(fileans);
    fclose(file);
    if (option == 0) return 1;
    if (option == 1) return tedad;
    if (option == 2) return 0;
}

int text_cmp(char file_name1[], char file_name2[]){
    FILE *file1 = fopen(file_name1, "r");
    FILE *file2 = fopen(file_name2, "r");
    FILE *fileans = fopen("ans.txt", "w");
    char line1[300]; line1[0] = '`';
    char line2[300]; line2[0] = '`';
    int big1 = 0, big2 = 0;
    int which_line = 1;//seeing which line we are
    while(line1[0] != '\0' || line2[0] != '\0'){
        line1[0] = '\0'; line2[0] = '\0';
        fgets(line1, 300, file1);
        fgets(line2, 300, file2);
        if(line1[0] == '\0'){
            big2 = 1;
            break;
        }
        if (line2[0] == '\0'){
            big1 = 1;
            break;
        }
        //counting line1 and line2 length
        int tedad1 = 0, tedad2 = 0;
        char temp = line1[0];
        while(temp != '\0' && temp != '\n'){
            temp = line1[++tedad1];
        }
        temp = line2[0];
        while (temp != '\0' && temp != '\n'){
            temp = line2[++tedad2];
        }
        if (tedad1 != tedad2){
            fprintf(fileans, "====<%d>====\n", which_line);
            fprintf(fileans, "%s", line1); if (line1[tedad1] != '\n') fprintf(fileans, "\n");
            fprintf(fileans, "%s", line2); if (line2[tedad2] != '\n') fprintf(fileans, "\n");
        }
        else{
            for (int i = 0; i < tedad1; i++){
                if (line1[i] != line2[i]){
                    fprintf(fileans, "====<%d>====\n", which_line);
                    fprintf(fileans, "%s", line1); if (line1[tedad1] != '\n') fprintf(fileans, "\n");
                    fprintf(fileans, "%s", line2); if (line2[tedad2] != '\n') fprintf(fileans, "\n");
                    break;
                }
            }
        }
        which_line++;
    }
    if (big1){
        int last_line = which_line;
        char line[300]; line[0] = '`';
        while(line[0] != '\0'){
            line[0] = '\0';
            fgets(line, 300, file1);
            if (line[0] != '\0') last_line++;
        }
        fseek(file1, 0, SEEK_SET);
        for (int i = 0; i < which_line - 1; i++){
            fgets(line, 300, file1);
        }
        fprintf(fileans, "====<file1 is bigger:%d - %d>====\n", which_line, last_line);
        line[0] = '`';
        while (line[0] != '\0'){
            line[0] = '\0';
            fgets(line, 300, file1);
            fprintf(fileans, "%s", line);
            int tedad = 0;
            char temp = line[0];
            while(temp != '\0'){
                temp = line[++tedad];
            }
            // if (line[tedad - 1] != '\n') printf("\n");
        }
    }
    else if (big2){
        int last_line = which_line;
        char line[300]; line[0] = '`';
        while(line[0] != '\0'){
            line[0] = '\0';
            fgets(line, 300, file2);
            if (line[0] != '\0') last_line++;
        }
        fseek(file2, 0, SEEK_SET);
        for (int i = 0; i < which_line - 1; i++){
            fgets(line, 300, file2);
        }
        fprintf(fileans, "====<file2 is bigger:%d - %d>====\n", which_line, last_line);
        line[0] = '`';
        while (line[0] != '\0'){
            line[0] = '\0';
            fgets(line, 300, file2);
            fprintf(fileans, "%s", line);
            int tedad = 0;
            char temp = line[0];
            while(temp != '\0'){
                temp = line[++tedad];
            }
            // if (line[tedad - 1] != '\n') printf("\n");
        }
    }
    fclose(file1);
    fclose(file2);
    fclose(fileans);
}

int dirtree(char dir_cur[], int depth, int first_depth){
    if (depth == 0) return 1;
    DIR *directory;
    directory = opendir(dir_cur);
    struct dirent *entity;
    int dir_size = strlen(dir_cur);
    while ((entity = readdir(directory)) != NULL){
        char *ent_name = (entity->d_name);
        if (((entity->d_name)[0] == '.' && strlen(ent_name) == 1) || (ent_name[0] == '.' && ent_name[1] == '.' && str_len(ent_name) == 2)) continue;
        int ent_size = strlen(ent_name);
        char next_dir[100];
        for (int i = 0; i < dir_size; i++){
            next_dir[i] = dir_cur[i];
        }
        next_dir[dir_size] = '/';
        for (int i = dir_size + 1; i < dir_size + ent_size + 1; i++){
            next_dir[i] = ent_name[i - dir_size - 1];
        }
        next_dir[dir_size + ent_size + 1] = '\0';
        DIR *temp_dir;
        temp_dir = opendir(next_dir);
        if (temp_dir != NULL){
            FILE *fileans = fopen("ans.txt", "a");
            for (int j = 0; j < 2 * (first_depth - depth); j++){
                fprintf(fileans, "-");
            }
            fprintf(fileans, "%s:\n", ent_name);
            fclose(fileans);
            dirtree(next_dir, depth - 1, first_depth);
        }
        else{
            FILE *fileans = fopen("ans.txt", "a");
            for (int j = 0; j < 2 * (first_depth - depth); j++){
                fprintf(fileans, "-");
            }
            fprintf(fileans, "%s\n", ent_name);
            fclose(fileans);
        }
        closedir(temp_dir);

        
    }
    
    return 1;
}

int close_pair(char file_name[]){
    save(file_name);
    FILE *file = fopen(file_name, "r");
    FILE *filetemp = fopen("root/temp_file.txt", "w");
    char line[300]; line[0] = '`';
    int is_p = 0;//seeing if we have found a {
    int br = 0;
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, file);
        int space_amount = 0;
        char letter = line[0];
        int temp_index = 0;//finding space amounts
        while (letter == ' '){
            letter = line[++temp_index];
            space_amount++;
        }
        for (int i = 0; i < space_amount; i++){
            fprintf(filetemp, " " );
        }
        int letter_index = space_amount;//which letter of the line are we
        letter = line[space_amount];
        //looking for {
        int space_changed = 0;//checking if we changed the value of space_amount
        int new_line = 1;
        int fnew_line = 0;
        while(letter != '\0'){
            if (letter != '{' && letter != '}'){
                if (letter == ' '){
                    int k = letter_index;
                    while (line[k] == ' '){
                        k++;
                    }
                    if (line[k] != '{' && !fnew_line){
                        for (int i = 0; i < k - letter_index; i++){
                            fprintf(filetemp, " ");
                        }
                        letter_index = k - 1;
                        new_line = 0;
                        fnew_line = 0;
                    }
                    else{
                        letter_index = k - 1;
                    }
                }
                else {
                    fprintf(filetemp, "%c", letter);
                    new_line = 0;
                    fnew_line = 0;
                }
            }
            else if (letter == '{') {
                if (!new_line && !fnew_line && line[letter_index - 1] != ' ' && letter_index != 0 || (!new_line && letter_index && space_amount != 0)){//checking if there is a space behind { or not
                    fprintf(filetemp, " {");
                }
                else fprintf(filetemp, "{");
                if(line[letter_index + 1] != '\n'){
                    if (line[letter_index + 1] != '}'){
                        fprintf(filetemp, "\n");
                        for (int i = 0; i < space_amount + 4; i++){
                            fprintf(filetemp, " ");
                        }
                    }
                    space_amount += 4; space_changed = 1;
                    is_p = 1;
                    new_line = 1;
                    fnew_line = 1;
                    
                }
            }
            else{
                if (space_changed) space_amount -= 4;
                if (letter_index != space_amount){
                    fprintf(filetemp, "\n");
                    for (int i = 0; i < space_amount; i++){
                        fprintf(filetemp, " ");
                    }
                }
                fprintf(filetemp, "}");
                if (line[letter_index + 1] != '\n'){
                    int no = 0;
                    if (line[letter_index + 1 == ' ']){
                        int k = letter_index + 1;
                        while (line[k] == ' '){
                            k++;
                        }
                        if (line[k] == '}') no = 1;
                    }
                    
                    if (line[letter_index + 1] != '}' && !no) {
                        fprintf(filetemp, "\n");
                        new_line = 1;
                        fnew_line = 1;
                        for (int i = 0; i < space_amount; i++){
                            fprintf(filetemp, " ");
                        }
                    }
                    
                    if (line[letter_index + 1] == '{' && space_amount){
                        fprintf(filetemp, "\n");
                        for (int i = 0; i < space_amount - 1; i++){
                            fprintf(filetemp, " ");
                        }
                    }
                }
            }
            letter = line[++letter_index];
            
        }
    }
    fclose(file);
    fclose(filetemp);
    remove(file_name);
    rename("root/temp_file.txt", file_name);
    // if (is_p != 0) close_pair(file_name);
    return 1;
}

void print_ans(){
    FILE *fileans = fopen("ans.txt", "r");
    char line[300]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, 300, fileans);
        if (line[0] != '\0') printf("%s", line);
    }
    printf("\n");
    fclose(fileans);
}

void armann(char arr[]){
    FILE *fileans = fopen("ans.txt", "r");
    char letter = '`';
    int arr_index = 0;
    while (fscanf(fileans, "%c", &letter) != EOF){
        arr[arr_index++] = letter;
    }
    arr[arr_index] = '\0';
    fclose(fileans);
}

int fileSize(char file_name[]){
    FILE *file = fopen(file_name, "r");
    int ans = 0;
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (line[0] != '\0') ans++;
    }
    fclose(file);
    return ans;
}

int mmax(int a, int b){
    if (a > b) return a;
    else return b;
}

int min(int a, int b){
    if (a < b) return a;
    return b;
}

int line_len(char line[]){
    int ans = strlen(line);

}

int lineSize(char file_name[], int lines[]){
    FILE *file = fopen(file_name, "r");
    int last_line = 0;
    int index = 0;
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (line[0] != '\0') {
            lines[index++] = strlen(line);
        }
        last_line = ftell(file);
    }
    fclose(file);
    return fileSize(file_name);
}

int RLineSize(char file_name[], int lines[]){
    FILE *file = fopen(file_name, "r");
    int last_line = 0;
    int index = 0;
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (line[0] != '\0') {
            lines[index++] = str_len(line);
        }
        last_line = ftell(file);
    }
    fclose(file);
    return fileSize(file_name);
}

void printstat(WINDOW *stat, WINDOW *command, char mode[]){
    wclear(stat);
    box(stat, 0, 0);
    mvwprintw(stat, 1, 1, "%s", mode); wrefresh(stat);
    wclear(command);
    box(command, 0, 0);
    wmove(command, 1, 1); wrefresh(command);
}

void printFile(WINDOW *filewin, char file_name[], int ln){
    wclear(filewin);
    box(filewin, 0, 0);
    FILE *file = fopen(file_name, "r");
    char line[MAX];
    for (int i = 0; i < ln - 1; i++){
        fgets(line, MAX, file);
    }
    line[0] = '\0';
    wmove(filewin, 1, 0);
    for (int i = 0; i < 18; i++){
        fgets(line, MAX, file);
        if (line[0] == '\0') break;
        int a = strlen(line);
        if (line[a - 2] == '\r' && line[a - 1] == '\n'){
            if (line[a - 1] == '\n') line[a - 2] = '\n';
            line[a - 1] = '\0';
        }
        // wprintw(filewin, "%d: %s", min(ln + i, mmax(1, fileSize(file_name) - 17) + i), line);
        wprintw(filewin, "%d: %s", ln + i, line);
        line[0] = '\0';
    }
    wrefresh(filewin);
    fclose(file);
}

void inDeitel(char file1[], char file2[]){
    FILE *file = fopen(file1, "r");
    FILE *deitel = fopen(file2, "w");
    char line[MAX];
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        
        if (line[0] != '\0') {
            fprintf(deitel, "%s", line);
            // for (int i = 0; i < str_len(line); i++){
            //     fprintf(deitel, "%c", line[i]);
            // }
            // fprintf(deitel, "%c", "\n");
        }
    }
    fclose(file);
    fclose(deitel);
}

void StrToStr(char fst[], char sec[]){
    int index = 0;
    for (int i = 0; i < str_len(fst); i++){
        if (fst[i] != '\r') sec[index++] = fst[i];
    }
    sec[index++] = '\0';
}

int posToLine(int place, int lines[]){
    int index = 0;
    long long int sum = lines[0] - 1;
    while (place > sum){
        index++;
        sum += lines[index];
    }
    return index + 1;
}

int posToPos(int place, int lines[], int fst){
    int ln = posToLine(place, lines);
    int sum = 0;
    for (int i = 0; i < ln - 1; i++){
        sum += lines[i];
    }
    return place - sum - 1;
}

int numSize(int a){
    int ans = 0;
    while (a != 0){
        a /= 10;
        ans++;
    }
    return ans + 2;
}

int printFileFind(WINDOW *filenamewin, WINDOW *filewin, char target[][100],  int target_size, int chand/*-1: all*/, int star_list[], char message[], int sizes[], int fst){
    // FILE *file = fopen("saving/root/temp.txt", "w"); fclose(file);
    wclear(filewin); box(filewin, 0, 0);
    inDeitel("deitel.txt", "temp.txt");
    int deitel_results[MAX] = {0};
    findd(target, target_size, "deitel.txt", star_list, -1, deitel_results);
    int original_chand = chand;
    // endwin();
    // printf("%s\n", target[0]);

    replace("temp.txt", target, target_size, "TTTTT", -1, star_list);
    int stars[MAX] = {0};
    int results[MAX] = {0};
    int tedad = findd("TTTTT", 1, "temp.txt", stars, -1, results);
    int lines[MAX] = {0}; lineSize("temp.txt", lines);
    char extra[MAX];
    int ln = posToLine(results[chand - 1], lines);
    FILE *file = fopen("temp.txt", "r");
    FILE *deitel = fopen("deitel.txt", "r");
    for (int i = 0; i < ln - 1; i++){
        fgets(extra, MAX, file);
        fgets(extra, MAX, deitel);
    }
    // if (ln > 1) fseek(file, -1, SEEK_CUR);
    // if (ln > 1) fseek(deitel, -1, SEEK_CUR);
    char word[MAX];
    char word2[MAX];
    char next = '`';
    int lineAmount = ln;
    char deitelNext = '`';
    mvwprintw(filewin, 1, 0, "%d: ", ln);
    while (fscanf(file, "%c", &next) != EOF && lineAmount - ln + 1 < 18){
        fscanf(deitel, "%c", &deitelNext);
        if (next == '\n' || next == ' '){
            while (next == '\n' || next == ' '){
                if (next == '\n') lineAmount++;
                wprintw(filewin, "%c", next);
                if (next == '\n') wprintw(filewin, "%d: ", lineAmount);
                fscanf(file, "%c", &next);
                fscanf(deitel, "%c", &deitelNext);
            }
            // fseek(file, -1, SEEK_CUR);
            // fseek(deitel, -1, SEEK_CUR);
        }
        // else if(next == ' '){
        //     while (next == ' ' || next == '\n'){
        //         if (next == '\n') lineAmount++;
        //         wprintw(filewin, "%c", next);
        //         if (next == '\n') wprintw(filewin, "%d: ", lineAmount);
        //         fscanf(file, "%c", &next);
        //         fscanf(deitel, "%c", &deitelNext);
        //     }
            // fseek(file, -1, SEEK_CUR);
            // fseek(deitel, -1, SEEK_CUR);
        // }
        if (next != '\r') {
            fseek(file, -1, SEEK_CUR);
            fseek(deitel, -1, SEEK_CUR);
            fscanf(file, "%s", word);
            if (str_cmp(word, "TTTTT") == 0){
                fscanf(deitel, "%s", word2);
                wprintw(filewin, "%s", word2);
            }
            else{
                start_color();
                init_pair(1, COLOR_BLACK, COLOR_GREEN);
                wattron(filewin, COLOR_PAIR(1));
                for (int k = 0; k < target_size; k++){
                    fscanf(deitel, "%s", word2);
                    wprintw(filewin, "%s", word2);
                }
                // fseek(deitel, -1, SEEK_CUR);
                wattroff(filewin, COLOR_PAIR(1));
            }
        }
    }
    wmove(filewin, 1, numSize(ln) + posToPos(deitel_results[original_chand - 1], sizes, fst) + 1);
    wrefresh(filewin);
    fclose(deitel);
    fclose(file);
    remove("temp.txt");
    return numSize(ln) + posToPos(deitel_results[original_chand - 1] , sizes, fst) + 1;


}

int getNum(WINDOW *command){
    char letter = wgetch(command);
    int ans = 0;
    int index = 0;
    while (letter >= 48 && letter <= 57){
        ans = ans * 10 + letter - 48;
        letter = wgetch(command);
    }
    return ans;
}

int main(){
    initscr();
    start_color();
    // noraw();
    //making different windows
    WINDOW *filewin = newwin(20, WIDTH, 0, 0);
    box(filewin, 0, 0);
    refresh(); wrefresh(filewin);
    WINDOW *stat = newwin(3, 10, 20, 0);
    box(stat, 0, 0);
    WINDOW *fileNameWin = newwin(3, WIDTH - 10, 20, 10);
    box(fileNameWin, 0, 0);
    refresh(); wrefresh(stat); wrefresh(fileNameWin);
    WINDOW *command = newwin(3, WIDTH, 23, 0);
    box(command, 0, 0);
    refresh(); wrefresh(command);
    FILE *file = fopen("deitel.txt", "w");  fclose(file);
    int mode = 0; // 0:NORMAL, 1:VISUAL, 2:INSNERT  

    int whichLine = 1;
    printFile(filewin, "deitel.txt", whichLine);
    int cursorY = 1, cursorX = 3; wmove(filewin, cursorY, cursorX);wrefresh(filewin); 
    int lines[MAX] = {0};
    int tedad = lineSize("deitel.txt", lines);
    int RLines[MAX] = {0}; RLineSize("deitel.txt", RLines);

    char orgfile[MAX] = "deitel.txt";
    char saveFileName[MAX] = "deitel.txt";
    while (1){
        printstat(stat, command, "NORMAL");
        
        if (mode == 0) {    
            while (1){
                cbreak();
                char dir;
                noecho();
                dir = wgetch(command);
                // scanw("%c", &dir);
                // wprintw(command, "%c", dir); wrefresh(command);
                if (dir == 'e'){
                    if (!((whichLine == 1 && cursorY == 5) ||( whichLine == 2 && cursorY == 5))){
                        if (cursorY > 4){ 
                            cursorY--;
                            cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                        }
                        int happened = 0;
                        if (cursorY == 4){
                            if (whichLine > 1) happened = 1;
                            if (whichLine > 1) whichLine--;
                            printFile(filewin, "deitel.txt", whichLine);
                            cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            

                        }
                        if (cursorY <= 4){
                            if (!happened  && cursorY > 1) {
                                cursorY--; 
                                cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                                
                            }
                        }
                        if (whichLine < 1) whichLine = 1;
                        
                        wmove(filewin, cursorY, cursorX);
                    }
                    else{
                        if (cursorY > 1) {
                            cursorY--;
                            cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                        }
                        wmove(filewin, cursorY, cursorX);
                    }
                }
                if (dir == 'd'){
                    if (!(cursorY >= 15 && whichLine >= fileSize("deitel.txt") - 17) || fileSize("deitel.txt") < 18) {
                            int happened = 0;
                            if (cursorY + whichLine == fileSize("deitel.txt") - 3) happened = 1;
                            if (cursorY < 16 && cursorY + whichLine < fileSize("deitel.txt") || (fileSize("deitel.txt") < 18 && cursorY + whichLine - 1 < fileSize("deitel.txt"))) cursorY++;
                            if (cursorY >= 16) {
                                if (fileSize("deitel.txt") >= 18) whichLine++;
                                printFile(filewin, "deitel.txt", whichLine);
                                if (whichLine < fileSize("deitel.txt") - 16 && fileSize("deitel.txt") > 17) {
                                    cursorY--;
                                    happened = 1;}
                            }
                            if (whichLine > mmax(1, fileSize("deitel.txt") - 17)) whichLine =mmax(1,  fileSize("deitel.txt") - 17);
                            cursorX = min(cursorX - numSize(whichLine + cursorY - 2) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            // if (cursorY >= 16 && cursorY < 18 && whichLine == fileSize("deitel.txt") - 17 && !happened) {cursorY++;}
                            wmove(filewin, cursorY, cursorX);
                        }
                        else{
                            if ((cursorY < 18 && (cursorY + whichLine) <= fileSize("deitel.txt"))) cursorY++;
                            cursorX = min(cursorX - numSize(whichLine + cursorY - 2) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            wmove(filewin, cursorY, cursorX);
                        }
                }
                if (dir == 's'){
                    if (cursorX > numSize(whichLine + cursorY - 1) ) cursorX--;
                    wmove(filewin, cursorY, cursorX);
                }
                if (dir == 'f'){
                    if (cursorX < RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1)) cursorX++;
                    wmove(filewin, cursorY, cursorX);
                }
                wrefresh(filewin);
                
                if (dir == 'i'){
                    mode = 2;
                    break;
                }

                if (dir == 'v') {
                    printstat(stat, command, "VISUAL"); mode = 1;
                    break;
                }
                if (dir == 'p'){
                    int current = 0;
                    int currentPos = cursorX - 3;
                    for (int i = 0; i < whichLine + cursorY - 2; i++){
                        current += lines[i];
                    }
                    current += currentPos; 
                    ppaste("deitel.txt", whichLine + cursorY - 1, cursorX - 3);
                    printFile(filewin, "deitel.txt", whichLine);
                    lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                    if (orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                    if (!str_cmp("deitel.txt", saveFileName)) {wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);}
                }
                if (dir == 'u'){
                    undo("deitel.txt");
                    lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                    printFile(filewin, "deitel.txt", whichLine);
                }
                else if(dir == '='){
                    close_pair("deitel.txt");
                    lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                    printFile(filewin, "deitel.txt", whichLine);
                }

                if (dir == ':'){
                    wrefresh(filewin); wclear(command); box(command, 0, 0); echo(); wmove(command, 1, 1); wprintw(command, ":"); wrefresh(command);
                    char a = wgetch(command);
                    char input[MAX] = "";
                    char file_address[MAX] = "";
                    inp(a, input, command);
                    if (str_cmp(input, "open")){
                        a = wgetch(command);
                        if (a == '"'){
                            inp_q(file_address, command);
                            wgetch(command);
                        }
                        else{
                            inp(a, file_address, command);
                        }
                        if (check_exist(file_address) != 0){
                            if (str_cmp(saveFileName, "deitel.txt") == 0){
                                inDeitel("deitel.txt", saveFileName);
                            }
                            inDeitel(file_address, "deitel.txt");
                            printFile(filewin, "deitel.txt", 1);
                            lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                            StrToStr(file_address, orgfile);
                            StrToStr(file_address, saveFileName);
                            wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", saveFileName); wrefresh(fileNameWin);
                            cursorX = 3; cursorY = 1;
                        }
                        else{
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "There is no file with this name. Press any key to continue..."); wrefresh(command); wgetch(command);
                        }
                    }
                    else if (str_cmp(input, "saveas")){
                        a = wgetch(command);
                        if (a == '"'){
                            inp_q(file_address, command);
                            wgetch(command);
                        }
                        else{
                            inp(a, file_address, command);
                        }
                        StrToStr(file_address, orgfile);
                        StrToStr(file_address, saveFileName);
                        wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", saveFileName); wrefresh(fileNameWin);
                        inDeitel("deitel.txt", file_address);
                    }

                    else if (str_cmp(input, "save")){
                        if (str_cmp(orgfile, "deitel.txt") == 0) {
                            inDeitel("deitel.txt", saveFileName);
                            if (!str_cmp(saveFileName, "deitel.txt")) wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", saveFileName); wrefresh(fileNameWin);
                        }
                        else{
                            mvwprintw(command,1, 1, "Your file doesn't have a name. Use saveas. Please enter a button to continue."); wrefresh(command);
                            wgetch(command);
                            wclear(command); box(command, 0, 0); wrefresh(command);
                        }
                    }

                    else if (str_cmp(input, "replace")){
                        char extra[MAX];
                        inp("t", extra, command);
                        char str1[MAX];
                        char str2[MAX];
                        char letter = wgetch(command);
                        if (letter == '"'){
                            inp_q(str1, command);
                            wgetch(command);
                        }
                        else{
                            inp(letter, str1, command);
                        }
                        inp("t", extra, command);
                        letter = wgetch(command);
                        char end;
                        if (letter == '"'){
                            inp_q(str2, command);
                            end = wgetch(command);
                        }
                        else{
                            end = inp(letter, str2, command);
                        }
                        int chand = 1;
                        char file_name[MAX] = "deitel.txt";
                        char option[50];
                        if (end !='\n'){
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(option, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, option, command);
                            }
                            if (str_cmp(option, "--file")){
                                letter = wgetch(command);
                                if (letter == '"'){
                                    inp_q(file_name, command);
                                    end = wgetch(command);
                                }
                                else{
                                    end = inp(letter, file_name, command);
                                }
                                if (end != '\n'){ 
                                    letter = wgetch(command);
                                    if (letter == '"'){
                                        inp_q(option, command);
                                        end = wgetch(command);
                                    }
                                    else{
                                        end = inp(letter, option, command);
                                    }   
                                    if (str_cmp(option, "-at")){
                                        chand = getNum(command);
                                    }
                                    else{
                                        chand = -1;
                                    }
                                }
                            }
                            else{ 
                                if (str_cmp(option, "-at")){
                                    chand = getNum(command);
                                }
                                else{
                                    chand = -1;
                                }
                            }
                        
                        }
                        //hameye voroodi haro gereftim...
                        int stars[MAX];
                        char target[MAX][100];
                        char target2[MAX][100];
                        int target_size = split_space(str1, target);
                        for (int i = 0; i < target_size; i++){
                            stars[i] = star(target[i]);
                        }
                        char str1Final[MAX];
                        make_message_findd(str1, str1Final);
                        target_size = split_space(str1Final, target2);
                        int results[MAX] = {0};
                        int tedad = findd(target2, target_size, "deitel.txt", stars, -1, results);
                        if (str_cmp(file_name, "deitel.txt")){
                            if (tedad > 0){
                                replace("deitel.txt", target2, target_size, str2, chand, stars);
                                int ln;
                                int pos;
                                if (chand > 0) {ln = posToLine(results[chand - 1], lines);}
                                else ln = posToLine(results[0], lines);
                                whichLine = ln;
                                cursorY = 1;
                                if (chand > 0) pos = posToPos(results[chand - 1], lines, whichLine + cursorY - 1);
                                else pos = posToPos(results[0], lines, whichLine + cursorY - 1);
                                cursorX = numSize(whichLine) + pos;
                                printFile(filewin, "deitel.txt", whichLine);
                                if (orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                                if (!str_cmp(saveFileName, "deitel.txt")) {wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);}
                            }
                            else{
                                wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "There was no match. Press any key to continue...");wrefresh(command); wgetch(command);
                                wclear(command); box(command, 0, 0); wrefresh(command);
                            }
                        }
                        else{
                            replace(file_name, target2, target_size, str2, chand, stars);
                        }
                    }
                    else if (str_cmp(input, "tree")){
                        int depth;
                        depth = getNum(command);
                        if (depth >= 0) {
                            inDeitel("deitel.txt", saveFileName);
                            wclear(fileNameWin); box(fileNameWin, 0, 0); wrefresh(fileNameWin);
                            wclear(filewin); box(filewin, 0, 0); wmove(filewin, 1, 0); wrefresh(filewin);
                            FILE *file = fopen("ans.txt", "w"); fclose(file);
                            StrToStr("deitel.txt", orgfile);
                            StrToStr("deitel.txt", saveFileName);
                            dirtree("root/", depth, depth);
                            inDeitel("ans.txt", "deitel.txt");
                            printFile(filewin, "deitel.txt", 1);
                            lineSize("deitel.txt", lines); RLineSize("deitel.txt", RLines);
                            cursorY = 1;
                            cursorX = 3;
                            
                        }
                    }
                    
                    else if (str_cmp(input, "removestr")){
                        char file_address[MAX] = "deitel.txt";//getting address of file
                        char letter;
                        letter = wgetch(command);
                        char extra[MAX];
                        inp(letter, extra, command);
                        // scanf("--file ");
                        int thisFile = 1;
                        int ln = 0; int pos = 0;
                        if (str_cmp(extra, "--file")){
                            thisFile = 0;
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(file_address, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, file_address, command);
                            }
                            inp("t", extra, command);
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        else{
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        int check = check_exist(file_address);//checking if we have a file with this name or not
                        if (!check){
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"There was no file with this name. Press any key to continue...");wrefresh(command);
                            wgetch(command);
                        }
                        else{
                            inp("t", extra, command);
                            int size;
                            size = getNum(command);
                            wgetch(command);
                            char dir = wgetch(command);
                            if (!thisFile && !strcmp("deitel.txt", saveFileName)) rmv(file_address, ln, pos, size, dir);
                            else{
                                // if (!thisFile && str_cmp(file_address, saveFileName)) rmv(file_address, ln, pos, size, dir);
                                rmv("deitel.txt", ln, pos, size, dir);
                                printFile(filewin, "deitel.txt", whichLine);
                                lineSize("deitel.txt", lines); RLineSize("deitel.txt", RLines);
                                if (!str_cmp("deitel.txt", saveFileName) && orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                                if (!str_cmp("deitel.txt", saveFileName)){
                                    wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);
                                }
                            }

                        }
                    }

                    else if (str_cmp(input, "copystr")){
                        char file_address[MAX] = "deitel.txt";//getting address of file
                        char letter;
                        letter = wgetch(command);
                        char extra[MAX];
                        inp(letter, extra, command);
                        // scanf("--file ");
                        int thisFile = 1;
                        int ln = 0; int pos = 0;
                        if (str_cmp(extra, "--file")){
                            thisFile = 0;
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(file_address, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, file_address, command);
                            }
                            inp("t", extra, command);
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        else{
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        int check = check_exist(file_address);//checking if we have a file with this name or not
                        if (!check){
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"There was no file with this name. Press any key to continue...");wrefresh(command);
                            wgetch(command);
                        }
                        else{
                            inp("t", extra, command);
                            int size;
                            size = getNum(command);
                            wgetch(command);
                            char dir = wgetch(command);
                            cpy(file_address, ln, pos, size, dir);
                        }
                    }

                    else if (str_cmp(input, "cutstr")){
                        char file_address[MAX] = "deitel.txt";//getting address of file
                        char letter;
                        letter = wgetch(command);
                        char extra[MAX];
                        inp(letter, extra, command);
                        // scanf("--file ");
                        int thisFile = 1;
                        int ln = 0; int pos = 0;
                        if (str_cmp(extra, "--file")){
                            thisFile = 0;
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(file_address, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, file_address, command);
                            }
                            inp("t", extra, command);
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        else{
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        int check = check_exist(file_address);//checking if we have a file with this name or not
                        if (!check){
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"There was no file with this name. Press any key to continue...");wrefresh(command);
                            wgetch(command);
                        }
                        else{
                            inp("t", extra, command);
                            int size;
                            size = getNum(command);
                            wgetch(command);
                            char dir = wgetch(command);
                            cpy(file_address, ln, pos, size, dir);
                            if (!thisFile && !strcmp("deitel.txt", saveFileName)) rmv(file_address, ln, pos, size, dir);
                            else{
                                // if (!thisFile && str_cmp(file_address, saveFileName)) rmv(file_address, ln, pos, size, dir);
                                rmv("deitel.txt", ln, pos, size, dir);
                                printFile(filewin, "deitel.txt", whichLine);
                                lineSize("deitel.txt", lines); RLineSize("deitel.txt", RLines);
                                if (!str_cmp("deitel.txt", saveFileName) && orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                                if (!str_cmp("deitel.txt", saveFileName)){
                                    wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);
                                }
                            }

                        }
                    }
                    
                    else if (str_cmp(input, "insertstr")){
                        char file_address[MAX] = "deitel.txt";//getting address of file
                        char letter;
                        letter = wgetch(command);
                        char extra[MAX];
                        char strt[MAX];
                        char str[MAX];
                        inp(letter, extra, command);
                        // scanf("--file ");
                        int thisFile = 1;
                        int ln = 0; int pos = 0;
                        if (str_cmp(extra, "--file")){
                            thisFile = 0;
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(file_address, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, file_address, command);
                            }
                            inp("t", extra, command);
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(strt, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, strt, command);
                            }
                            inp("t", extra, command);
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        else{
                            letter = wgetch(command);
                            if (letter == '"'){
                                inp_q(strt, command);
                                wgetch(command);
                            }
                            else{
                                inp(letter, strt, command);
                            }
                            inp("t", extra, command);
                            ln = getNum(command);
                            pos = getNum(command);
                        }
                        make_message(strt, str);
                        int check = check_exist(file_address);//checking if we have a file with this name or not
                        if (!check){
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"There was no file with this name. Press any key to continue...");wrefresh(command);
                            wgetch(command);
                        }
                        else{
                            if (!thisFile && !strcmp("deitel.txt", saveFileName)) {
                                insert(file_address, str, ln, pos);
                            }
                            else{
                                insert("deitel.txt", str, ln, pos);
                                printFile(filewin, "deitel.txt", whichLine);
                                lineSize("deitel.txt", lines); RLineSize("deitel.txt", RLines);
                                if (!str_cmp("deitel.txt", saveFileName) && orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                                if (!str_cmp("deitel.txt", saveFileName)){
                                    wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);
                                }
                            }

                        }
                    }
                    
                    else if (str_cmp(input, "createfile")){
                        char extra[MAX]; inp("t", extra, command);
                        char file_address[MAX];
                        char letter = wgetch(command);
                        if (letter == '"'){
                            inp_q(file_address, command);
                            wgetch(command);
                        }
                        else{
                            inp(letter, file_address, command);
                        }
                        char addresses[30][100];
                        int t = dir_addresses(file_address, addresses);
                        for (int i = 0; i < t - 1; i++){
                            make_dir(addresses[i]);
                        }
                        int success = create_file(file_address);
                        if (success) {
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"We made your file!. Press any key to continue...");
                            wgetch(command);
                            wclear(command); box(command, 0, 0);
                        }

                        else{
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1,"This file already exists. Press any key to continue...");
                            wgetch(command);
                            wclear(command); box(command, 0, 0);
                        }
                    }
                                
                    else{
                        wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "Invalid input. Press any key to continue..."); wrefresh(command); wgetch(command);
                        wclear(command); box(command, 0, 0); wrefresh(command);
                    }
                    
                    wclear(command); box(command, 0, 0); wrefresh(command);
                    noecho();

                }
                else if (dir == '/'){
                    echo();
                    wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "/"); wrefresh(command);
                    char input[MAX];
                    char letter = wgetch(command);
                    if (letter == '"'){
                        inp_q(input, command);
                        wgetch(command);
                    }
                    else{
                        inp(letter, input, command);
                    }
                    wclear(command); box(command, 0, 0); wrefresh(command);
                    int stars[MAX];
                    int results[MAX] = {0};
                    char words_temp[MAX][100];
                    int tedad = split_space(input, words_temp);
                    for (int i = 0; i < tedad; i++){
                        stars[i] = star(words_temp[i]);
                    }
                    char line[MAX];
                    make_message_findd(input, line);
                    char words[MAX][MAX];
                    tedad = split_space(line, words);
                    int answerAmount = findd(words, tedad, "deitel.txt", stars, -1, results);
                    noecho();
                    if (answerAmount > 0){
                        int chand = 1;
                        printFileFind(fileNameWin, filewin,  words, tedad, chand, stars, line, lines, whichLine + cursorY - 1);
                        wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "Press 'n' for next match or press anything else to teminate find..."); wrefresh(command);
                        char next = wgetch(command);
                        wclear(command); box(command, 0, 0); wrefresh(command);
                        while (next == 'n'){
                            // chand = (chand + 1) % answerAmount + 1;
                            chand++;
                            if (chand == answerAmount + 1) chand = 1;
                            cursorX = printFileFind(fileNameWin, filewin, words, tedad, chand, stars, line, lines, whichLine + cursorY - 1);
                            wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "Press 'n' for next match or press anything else to teminate find..."); wrefresh(command);
                            next = wgetch(command);
                            wclear(command); box(command, 0, 0); wrefresh(command);
                        }
                        printFile(filewin, "deitel.txt", posToLine(results[chand - 1], lines));
                        whichLine = posToLine(results[chand - 1], lines);
                        // lineSize("deitel.txt", lines); RLineSize("deitel.txt", RLines);
                        cursorY = 1;
                        wmove(filewin, cursorY, cursorX); wrefresh(filewin);
                    }
                    else {
                        wclear(command); box(command, 0, 0); mvwprintw(command, 1, 1, "There was no match. Press any key to continue..."); wrefresh(command); wgetch(command);
                        wclear(command); box(command, 0, 0); wrefresh(command);
                    }
                    

                }
                else if (dir == '`'){
                    endwin();
                    return 0;
                }
                wmove(filewin, cursorY, cursorX); wrefresh(filewin);
                // attroff(A_INVIS);
                if (dir == 126) break;
            }
        }
        //VISUAL    
        wrefresh(filewin);
        if (mode == 1){
            int current = 0;
            int currentPos = cursorX - numSize(whichLine + cursorY - 1);
            for (int i = 0; i < whichLine + cursorY - 2; i++){
                current += lines[i];
            }
            current += currentPos; 
                while (1){
                    if (mode == 1){
                        char dir;
                        dir = getch();



                        if (dir == 'e'){
                        if (!((whichLine == 1 && cursorY == 5) ||( whichLine == 2 && cursorY == 5))){
                            if (cursorY > 4){ 
                                cursorY--;
                                cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            }
                            int happened = 0;
                            if (cursorY == 4){
                                if (whichLine > 1) happened = 1;
                                if (whichLine > 1) whichLine--;
                                printFile(filewin, "deitel.txt", whichLine);
                                cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                                

                                }
                                if (cursorY <= 4){
                                    if (!happened  && cursorY > 1) {
                                        cursorY--; 
                                        cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                                        
                                    }
                                }
                                if (whichLine < 1) whichLine = 1;
                                
                                wmove(filewin, cursorY, cursorX);
                            }
                            else{
                                if (cursorY > 1) {
                                    cursorY--;
                                    cursorX = min(cursorX - numSize(whichLine + cursorY) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                                }
                                wmove(filewin, cursorY, cursorX);
                            }
                        }



                    if (dir == 'd'){
                        if (!(cursorY >= 15 && whichLine >= fileSize("deitel.txt") - 17) || fileSize("deitel.txt") < 18) {
                            int happened = 0;
                            if (cursorY + whichLine == fileSize("deitel.txt") - 3) happened = 1;
                            if (cursorY < 16 && cursorY + whichLine < fileSize("deitel.txt") || (fileSize("deitel.txt") < 18 && cursorY + whichLine - 1 < fileSize("deitel.txt"))) cursorY++;
                            if (cursorY >= 16) {
                                if (fileSize("deitel.txt") >= 18) whichLine++;
                                printFile(filewin, "deitel.txt", whichLine);
                                if (whichLine < fileSize("deitel.txt") - 16 && fileSize("deitel.txt") > 17) {
                                    cursorY--;
                                    happened = 1;}
                            }
                            if (whichLine > mmax(1, fileSize("deitel.txt") - 17)) whichLine =mmax(1,  fileSize("deitel.txt") - 17);
                            cursorX = min(cursorX - numSize(whichLine + cursorY - 2) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            // if (cursorY >= 16 && cursorY < 18 && whichLine == fileSize("deitel.txt") - 17 && !happened) {cursorY++;}
                            wmove(filewin, cursorY, cursorX);
                        }
                        else{
                            if ((cursorY < 18 && (cursorY + whichLine) <= fileSize("deitel.txt"))) cursorY++;
                            cursorX = min(cursorX - numSize(whichLine + cursorY - 2) + numSize(whichLine + cursorY - 1) , RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1));
                            wmove(filewin, cursorY, cursorX);
                        }
                    }
                    if (dir == 's'){
                        if (cursorX > numSize(whichLine + cursorY - 1) ) cursorX--;
                        wmove(filewin, cursorY, cursorX);
                    }
                    if (dir == 'f'){
                        if (cursorX < RLines[cursorY + whichLine - 2] + numSize(whichLine + cursorY - 1)) cursorX++;
                        wmove(filewin, cursorY, cursorX);
                    }
                    if (dir == 'h' || dir == 'k' || dir == 'c'){
                        int final = 0;
                        int currentPos = cursorX - numSize(whichLine + cursorY - 1);
                        for (int i = 0; i < whichLine + cursorY - 2; i++){
                            final += lines[i];
                        }
                        final += currentPos;
                        mode = 0;
                        // wclear(fileNameWin); mvwprintw(fileNameWin, 1, 1, "start: %d end: %d", current, final); wrefresh(fileNameWin); wgetch(fileNameWin);
                        // break;
                        int start = min(final, current);
                        int end = mmax(final, current);
                        if (dir == 'h'){
                            rmv("deitel.txt", posToLine(start, lines), posToPos(start, lines, whichLine + cursorY - 1), end - start, 'f');
                            printFile(filewin, "deitel.txt", whichLine);
                            lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                            if (orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                            if (!str_cmp(saveFileName, "deitel.txt")) {wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);}
                            break;
                        }
                        else if (dir == 'k'){
                            cut("deitel.txt", posToLine(start, lines), posToPos(start, lines, whichLine + cursorY - 1), end - start, 'f');
                            printFile(filewin, "deitel.txt", whichLine);
                            lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                            if (orgfile[strlen(orgfile) - 1] != '+') strncat(orgfile, " +", 2);
                            if (!str_cmp(saveFileName, "deitel.txt")) {wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);}
                            break;
                        }
                        else if(dir == 'c'){
                            cpy("deitel.txt", posToLine(start, lines), posToPos(start, lines, whichLine + cursorY - 1), end - start, 'f');
                            break;
                        }

                    }
                    wrefresh(filewin);
                }
            }
        }
        //INSERT
        if (mode == 2){
            while (1){
                echo();
                printstat(stat, command, "INSERT");
                char a = wgetch(filewin);
                if (a == '`'){
                    mode = 0;
                    noecho();
                    break;
                }
                if (orgfile[strlen(orgfile) - 1] != '+' && str_cmp(orgfile, "deitel.txt") == 0) strncat(orgfile, " +", 2);
                if (!str_cmp(saveFileName, "deitel.txt")) {wclear(fileNameWin); box(fileNameWin, 0, 0); mvwprintw(fileNameWin, 1, 1, "%s", orgfile); wrefresh(fileNameWin);}
                int current = 0;
                int currentPos = cursorX - numSize(whichLine + cursorY - 1);
                for (int i = 0; i < whichLine + cursorY - 2; i++){
                    current += lines[i];
                }
                current += currentPos;
                char b[2]; b[0] = a; b[1] = '\0';
                if (a != 27) insert("deitel.txt", b, whichLine + cursorY - 1, currentPos);
                printFile(filewin, "deitel.txt", whichLine);
                lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                if (a == 27){
                    // rmv("deitel.txt", whichLine + cursorY - 1, currentPos, 2, 'b');
                    rmv("deitel.txt", whichLine + cursorY - 1, currentPos, 1, 'b');
                    if (cursorX != numSize(whichLine + cursorY - 1)) cursorX--;
                    else{
                        cursorX = lines[whichLine + cursorY - 3] + 2;
                        cursorY--;
                    }
                    printFile(filewin, "deitel.txt", whichLine);
                    lineSize("deitel.txt", lines); RLineSize("deitel.txt",  RLines);
                }
                else if (a != '\n'){
                    cursorX++;
                }
                else{
                    cursorY++;
                    cursorX = numSize(whichLine + cursorY - 1);
                }
                wmove(filewin, cursorY, cursorX); wrefresh(filewin);

            }
        }
    }
    
    
    getch();
    endwin();
    return 0;
}