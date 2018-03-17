int a[512] = {1};

int main(){
    int i;
    for(i = 0; i < 512; ++i){
        a[i] = 15;
    }
    return a[511];
}
