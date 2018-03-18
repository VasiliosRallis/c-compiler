float factflo(float n1);

int main(){
    float n1 = 8.3;

    float a=1;
    float i = 0; 
	for(i=n1;i>=0;--i){
			a=a * i;
	}
    
    return (a == factflo(n1));
}
	
