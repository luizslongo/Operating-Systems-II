from time import time

while True:
    line = input()
    if line.startswith("TIME:"):
        t = time()
        print(f"cur: {t}, got: {line.split()[1]}")
        break
