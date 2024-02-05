#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000


char* move_str(char *str, int move_number){
    int len= strlen(str);
    for(int i=0; i<len-move_number;i++){
        str[i]=str[i+move_number];
    }
    str[len-move_number]='\0';
    return str;
}

void make_local_username(char *username){
   /* if (access("/mnt/d/pwd", F_OK) != -1){
        printf("1111111111111111111\n");
        //find place of neogit
        FILE *file = fopen("/mnt/d/pwd","r");
        char pwd[1000];
        fgets(pwd, sizeof(pwd), file);
        fclose(file);

        //going to neogit place
        char replace[100];
        strcpy(replace,"username: ");
        strcat(replace,username);

        file = fopen(".neogit/config", "r+");

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strncmp(line, "username", 8) == 0){
                printf("2222222222222222\n");
                int length = strlen(line);
                // remove '\n'
                if (length > 0 && line[length - 1] == '\n') {
                    line[length - 1] = '\0';
                }
                // تغییر عبارت
                strncpy(line, replace, strlen(replace));
                printf("%s\n",line);
            }
        }
        fclose(file);
    }*/
    FILE *file = fopen("username", "w");
    fprintf(file, "%s", username);
    fclose(file);
}

void make_local_email(char *email){
    FILE *file = fopen("email", "w");
    fprintf(file, "%s", email);
    fclose(file);
}

void make_global_username(char *username){
    char cwd[1024];
    do{
        if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
        if (strcmp(cwd, "/mnt/d") != 0) {
            if (chdir("..") != 0) return 1;
        }
    }while (strcmp(cwd, "/mnt/d") != 0);

    FILE *file = fopen("username", "w");
    fprintf(file, "%s", username);
    fclose(file);
}

void make_global_email(char *email){
    char cwd[1024];
    do{
        if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
        if (strcmp(cwd, "/mnt/d") != 0) {
            if (chdir("..") != 0) return 1;
        }
    }while (strcmp(cwd, "/mnt/d") != 0);
    FILE *file = fopen("email", "w");
    fprintf(file, "%s", email);
    fclose(file);
}

int make_global_pwd_of_neogit(char *pwd){
    char cwd[1024];
    do{
        if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
        if (strcmp(cwd, "/mnt/d") != 0) {
            if (chdir("..") != 0) return 1;
        }
    }while (strcmp(cwd, "/mnt/d") != 0);
    FILE *file = fopen("pwd", "w");
    fprintf(file, "%s", pwd);
    fclose(file);

    return create_configs(pwd);
}

int run_init(int argc, char * argv[],char * username, char *email) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
                exists = true;
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0) {
            if (chdir("..") != 0) return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);

    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
        
    if (!exists) {
        if (mkdir(".neogit", 0755) != 0) return 1;
        return make_global_pwd_of_neogit(cwd);
        //return create_configs(cwd);
    } else {
        perror("neogit repository has already initialized");
    }
    return 0;
}

int create_configs(char *cwd) {


    char file_path[1024];
    strcpy(file_path,cwd);
    strcat(file_path,"/username");

    char username[40];
    char email[50];

    if (access(file_path, F_OK) != -1){
        FILE *file = fopen(file_path, "r");
        fgets(username, sizeof(username), file);
        printf("%s\n",username);
        fclose(file);
    } 
    else if (access("/mnt/d/username", F_OK) != -1){
        FILE *file = fopen("/mnt/d/username", "r");
        fgets(username, sizeof(username), file);
        fclose(file);
    } 

    char file_path2[1024];
    strcpy(file_path,cwd);
    strcat(file_path,"/email");
    if (access(file_path2, F_OK) != -1){
        FILE *file = fopen(file_path, "r");
        fgets(email, sizeof(email), file);
        fclose(file);
    } 
    else if (access("/mnt/d/email", F_OK) != -1){
        FILE *file = fopen("/mnt/d/email", "r");
        fgets(email, sizeof(email), file);
        fclose(file);
    } 
    //going to neogit place
    if (chdir(cwd) != 0) return 1;

    
    FILE *file = fopen(".neogit/config", "w");
    if (file == NULL) return 1;

    fprintf(file, "username: %s\n", username);
    fprintf(file, "email: %s\n", email);
    fprintf(file, "branch: %s\n", "master");
    fprintf(file, "last_commit_ID: %d", 0);

    fclose(file);
    
    // create commits folder
    if (mkdir(".neogit/commits", 0755) != 0) return 1;

    // create stages file folder
    if (mkdir(".neogit/stages_files", 0755) != 0) return 1;

    file = fopen(".neogit/staging", "w");
    fclose(file);

    file = fopen(".neogit/tracks", "w");
    fclose(file);

    file=fopen(".neogit/branchs","w");
    fprintf(file, "%s\n","master");
    fclose(file);

    return 0;
}

int run_add(int argc, char * const argv) {
    return find_path_and_stage(argv);
}

char *find_path(char *filepath){
    //if we have .. in adress
    int back_count=0;
    while(filepath[0]=='.' && filepath[1]=='.'){
        move_str(filepath,3);
        back_count++;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    for(int i=strlen(cwd); 1; i--){
        if(cwd[i]=='/'){  cwd[i]='\0';  
            back_count--;}
        if(back_count<=0)  break;
    } 

    
    char *file_path=(char *)malloc(1000 * sizeof(char));
    strcpy(file_path,cwd);
    strcat(file_path,"/");
    strcat(file_path,filepath);
    return file_path;
}

int find_path_and_stage(char *filepath){
    //if we have .. in adress
    int back_count=0;
    while(filepath[0]=='.' && filepath[1]=='.'){
        move_str(filepath,3);
        back_count++;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    for(int i=strlen(cwd); 1; i--){
        if(cwd[i]=='/'){  cwd[i]='\0';  
            back_count--;}
        if(back_count<=0)  break;
    } 

    
    char file_path[1024];
    strcpy(file_path,cwd);
    strcat(file_path,"/");
    strcat(file_path,filepath);
    return add_to_staging (file_path);
}

int add_to_staging(char *file_path) {

    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    if (access(file_path, F_OK) != -1){

        FILE *file = fopen(".neogit/staging", "r");
        if (file == NULL) return 1;
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            int length = strlen(line);

            // remove '\n'
            if (length > 0 && line[length - 1] == '\n') {
                line[length - 1] = '\0';
            }

            if (strcmp(file_path, line) == 0) return 0;
        }
        fclose(file);

    
        file = fopen(".neogit/staging","a");
        if (file == NULL) return 1;

        fprintf(file, "%s\n", file_path);
        fclose(file);

        track_file(file_path);

        //find name of file
        char *name=strrchr(file_path,'/');
        if(name!=NULL){
            name++;
        }else{
            name=file_path;
        }    

        //place of copy........................................
        FILE *sourceFile, *destinationFile;
        char ch;

        // باز کردن فایل مبدأ به عنوان خواندنی
        sourceFile = fopen(file_path, "rb");
        if (sourceFile == NULL) {
            perror("Error opening source file");
            return 1;
        }

        if (chdir(".neogit/stages_files") != 0) return 1;
        // باز کردن فایل مقصد به عنوان نوشتنی
        destinationFile = fopen(name, "wb");
        if (destinationFile == NULL) {
            perror("Error opening destination file");
            fclose(sourceFile);
            return 2;
        }

        // خواندن از فایل مبدأ و نوشتن در فایل مقصد
        while ((ch = fgetc(sourceFile)) != EOF) {
            fputc(ch, destinationFile);
        }

        // بستن فایل‌ها
        fclose(sourceFile);
        fclose(destinationFile);


        return 0;
    }
    else{
        return 1;
    }
}

int track_file(char *filepath) {
    if (is_tracked(filepath)) return 0;

    FILE *file = fopen(".neogit/tracks", "a");
    if (file == NULL) return 1;
    fprintf(file, "%s\n", filepath);
    return 0;
}

int is_tracked(char *filepath) {
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/tracks", "r");
    if (file == NULL) return 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        
        if (strcmp(line, filepath) == 0) return 1;

    }
    fclose(file); 

    return 0;
}

int check_file_or_folder(char *path){
    struct stat fileStat;
    if (stat(path, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            return 5;
        } else if (S_ISDIR(fileStat.st_mode)) {
            return 10;
        } else {
            printf("It's neither a regular file nor a directory.\n");
            return 1;
        }
    } else {
        printf("the file(%s) is not existed\n",path);
        return 1;
    }
}

int check_staging(char *filepath){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/staging", "r");
    if (file == NULL) return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL){
        int length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        if (strcmp(filepath, line) == 0) return 0;

    }
    return 1;
}

int remove_from_staging(char *filepath) {

    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

     //find name of file
    char *name=strrchr(filepath,'/');
    if(name!=NULL){
        name++;
    }else{
        name=filepath;
    }   

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    if (access(filepath, F_OK) != -1){
        FILE *file = fopen(".neogit/staging", "r");
        if (file == NULL) return 1;
    
        FILE *tmp_file = fopen(".neogit/tmp_staging", "w");
        if (tmp_file == NULL) return 1;

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            int length = strlen(line);

            // remove '\n'
            if (length > 0 && line[length - 1] == '\n') {
                line[length - 1] = '\0';
            }

            if (strcmp(filepath, line) != 0) {
            fputs(line, tmp_file);
            fputs("\n", tmp_file);
            }
        }
        fclose(file);
        fclose(tmp_file);

        remove(".neogit/staging");
        rename(".neogit/tmp_staging", ".neogit/staging");
        if (chdir(".neogit/stages_files") != 0) return 1; 
        remove(name);
        return 0;
    }
    else{
        return 1;
    }
}

int last_stage(){

    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;
    //find count of line
    int linecount=0;
    char ch;
    file=fopen(".neogit/staging","r");
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            linecount++;
        }
    }fclose(file);
    file=fopen(".neogit/staging","r");
    char line[MAX_LINE_LENGTH];
    for (int i=0; i<linecount; i++) {
        fgets(line, sizeof(line), file);
        int length=strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
    }
    char last_line[1000];
    strcpy(last_line,line);
    fclose(file);
    return remove_from_staging(last_line);
}

int inc_last_commit_ID() {
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    
    FILE *tmp_file = fopen(".neogit/tmp_config", "w");
    if (tmp_file == NULL) return -1;

    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
            last_commit_ID++;
            fprintf(tmp_file, "last_commit_ID: %d\n", last_commit_ID);

        } else fprintf(tmp_file, "%s", line);
    }
    fclose(file);
    fclose(tmp_file);

    remove(".neogit/config");
    rename(".neogit/tmp_config", ".neogit/config");
    return last_commit_ID;
}

char* getCurrentTimeString() {
    // دریافت زمان جاری
    time_t currentTime;
    time(&currentTime);

    // تبدیل زمان به رشته
    char *timeString = ctime(&currentTime);

    // حذف کاراکتر newline از انتهای رشته
    size_t length = strlen(timeString);
    if (length > 0 && timeString[length - 1] == '\n') {
        timeString[length - 1] = '\0';
    }

    return timeString;
    //char *currentTimeStr = getCurrentTimeString();
}

int run_commit(char *message){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    //find count of staging file
    int count_of_file=0;
    file = fopen(".neogit/staging", "r");
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        count_of_file++;
    }
    fclose(file);
    if(count_of_file==0){
        printf("no stage file\n");
        return 1;
    }
    //ID
    int commit_id=inc_last_commit_ID();
    //Time
    char *currentTimeStr = getCurrentTimeString();
    
    //whrite information of commit in file
    FILE *first,*second,*staging;
    char second_path[1000];
    strcpy(second_path,".neogit/commits/");
    char str[20];
    sprintf(str, "%d", commit_id);
    strcat(second_path,str);
    first=fopen(".neogit/config","r");
    second=fopen(second_path,"w");
    int flag=0;
    while (fgets(line, sizeof(line), first) != NULL) {
        fprintf(second, "%s", line);
        flag++;
        if(flag==3)  break;
    }
    fclose(first);
    staging=fopen(".neogit/staging","r");
    fprintf(second, "id: %d\n", commit_id);
    fprintf(second, "message: %s\n", message);
    fprintf(second, "Time: %s\n", currentTimeStr);
    fprintf(second, "count of file: %d\n", count_of_file);
    fprintf(second, "%s\n", "adress of files :" );
    while (fgets(line, sizeof(line), staging) != NULL) {
        fprintf(second, "%s", line);
    }
    fclose(second);
    fclose(staging);
    // make folder and copy stages file
    strcat(second_path,"f");
    if (mkdir(second_path, 0755) != 0) return 1;
    //place of copy........................................
    staging=fopen(".neogit/staging","r");
    while (fgets(line, sizeof(line), staging) != NULL){
        FILE *sourceFile, *destinationFile;
        char ch;

        int length = strlen(line);
        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        char *name=strrchr(line,'/');
        if(name!=NULL){
            name++;
        }else{
            name=line;
        } 

        char second_pwd[1000];
        strcpy(second_pwd,second_path);
        strcat(second_pwd,"/");
        strcat(second_pwd,name);

        // باز کردن فایل مبدأ به عنوان خواندنی
        sourceFile = fopen(line, "rb");
        if (sourceFile == NULL) {
            perror("Error opening source file");
            return 1;
        }

        // باز کردن فایل مقصد به عنوان نوشتنی
        destinationFile = fopen(second_pwd, "wb");
        if (destinationFile == NULL) {
            perror("Error opening destination file");
            fclose(sourceFile);
            return 2;
        }

        // خواندن از فایل مبدأ و نوشتن در فایل مقصد
        while ((ch = fgetc(sourceFile)) != EOF) {
            fputc(ch, destinationFile);
        }

        fclose(sourceFile);
        fclose(destinationFile);
    }
    fclose(staging);

    staging=fopen(".neogit/staging","w");
    fclose(staging);
    //remove files of stages_file
    DIR *dir;
    struct dirent *entry;
    dir=opendir(".neogit/stages_files");
    while ((entry = readdir(dir)) != NULL){
        char cwd[1000];
        strcpy(cwd,pwd);
        strcat(cwd,"/.neogit/stages_files/");
        strcat(cwd,entry->d_name);
        if(check_file_or_folder(cwd)==5){
            remove(cwd);
        }
    }
    //print information of commit
    printf("commit ID:\t%d\n",commit_id);
    printf("commit Time:\t%s\n",currentTimeStr);
    printf("commit Message:\t%s\n",message);
    return 0;
}

int set_shortcut(char *message, char *shortcut_name){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file=fopen(".neogit/shortcut","w");
    fprintf(file, "%s\n", shortcut_name);
    fprintf(file, "%s", message);
    fclose(file);

    return 0;
}

int exist_shortcut(char *shortcut_name){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file=fopen(".neogit/shortcut","r");
    if (file == NULL) return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        if (strcmp(shortcut_name, line) == 0) return 0;
        return 1;
    }
    fclose(file);

}

char *shortcut_message(){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0);

    file=fopen(".neogit/shortcut","r");
    char *line=(char *)malloc(10*sizeof(char));
    int flag=0;
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        if(flag==1)  {  return line;}
        flag++;
    }
    fclose(file);
}

void remove_shortcut(){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0);

    remove(".neogit/shortcut");
}

int run_log(){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
        }
    }

    if (chdir(".neogit/commits") != 0) return 1;
    for(int i=last_commit_ID; i>0; i--){
        char str[20];
        sprintf(str, "%d", i);
        int flag=0;
        printf("commit %s:\n",str);
        file=fopen(str,"r");
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            if(flag==7)  break; 
            flag++;
            printf("%s",line);
        }
        fclose(file);
        printf("---------------------------------------------------\n");
    }

    return 0;
}

int run_log_n(char *n){
    int N=1;
    for(int i=1; i<10; i++){
        char str[20];
        sprintf(str, "%d", i);
        if(strcmp(n,str)==0)  {N=i;  break;}
    }
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
        }
    }
    if(N>last_commit_ID) {printf("erorr");  return 1;}
    if (chdir(".neogit/commits") != 0) return 1;
    for(int i=last_commit_ID; i>last_commit_ID-N; i--){
        char str[20];
        sprintf(str, "%d", i);
        int flag=0;
        printf("commit %s:\n",str);
        file=fopen(str,"r");
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            if(flag==7)  break; 
            flag++;
            printf("%s",line);
        }
        fclose(file);
        printf("---------------------------------------------------\n");
    }

    return 0;
}

int run_log_branch(char *branch){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
        }
    }
    if (chdir(".neogit/commits") != 0) return 1;
    for(int i=last_commit_ID; i>0; i--){
        char str[20];
        sprintf(str, "%d", i);
        int flag=0;
        file=fopen(str,"r");
        char line[MAX_LINE_LENGTH];
        char tmp_branch[30];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strncmp(line, "branch", 6) == 0) {
                sscanf(line, "branch: %s\n", tmp_branch);
            }  
        }
        fclose(file);
        if(strcmp(branch,tmp_branch)==0){
        printf("commit %s:\n",str);
        file=fopen(str,"r");
        while (fgets(line, sizeof(line), file) != NULL) {
            if(flag==7)  break; 
            flag++;
            printf("%s",line);
        }
        fclose(file);
        printf("---------------------------------------------------\n");
        }
        if(i==1 && strcmp(branch,tmp_branch)!=0)  printf("this branch is not existed\n");
    }
    return 0;
}

int run_log_since(char *time){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
        }
    }

    if (chdir(".neogit/commits") != 0) return 1;
    for(int i=last_commit_ID; i>0; i--){
        char str[20];
        sprintf(str, "%d", i);
        int flag=0;
        file=fopen(str,"r");
        char line[MAX_LINE_LENGTH];
        char tmp_time[30];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strncmp(line, "Time", 4) == 0) {
                int length = strlen(line);

                // remove '\n'
                if (length > 0 && line[length - 1] == '\n') {
                    line[length - 1] = '\0';
                    move_str(line,6);
                    strcpy(tmp_time,line);
                }
            }  
        }
        fclose(file);
        printf("%s\n",time);
        printf("%s\n",tmp_time);

        struct tm tmStruct;
        time_t timeValue2;

        // Use strptime to parse the string into a struct tm
        if (strptime(tmp_time, "%a  %b %d %H:%M:%S %Y", &tmStruct) == NULL) {
            fprintf(stderr, "Error parsing time string\n");
            return 1;
        }
        // Convert the struct tm to time_t
        timeValue2 = mktime(&tmStruct);

        time_t timeValue1;
        // Use strptime to parse the string into a struct tm
        if (strptime(time, "%a  %b %d %H:%M:%S %Y", &tmStruct) == NULL) {
            fprintf(stderr, "Error parsing time string\n");
            return 1;
        }
        // Convert the struct tm to time_t
        timeValue1 = mktime(&tmStruct);


        if(timeValue2>timeValue1){
        printf("commit %s:\n",str);
        file=fopen(str,"r");
        while (fgets(line, sizeof(line), file) != NULL) {
            if(flag==7)  break; 
            flag++;
            printf("%s",line);
        }
        fclose(file);
        printf("---------------------------------------------------\n");
        }
    }
    return 0;
    
}
 
int add_branch(char *branch_name){
    //find place of neogit
    FILE *file = fopen("/mnt/d/pwd","r");
    if (file == NULL) return 1;
    char pwd[1000];
    fgets(pwd, sizeof(pwd), file);
    fclose(file);

    //going to neogit place
    if (chdir(pwd) != 0) return 1;

    file=fopen(".neogit/branchs","a");
    fprintf(file,"%s\n",branch_name);
    fclose(file);
}

int main(int argc, char *argv[]){

    if (argc < 2) {
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    char *username="-";
    char *email="-";
    

    if (strcmp(argv[1], "init") == 0) {
        return run_init(argc, argv, username, email);
    }
    else if(strcmp(argv[1],"config")==0){
        if(strcmp(argv[2],"user.name")==0 ){
            username=argv[3];
            make_local_username(username);
        }
        else if(strcmp(argv[3],"user.name")==0){
            username=argv[4];
            make_global_username(username);
        }
        else if(strcmp(argv[2],"user.email")==0 ){
            email=argv[3];
            make_local_email(email);
        }
        else if(strcmp(argv[3],"user.email")==0){
            email=argv[4];
            make_global_email(email);
        }
    }
    else if(strcmp(argv[1],"add")==0){
        if (argc<3)  {printf("please specify a file\n");  return 1;}
        //check wildcard
        if(strcmp(argv[2],"-f")!=0 && strcmp(argv[2],"-n")!=0 && argc>3){
            for(int i=0; i<argc-2; i++){
                if (chdir(cwd) != 0) return 1;
                char *path=find_path(argv[i+2]);
                if(check_file_or_folder(path)==5) add_to_staging(path);  
                else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            add_to_staging(path_tmp); 
                        }
                    }
                }
            }
            }
        }
        //-f
        else if(strcmp(argv[2],"-f")==0){
            for(int i=0; i<argc-3; i++){
                if (chdir(cwd) != 0) return 1;
                char *path=find_path(argv[i+3]);
                if(check_file_or_folder(path)==5) add_to_staging(path);  
                else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            add_to_staging(path_tmp); 
                        }
                    }
                }
            }
            }
        }
        //-n
        else if(strcmp(argv[2],"-n")==0){
            DIR *dir;
            struct dirent *entry;
            dir=opendir(cwd);
            if(dir!=NULL){
                while ((entry = readdir(dir)) != NULL){
                    char pwd[1000];
                    strcpy(pwd,cwd);
                    strcat(pwd,"/");
                    strcat(pwd,entry->d_name);
                    if(check_file_or_folder(pwd)==5){
                        printf("%s\t\t",entry->d_name);
                        if(check_staging(pwd)==0)   printf("stage\n");
                        else printf("not stage\n");
                    }
                    else if(check_file_or_folder(pwd)==10){
                        printf("%s\t\t",entry->d_name);
                        DIR *dir;
                        struct dirent *entry;
                        dir=opendir(pwd);
                        if(dir!=NULL){
                            int flag=0;
                            while ((entry = readdir(dir)) != NULL){
                                char new_pwd[1000];
                                strcpy(new_pwd,pwd);
                                strcat(new_pwd,"/");
                                strcat(new_pwd,entry->d_name);
                                if(check_staging(new_pwd)==0)  flag=1;
                            }
                            if(flag)  printf("stage\n");
                            else  printf("not stage\n");
                        }
                    }
                }
            }
        }
        //standard add
        else{
            char *path=find_path(argv[2]);
            if(check_file_or_folder(path)==5) add_to_staging(path);  
            else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            add_to_staging(path_tmp); 
                        }
                    }
                }
            }
        }
    }
    else if(strcmp(argv[1],"reset")==0){
        if (argc<3)  {printf("please specify a file\n");  return 1;}
        //check wildcard
        if(strcmp(argv[2],"-f")!=0 && strcmp(argv[2],"-n")!=0 && argc>3){
            for(int i=0; i<argc-2; i++){
                if (chdir(cwd) != 0) return 1;
                char *path=find_path(argv[i+2]);
                if(check_file_or_folder(path)==5) remove_from_staging(path);  
                else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            remove_from_staging(path_tmp); 
                        }
                    }
                }
            }
            }
        }
        //-f
        else if(strcmp(argv[2],"-f")==0){
            for(int i=0; i<argc-3; i++){
                if (chdir(cwd) != 0) return 1;
                char *path=find_path(argv[i+3]);
                if(check_file_or_folder(path)==5) remove_from_staging(path);  
                else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            remove_from_staging(path_tmp); 
                        }
                    }
                }
            }
            }
        }
        //-undo
        else if(strcmp(argv[2],"-undo")==0){
            last_stage();
        }
        //standard reset
        else{
            char *path=find_path(argv[2]);
            if(check_file_or_folder(path)==5) remove_from_staging(path);  
            else if(check_file_or_folder(path)==10){
                DIR *dir;
                struct dirent *entry;
                dir = opendir(path);
                if(dir != NULL){
                    while ((entry = readdir(dir)) != NULL) {
                        char path_tmp[1000];
                        strcpy(path_tmp,path);
                        if (entry->d_type == DT_REG) {
                            strcat(path_tmp,"/");
                            strcat(path_tmp,entry->d_name);
                            remove_from_staging(path_tmp); 
                        }
                    }
                }
            }
        }
    }
    //commit
    else if(strcmp(argv[1],"commit")==0 && strcmp(argv[2],"-m")==0){
        if(argc<4){  printf("please input message of commit\n");  return 1; }
        char message[1000];
        strcpy(message,argv[3]);
        if(strlen(message)>72){   printf("characters of Message must be less than 72\n");  return 1; }
        run_commit(message);
    }
    else if(strcmp(argv[1],"set")==0 && strcmp(argv[2],"-m")==0 && strcmp(argv[4],"-s")==0){
        set_shortcut(argv[3],argv[5]);
    }
    else if(strcmp(argv[1],"commit")==0 && strcmp(argv[2],"-s")==0){
        if(!exist_shortcut(argv[3])){
            char message[1000];
            strcpy(message,shortcut_message());
            run_commit(message);
        }
        else {printf("you don't set this shortcut\n");   return 1;}
    }
    else if(strcmp(argv[1],"remove")==0 && strcmp(argv[2],"-s")==0){
        remove_shortcut();
    }
    else if(strcmp(argv[1],"replace")==0 && strcmp(argv[2],"-m")==0 && strcmp(argv[4],"-s")==0){
        if(exist_shortcut(argv[5]))   printf("you don't set this shortcut\n");
        else   set_shortcut(argv[3],argv[5]);
    }
    //log
    else if(strcmp(argv[1],"log")==0 && argc==2){
        run_log();
    }
    //-n
    else if(strcmp(argv[1],"log")==0 && strcmp(argv[2],"-n")==0 ){
        run_log_n(argv[3]);
    }
    //-branch
    else if(strcmp(argv[1],"log")==0 && strcmp(argv[2],"-branch")==0 ){
        run_log_branch(argv[3]);
    }
    else if(strcmp(argv[1],"log")==0 && strcmp(argv[2],"-since")==0 ){
        run_log_since(argv[3]);
    }

    return 0;

}