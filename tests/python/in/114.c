int x ;

int f() {
    return 2+x;
}

int main(){
    int  y = 2+x;   
    return y+f();
}

