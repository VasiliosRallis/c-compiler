int atoi(char* str);

int test(){
    char str[] = {'2','3','\000'};
    return atoi(str);
}
