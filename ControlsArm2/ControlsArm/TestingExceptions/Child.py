try:
    def divideZero():
        return 5/0

except Exception as ex:
    print(f"an exception has occurred: {ex}")