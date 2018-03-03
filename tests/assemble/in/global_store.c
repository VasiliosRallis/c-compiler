int x = 3;
int f(){
    return x;
}
int main(){
    x = 4;
    return f();
}
