x=4;
z=0;

def f() :
    global x
    global z
    return 2+z

def main():
    global x 
    global z
    y = 2+x   
    return y+f()


# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

