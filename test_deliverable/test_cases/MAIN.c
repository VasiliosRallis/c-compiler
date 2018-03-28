int a = 0;
int f(){
    a = 5;
    return 1;
}

int main(){
    return a;
}

int g(){
    a = 3;
    return 1;
}
