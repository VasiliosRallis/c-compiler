def main():
    a =0
    b = 1
    if (a == b):
        b =5
        if(a < b ):
            b=7
        else: 
            b=6
    return b


# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

