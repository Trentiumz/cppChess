files = ["pawn", "bishop", "knight", "rook", "queen", "king"]
print("{")
for i in files:
    arr = [[] for _ in range(8)]
    with open(i + ".pr", "rt") as fileInput:
        for r in range(8):
            arr[7 - r] = [float(x) for x in fileInput.readline().split()]
    print("{")
    print("\n".join(map(lambda row: "{" + ", ".join(map(str, row)) + "},", arr)))
    print("},")
    print()
print("};")
