def main():
    with open("testing/output.txt", "r") as f1, open("testing/nestestlog.txt", "r") as f2:
        for line_num, (line1, line2) in enumerate(zip(f1, f2), start=1):
            # print(f"{line1[5:]=} {line2[86:]=}")
            if line1[5:] != line2[86:]:
                print(f"Difference found in line '{line1[:4]}'")
                print(f"   Incorrect cycles: {line1[9:].strip()}")
                return

if __name__ == "__main__":
    main()

