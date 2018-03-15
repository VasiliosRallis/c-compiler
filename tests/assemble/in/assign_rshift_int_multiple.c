int main(){
    int x =2147483647;
    int y=x>>=20;
    x=y>>10;
    return x;
}
