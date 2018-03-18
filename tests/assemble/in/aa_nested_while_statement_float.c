int main(){
    float x =11.3;
    float y = 10.3;
    float sumofCombi =0;
	while(x>=5){
        while(y>=5){
            sumofCombi = sumofCombi +x+y;
            y--;
       }
        --x;
    }
	
	return sumofCombi;
}
