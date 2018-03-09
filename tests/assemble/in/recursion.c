int addr(int n){
    if(n == 1) return 1;
    else return n + addr(n-1);
}

int main(){
    return addr(4);
}
