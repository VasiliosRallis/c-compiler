int z =4;
int factorial(int a) {
    if(a==1){
        return 1;
    }
    else{
        return a*factorial(a-1) ;
    }
}

int x=3;

int main(){   
    return factorial(5)+factorial(x)+factorial(z);
}

