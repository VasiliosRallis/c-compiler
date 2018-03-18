int main(){
    int x =0;
    int y = 0;
    int sumofCombi =0;
	while(x<=5){
        while(y<=5){
            sumofCombi = sumofCombi +x+y;
            y++;
       }
        x++;
    }
	
	return sumofCombi;
}
