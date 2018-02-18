z=0

def f() :
    global z
    return 2+z

x=0

def main():
    global z
    global x        
    y = 2+x+z   
    return y+f()


# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

