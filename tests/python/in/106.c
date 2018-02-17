int main(){
    int a =0;
    int b = 1;
    if (a == b) {
        b =5;
        if(a < b )
            b=7;
        else 
            b=6;
    }
    return b;
}
