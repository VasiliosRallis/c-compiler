float add(float n1, int n2, float n3, int n4, float n5);

int main(){
    float n1 = 3.4;
    int n2 = 1;
    float n3 = 12.6;
    int n4 = 3;
    float n5 = 12.33;
    return n1+n2+n3+n4+n5 == add(n1,n2,n3,n4,n5);
}
