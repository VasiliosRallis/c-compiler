int main(){
    int x =1;
    int y = 1;
    int sumofCombi =0;
    do{
        do{
            sumofCombi = sumofCombi +x*y;
            y++;
        }while(y<10);
        x++;
    }while(x<10);
	
	return sumofCombi;
}
