x=0
def main():
    global x
    x = x + 5
    return x + 5

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

