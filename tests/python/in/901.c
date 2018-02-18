int z =4;
int gcd(int a , int b) {
    while(a!=b){
        if(a>b){
            a = a-b;
        }
        else{
            b =b-a ;
        }
    }
    return a;
}

int x=3;

int main(){   
    return gcd(x*z*100,500) +gcd(x,z);
}

