int x = 2;
int f(){
    x = 5;
    return x;
}
int main(){
    int a = f();
    return x;
}
