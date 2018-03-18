int main(){
    float x =1.5;
    float y = 1.5;
    float sumofCombi =0;
    do{
        do{
            sumofCombi = sumofCombi + x*y;
            y++;
        }while(y<10);
        x++;
    }while(x<10);
	
	return sumofCombi;
}
