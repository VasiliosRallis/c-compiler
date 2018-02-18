z=4

def factorial(a) :
    global z
    if(a==1):
        return 1
    else:
        return a*factorial(a-1)

x=3

def main():
    global z
    global x   
    return factorial(5)+factorial(x)+factorial(z)

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

