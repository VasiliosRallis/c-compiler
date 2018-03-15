float add(float n1, float n2, float n3, float n4, float n5);

int main(){
    float n1 = 1.2;
    float n2 = 1.8;
    float n3 = 3.6;
    float n4 = 11.2;
    float n5 = 13.2;
    return n1+n2+n3+n4+n5 == add(n1,n2,n3,n4,n5);
}
