int x = 2;
int f(){
    return x;
}
int main(){
    int x = x+1;
    return f() + x;
}
