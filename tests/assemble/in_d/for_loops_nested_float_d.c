float sumCombi(float n1, float n2){
    float b=0;

    float i =0;
    float j =0; 
	for(i=n1;i<5;i++){
		for(j=n2;j<5;j=j+1){
			b=b + i+ j;
		}
	}
    return b;
}
