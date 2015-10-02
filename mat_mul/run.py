import os
import sys

def sourceFileParse(sourceFileName, dest, numOfCores, matSize): 
    fWrite = open(dest, "w")                                                
    try:                                                                        
        f = open(sourceFileName)                                                
    except IOError:                                                             
        handleIOError(sourceFileName, "csource file" )                                               
        exit()                                                                  
    else:                                                                       
        with f:                                                                 
            for line in f:                                                      
                if ("numOfCores" in line.split()):
                    fWrite.write("#SBATCH -n " + str(numOfCores) + "\n"); 
                elif ("time" in line.split()):
                    if (sourceFileName == "test_mm.sbatch"): 
                        modifiedLine = line.split()
                        modifiedLine[4]= matSize 
                        newLine = " ".join(modifiedLine)
                        fWrite.write(newLine)
                    else:
                        modifiedLine = line.split()
                        modifiedLine[5]= matSize 
                        newLine = " ".join(modifiedLine)
                        fWrite.write(newLine)
                

                else:
                    fWrite.write(line)

def main():
    
    dest = "output.sbatch" 
    print "order of input is mode(seq or par) numberOfCores matrixDim\n"
#    inputFileName = "mat_mul.sbatch" 
#    numOfCores = 5
#    
    
    os.system("make");
    mode = sys.argv[1]
    numOfCores = sys.argv[2]
    matrixDim = sys.argv[3]
    if (mode == "seq"):
        inputFileName = "test_mm.sbatch"
        os.system("rm test_mm.batchOut*")
    else:
        inputFileName = "mat_mul.sbatch"
        os.system("rm mat_mul.batchOut*")
    sourceFileParse(inputFileName, dest,numOfCores, matrixDim )
    os.system("sbatch " + dest)
    


main()
