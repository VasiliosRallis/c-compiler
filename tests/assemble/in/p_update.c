int main(){
    int a = 2;
    int* b = &a;
    a = 3;
    return *b;
}
