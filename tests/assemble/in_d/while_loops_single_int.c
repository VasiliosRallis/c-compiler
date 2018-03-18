int sum(int n1);

int main(){
    int n1 = 9;

    int a=1;
    int i = 0; 
	while(a < n1){
        i = i+a;
        a++;
    }
    
    return (a == sum(n1));
}
	
