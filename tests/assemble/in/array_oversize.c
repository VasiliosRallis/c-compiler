int main(){
    int a[512] = {1};
    int i = 0;
    for(; i < 512; i++){
        a[i] = 15;
    }
    return a[511];
}
