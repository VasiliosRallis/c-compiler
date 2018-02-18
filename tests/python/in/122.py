z=4

def f() :
    global z
    return 2+z

x=3

def main():
    global z
    global x        
    y = 2+x+z   
    return y+f()

a =1

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

