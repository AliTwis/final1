#include <stdio.h> 
#include <dirent.h>
#include <unistd.h>
#define MAX 1000

int str_len(char strr[]){
    char ans = strlen(strr);
    if (strr[ans - 1] == '\r') return ans - 1;
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

int save(char file_name[]){
    char save_file[MAX] = "saving/";
    for (int i = 0; i < str_len(file_name); i++){
        save_file[7 + i] = file_name[i];
    }
    save_file[7 + str_len(file_name)] = '\0';
    FILE *file = fopen(file_name, "r");
    remove(save_file);
    FILE *filesave = fopen(save_file, "w");
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (line[0] != '\0') fprintf(filesave, "%s", line);
    }
    fclose(file);
    fclose(filesave);
    return 1;
}

int undo(char file_name[]){
    FILE *file = fopen(file_name, "r+");
    char save_file[MAX] = "saving/";
    for (int i = 0; i < str_len(file_name); i++){
        save_file[7 + i] = file_name[i];
    }
    save_file[7 + str_len(file_name)] = '\0';
    FILE *filesave = fopen(save_file, "r");
    FILE *filetemp = fopen("saving/root/temp_file.txt", "w+");
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    fclose(file);
    file = fopen(file_name, "w");
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, filesave);
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

int create_file(char address[]){
    if (access(address, F_OK) == 0){
        return 0;
    }
    FILE *newfile = fopen(address, "w");
    fclose(newfile);
    return 1;
}

int make_message(char old_message[], char new_message[]){//making \n and making " from \"
    int head = 0;
    for (int i = 0; i < MAX; i++){
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

char inp(char start, char input[]){//input without "
    input[0] = start;
    int index = 1;
    char letter;
    scanf("%c", &letter);
    while (letter != ' ' && letter != '\n'){
        input[index++] = letter;
        scanf("%c", &letter);
    }
    input[index] = '\0';
    if (letter == ' '){
        return ' ';
    }
    else return '\n';
}

int inp_q(char input[]){//input with "
    char a1 = 'a';
    char a2 = 'a';
    char a3 = 'a';
    char index = 0;
    scanf("%c", &a1);
    while (!((a1 == '"' && a2 != '\\') || (a1 == '"' && a2 == '\\' && a3 == '\\'))){
        input[index++] = a1;
        a3 = a2;
        a2 = a1;
        scanf("%c", &a1);
    }
    if (a1 == '"' && a2 == '\\' && a3 == '\\'){
        input[index] = '\0';
        index--;
    }
    else input[index] = '\0';
    return index;
}

void insert(char file_name[], char message[], int ln, int pos){
    save(file_name);
    FILE *file = fopen(file_name, "r+");
    FILE *filetemp = fopen("temp_file.txt", "w");
    //declaring line for getting each line of the file. rest is for saving rest of the file from the given position.
    char line[MAX]; line[0] = '`';
    //jumping to the line
    for (int l = 0; l < ln - 1; l++){
        fgets(line, MAX, file);
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
        fgets(line, MAX, file);
        fprintf(filetemp, "%s", line);
    }
    //changing names
    fclose(file);
    fclose(filetemp);
    remove(file_name);
    rename("temp_file.txt", file_name);
}

void cat(char file_name[]){
    FILE *file = fopen(file_name, "r");
    FILE *fileans = fopen("ans.txt", "w");
    char line[MAX];
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        fprintf(fileans, "%s", line);
    }
    fclose(file);
    fclose(fileans);
}

void rmv(char file_name[], int ln, int pos, int size, char dir){
    save(file_name);
    FILE *file = fopen(file_name, "r+");
    //declaring line for getting each line of the file. rest is for saving rest of the file from the given position.
    char line[MAX]; line[0] = '`';
    //jumping to the line
    for (int l = 0; l < ln - 1; l++){
        fgets(line, MAX, file);
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
        fgets(line, MAX, file);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    fclose(filetemp);
    fclose(file);
    remove(file_name);
    rename("root/temp_file.txt", file_name);

}

int cpy(char file_name[], int ln, int pos, int size, char dir){
    FILE *file = fopen(file_name, "r");
    char line[MAX];//saving lines
    for (int i = 0; i < ln - 1; i++){
        fgets(line, MAX, file);
    }
    char letter;//skipping letters until pos
    for (int i = 0; i <= pos; i++){
        fscanf(file, "%c", &letter);
    }
    char ans[MAX];
    if (dir == 'b') fseek(file, -size - 1, SEEK_CUR);
    for (int i = 0; i < size; i++){
        fscanf(file, "%c", &ans[i]);
    }
    fclose(file);
    //copying
    FILE *filecpy = fopen("clipboard.txt", "w");
    fprintf(filecpy, "%s", ans);
    fclose(filecpy);
}

int cut(char file_name[], int ln, int pos, int size, char dir){
    cpy(file_name, ln, pos, size, dir);
    rmv(file_name, ln, pos, size, dir);

}

int ppaste(char file_name[], int ln, int pos){
    save(file_name);
    FILE *file = fopen(file_name, "r");
    FILE *filetemp = fopen("root/temp_file.txt", "w");
    char line[MAX]; line[0] = '`';//saving each line of the file until the target
    for (int i = 0; i < ln - 1; i++){
        fgets(line, MAX, file);
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
        fgets(line, MAX, filecpy);
        if (line[0] != '\0') fprintf(filetemp, "%s", line);
    }
    line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
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
        if (line[i] != ' ' && line[i] != '\0' && line[i] != '\n'){
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
    for (int i = 0; i < MAX; i++){
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
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        char words[MAX][MAX];
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
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        char words[MAX][100];
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
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
        if (which_line && line[0] != '\0') fprintf(filetemp, "\n");//going to next line in temp file
        char words[MAX][100];
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
    char line[MAX]; line[0] = '`';
    while(line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
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
    char line1[MAX]; line1[0] = '`';
    char line2[MAX]; line2[0] = '`';
    int big1 = 0, big2 = 0;
    int which_line = 1;//seeing which line we are
    while(line1[0] != '\0' || line2[0] != '\0'){
        line1[0] = '\0'; line2[0] = '\0';
        fgets(line1, MAX, file1);
        fgets(line2, MAX, file2);
        if(line1[0] == '\0' && line2[0] != '\0'){
            big2 = 1;
            break;
        }
        if (line2[0] == '\0' && line1[0] != '\0'){
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
        char line[MAX]; line[0] = '`';
        while(line[0] != '\0'){
            line[0] = '\0';
            fgets(line, MAX, file1);
            if (line[0] != '\0') last_line++;
        }
        fseek(file1, 0, SEEK_SET);
        for (int i = 0; i < which_line - 1; i++){
            fgets(line, MAX, file1);
        }
        fprintf(fileans, "====<file1 is bigger:%d - %d>====\n", which_line, last_line);
        line[0] = '`';
        while (line[0] != '\0'){
            line[0] = '\0';
            fgets(line, MAX, file1);
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
        char line[MAX]; line[0] = '`';
        while(line[0] != '\0'){
            line[0] = '\0';
            fgets(line, MAX, file2);
            if (line[0] != '\0') last_line++;
        }
        fseek(file2, 0, SEEK_SET);
        for (int i = 0; i < which_line - 1; i++){
            fgets(line, MAX, file2);
        }
        fprintf(fileans, "====<file2 is bigger:%d - %d>====\n", which_line, last_line);
        line[0] = '`';
        while (line[0] != '\0'){
            line[0] = '\0';
            fgets(line, MAX, file2);
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
    int dir_size = str_len(dir_cur);
    while ((entity = readdir(directory)) != NULL){ 
        char *ent_name = (entity->d_name);
        if (((entity->d_name)[0] == '.' && str_len(ent_name) == 1) || (ent_name[0] == '.' && ent_name[1] == '.' && str_len(ent_name) == 2)) continue;
        int ent_size = str_len(ent_name);
        char next_dir[MAX];
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
    char line[MAX]; line[0] = '`';
    int is_p = 0;//seeing if we have found a {
    int br = 0;
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, file);
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
    char line[MAX]; line[0] = '`';
    while (line[0] != '\0'){
        line[0] = '\0';
        fgets(line, MAX, fileans);
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

int main(){
    int arman = 0;
    while (1){
        char order[MAX];
        scanf("%s", order);
        if (arman == 0) {FILE *file = fopen("ans.txt", "w"); fclose(file);}
        if (str_cmp(order, "insertstr")){
            char file_address[MAX];//getting address of file
            scanf(" --file ");
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            char temp_matn[MAX];
            if (arman == 0) {    
                scanf("--str ");
                scanf("%c", &letter);
                if (letter == '"'){
                    inp_q(temp_matn);
                    getchar();
                }
                else{
                    inp(letter, temp_matn);
                }
            }
            else{
                armann(temp_matn);
                arman = 0;
            }
            char matn[MAX];
            make_message(temp_matn, matn);
            int ln = 0; int pos = 0;
            scanf("--pos %d:%d", &ln, &pos);//getting line and pos
            insert(file_address, matn, ln, pos);
        }
        else if (str_cmp(order, "cat")){
            scanf(" --file ");
            char file_address[MAX];
            char letter;
            scanf("%c", &letter);
            char end;//checking for arman
            if (letter == '"'){
                inp_q(file_address);
                scanf("%c", &end);
            }
            else{
                end = inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
            cat(file_address);
            if (end == '\n'){
                print_ans();
                arman = 0;
            }
            else{
                scanf(" =D ");
                arman = 1;
            }
        }
        else if (str_cmp(order, "removestr")){
            char file_address[MAX];//getting address of file
            scanf(" --file ");
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
            int ln = 0; int pos = 0;
            scanf("--pos %d:%d", &ln, &pos);//getting line and pos
            scanf(" -size ");
            int size;
            scanf("%d", &size);
            scanf(" -");
            char dir;
            scanf("%c", &dir);
            rmv(file_address, ln, pos, size, dir);
        }
        else if(str_cmp(order, "copystr") || str_cmp(order, "cutstr")){
            char file_address[MAX];//getting address of file
            scanf(" --file ");
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            int ln = 0; int pos = 0;
            scanf("--pos %d:%d", &ln, &pos);//getting line and pos
            scanf(" -size ");
            int size;
            scanf("%d", &size);
            scanf(" -");
            char dir;
            scanf("%c", &dir);
            if (str_cmp(order, "copystr")){
                cpy(file_address, ln, pos, size, dir);
            }
            if (str_cmp(order, "cutstr")){
                cut(file_address, ln, pos, size, dir);
            }
        }
        else if(str_cmp(order, "pastestr")){
            char file_address[MAX];//getting address of file
            scanf(" --file ");
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            int ln = 0; int pos = 0;
            scanf("--pos %d:%d", &ln, &pos);//getting line and pos
            ppaste(file_address, ln, pos);
        }
        else if(str_cmp(order, "grep")){
            scanf(" -");
            char option_c;
            int option = 0;//what we give to our function for attributes we have
            scanf(" %c", &option_c);
            if (option_c == 'c'){
                option = 1;
                scanf(" --");
            }
            else if(option_c == 'l'){
                option = 2;
                scanf(" --");
            }
            scanf("str ");
            char letter;
            scanf("%c", &letter);
            char target_temp[MAX];
            if (letter == '"'){
                inp_q(target_temp);
                getchar();
            }
            else {
                inp(letter, target_temp);
            }
            char target[MAX];
            make_message(target_temp, target);//making what we are looking for
            scanf("--files ");
            int tedad = 0;//if option is 2, we need to save how many times we saw the target 
            //inputting name of the files
            char next = '`';
            while (next != '\n'){
                scanf("%c", &letter);
                char file_address[MAX];
                if (letter == '=') break;
                if (letter == '"'){
                    inp_q(file_address);
                    scanf("%c", &next);
                }
                else{
                    next = inp(letter, file_address);
                }
                int check = check_exist(file_address);//checking if we have a file with this name or not
                if (!check){
                    printf("We don't have a file with this address.\n");
                    continue;
                }
                 //saving this state of file
                //base on option, we will see what we should do
                if (option == 0){
                    grep(file_address, target, str_len(target), 0);
                }
                else if(option == 1){
                    tedad += grep(file_address, target, str_len(target), 1);
                }
                else{
                    tedad = grep(file_address, target, str_len(target), 2);
                    if (tedad == 1){
                        FILE *fileans = fopen("ans.txt", "a");
                        fprintf(fileans, "%s\n", file_address);
                        fclose(fileans);
                    }
                }
            }
            if (option == 1){
                FILE *fileans = fopen("ans.txt", "w");
                fprintf(fileans, "%d", tedad);
                fclose(fileans);
            }
            if (next == '\n'){

                print_ans();
                arman = 0;
            }
            else{
                scanf("D ");
                arman = 1;
            }
            
        }
        else if(str_cmp(order, "undo")){
            scanf(" --file ");
            char file_address[MAX];
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            undo(file_address);
        }
        else if(str_cmp(order, "compare")){
            getchar();
            char file_f[MAX];
            char file_s[MAX];
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_f);
                getchar();
            }
            else{
                inp(letter, file_f);
            }
            int check = check_exist(file_f);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
            save(file_f);//saving this state of file
            scanf("%c", &letter);
            char end;//checking for arman
            if (letter == '"'){
                inp_q(file_s);
                scanf("%c", &end);
            }
            else{
                end = inp(letter, file_s);
            }
            check = check_exist(file_s);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
            save(file_s);//saving this state of file
            text_cmp(file_f, file_s);
            if (end == '\n'){
                print_ans();
                arman = 0;
            }
            else{
                scanf("D ");
                arman = 1;
            }
        }
        else if(str_cmp(order, "tree")){
            int depth;
            scanf("%d", &depth);
            dirtree("root/", depth, depth);
            char end;//checking for arman
            scanf("%c", &end);
            if (end == '\n'){
                print_ans();
                arman = 0;
            }
            else{
                scanf("D ");
                arman = 1;
            }
        }
        else if(str_cmp(order, "createfile")){
            scanf(" --file ");
            char file_address[MAX];
            char letter;
            scanf("%c", &letter);
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            char addresses[MAX][100];
            int t = dir_addresses(file_address, addresses);
            for (int i = 0; i < t - 1; i++){
                make_dir(addresses[i]);
            }
            int success = create_file(file_address);
            if (success) printf("We made your file!\n");
            else(printf("This file already exists.\n"));
        }
        else if(str_cmp(order, "find")){
            char target[MAX];
                char letter;
            if (arman == 0) {    
                scanf(" --str ");
                scanf("%c", &letter);
                if (letter == '"'){
                    inp_q(target);
                    getchar();
                }
                else{
                    inp(letter, target);
                }
            }
            else{
                armann(target);
                arman = 0;
                getchar();
            }
            char words_temp[MAX][100];//saving words for checking their * condition
            int words_amount = split_space(target, words_temp);
            int stars[MAX] = {0};
            for (int i = 0; i < words_amount; i++){
                stars[i] = star(words_temp[i]);
            }
            char target_final[MAX];
            make_message_findd(target, target_final);
            char words[MAX][100];
            words_amount = split_space(target_final, words);
            scanf("--file ");
            scanf("%c", &letter);
            char file_address[MAX];
            int is_space = 0;
            if (letter == '"'){
                inp_q(file_address);
                scanf("%c", &letter);
            }
            else{
                letter = inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            FILE *fileans = fopen("ans.txt", "w");//saving the output in a file
            if (letter == '\n'){
                int results[MAX];
                int ans = findd(words, words_amount, file_address, stars, 1, results);
                fprintf(fileans, "%d", ans);
                fclose(fileans);
                print_ans();
                arman = 0;
                continue;
            }
            else {
                char end;
                scanf("%c", &end);
                if (end == '-') {
                    int results[MAX];
                    char option[40];
                    scanf("%s", option);
                    if (str_cmp(option, "count")){
                        int ans = findd(words, words_amount, file_address, stars, -2, results);
                        fprintf(fileans, "%d", ans);
                    }
                    else{
                        int at = 1;
                        if (str_cmp(option, "at")) scanf(" %d", &at);
                        scanf("%c", &letter);
                        if (letter == ' '){
                            scanf("-");
                            char option2[40];
                            scanf("%s", option2);
                            scanf("%c", &letter);//checkig for arman
                            if ((str_cmp(option, "all")) || (str_cmp(option2, "all"))){
                                int tedad = findd_word(words, words_amount, file_address, stars, -1, results);
                                for (int j = 0; j < tedad; j++){
                                    fprintf(fileans, "%d", results[j]);
                                    if (j < tedad - 1) fprintf(fileans, "\n", results[j]);
                                }
                            }
                            if ((str_cmp(option, "at")) || (str_cmp(option2, "at"))){
                                if (str_cmp(option2, "at")) scanf(" %d", &at);
                                int ans = findd_word(words, words_amount, file_address, stars, at, results);
                                fprintf(fileans, "%d", ans);
                            }
                        }
                        else{
                            if (str_cmp(option, "all")){
                                int tedad = findd(words, words_amount, file_address, stars, -1, results);
                                for (int j = 0; j < tedad; j++){
                                    fprintf(fileans, "%d", results[j]);
                                    if (j < tedad - 1) fprintf(fileans, "\n", results[j]);
                                }
                            }
                            else{
                                int ans = findd(words, words_amount, file_address, stars, at, results);
                                fprintf(fileans, "%d", ans);
                            }
                        }
                    }
                    if (letter == '\n'){
                        fclose(fileans);
                        print_ans();
                        arman = 0;
                        continue;
                    }
                    else{
                        scanf("D ");
                        arman = 1;
                    }
                }
                else{
                    scanf("D ");
                    arman = 1;
                }
            }
            fclose(fileans);
            
        }
        else if(str_cmp(order, "auto-indent")){
            scanf(" ");
            char letter;
            scanf("%c", &letter);
            char file_address[MAX];
            if (letter == '"'){
                inp_q(file_address);
                getchar();
            }
            else{
                inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            close_pair(file_address);
        }
        else if(str_cmp(order, "replace")){
            scanf(" --str1 ");
            char str1[MAX];
            char letter; scanf("%c", &letter);
            if (letter == '"'){
                inp_q(str1);
                getchar();
            }
            else{
                inp(letter, str1);
            }
            
            char str2[MAX];
            if (arman == 0){    
                scanf("--str2 ");
                scanf("%c", &letter);
                if (letter == '"'){
                    inp_q(str2);
                    getchar();
                }
                else{
                    inp(letter, str2);
                }
            }
            else{
                arman = 0;
                armann(str2);
            }
            char words_temp[MAX][100];//saving words for checking their * condition
            int words_amount = split_space(str1, words_temp);
            int stars[MAX] = {0};
            for (int i = 0; i < words_amount; i++){
                stars[i] = star(words_temp[i]);
            }

            char target_final[MAX];//eliminating extra * and /
            make_message_findd(str1, target_final);
            char words[MAX][100];
            words_amount = split_space(target_final, words);

            char new[MAX];
            make_message(str2, new);    
            scanf("--file ");
            char file_address[MAX];
            scanf("%c", &letter);
            char end;//checking if user want to use an option
            if (letter == '"'){
                inp_q(file_address);
                scanf("%c", &end);
            }
            else{
                end = inp(letter, file_address);
            }
            int check = check_exist(file_address);//checking if we have a file with this name or not
            if (!check){
                printf("We don't have a file with this address.\n");
                continue;
            }
             //saving this state of file
            if (end == '\n'){
                int success = replace(file_address, words, words_amount, new, 1, stars);
                if (!success){
                    printf("We couldn't find any match.\n");
                }
            }
            else{
                char option[30];
                scanf("-%s", option);
                if (str_cmp(option, "at")){
                    int chand = 0;
                    scanf("%d", &chand);
                    int success = replace(file_address, words, words_amount, new, chand, stars);
                    if (!success){
                        printf("We couldn't find any match.\n");
                    }
                }
                else{
                    int success = replace(file_address, words, words_amount, new, -1, stars);
                    if (!success){
                        printf("We couldn't find any match.\n");
                    }
                }
            }
        }
        else{
            printf("Invalid input.\n");
            char line[MAX]; gets(line);
        }
    }
}
