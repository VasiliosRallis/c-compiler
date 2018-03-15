int main(){
    int a[3] = {1,2,3};
    int *b = &a;
    *b = 20;
    return a[0];
}
