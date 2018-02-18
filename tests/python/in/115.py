x=3

def f() :  
    global x
    return 2+x

def main():
    global x
    y = 2+x;   
    return y+f()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

