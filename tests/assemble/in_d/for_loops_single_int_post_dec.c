int factorial(int n1);

int main(){
    int n1 = 9;

    int a=1;
    int i = 0; 
	for(i=n1;i>=0;i--){
			a=a * i;
	}
    
    return (a == factorial(n1));
}
	
