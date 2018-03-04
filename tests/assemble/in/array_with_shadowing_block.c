int main(){
    int array[3] = {1,2,3};
    {int array[3] = {4,5,6};}
    return array[0];
}
