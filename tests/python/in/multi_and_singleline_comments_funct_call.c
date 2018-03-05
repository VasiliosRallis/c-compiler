int x;

int f()
{
    x=x+1;
    return x;
}

int main()
{
    /* The order of execution of these functions
     is not defined, but either order results in
    //  the same result
    *
    ****/
    //121214
    /*
    return f(); 
    */
    //121214
    return f()+f();
    /*
    Making sure the multi line comments working well
    ****** //
        
        */
}

