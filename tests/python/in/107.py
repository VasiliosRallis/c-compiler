def main():
    a =0
    b = 5
    while(b>0):
        a = a +b
        b = b -1
    return a


# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

