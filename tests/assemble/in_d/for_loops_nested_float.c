float sumCombi(float n1, float n2);

int main(){
    float n1 = 0.5;
    float n2 = 0.7;

    float a=0;
    float i =0;
    float j =0; 
	for(i=n1;i<5;i++){
		for(j=n2;j<5;j=j+1){
			a=a + i+ j;
		}
	}
    
    return (a == sumCombi(n1,n2));
}
