import sys

def repair(inputFile, outputFile):
    with open(inputFile, 'r', encoding = 'utf-8', errors = 'replace') as inFile:
        with open(outputFile, 'w', encoding = 'utf-8') as outFile:
            for line in inFile:
                outFile.write(line)
        outFile.close()
    inFile.close()
if __name__ == "__main__":
    if len(sys.argv) == 3:
        inputfile = sys.argv[1]
        outputfile = sys.argv[2]
        repair(inputfile, outputfile)
    else :
        print("Please gibe me the input file first then the output file.")
