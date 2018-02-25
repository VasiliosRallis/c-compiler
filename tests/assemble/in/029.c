int main(){
    int a = 1;
    while(a != 4){
        int b = 2;
        for(; b != 3;){
            int c = 23 + 1;
            int d = 3;
            while(d != 4){
                d = d + 1;
            }
            b = b + 1;
        }
        a = a + b;
    }
    return a;
}
