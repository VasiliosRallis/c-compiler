int f2(int a){
    return a;
}

int f1(int a){
    f2(2);
    return a;
}

int main(){
    return f1(5);
}
