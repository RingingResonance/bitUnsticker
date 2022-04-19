/*
Bit Un-Sticker for recovering marginally corrupted binary data.
Jarrett Cigainero 2022
*/
#include <stdio.h>

int main(int argc, char *argIN[]){
    //Pars arguments from command line.
    if (argc != 3) printf("Bad syntax. Give filenames! Input binary file and rules binary file. \n");

	//Open Files.
	printf("Opening input file.\n");
	FILE *INfile = fopen(argIN[1], "rb");	//gets file pointer of file given in argument
	if (!INfile){
		printf("Could not open input data file.\n");
		return 0;
	}
    printf("Opening rules file.\n");
	FILE *RULESfile = fopen(argIN[2], "rb");	//gets file pointer of file given in argument
	if (!RULESfile){
		printf("Could not open rules data file.\n");
		return 0;
	}

	// Load file into allocated array
	//--------------------------------------->
	int IN_file_size;				    //	sizeof file array
	int rules_file_size;				//	sizeof file array
    //Get size of input file.
	fseek(INfile, 0, SEEK_END); 		    // seek to end of file
	IN_file_size = ftell(INfile); 		    // get current file pointer
	fseek(INfile, 0, SEEK_SET); 		    // seek back to beginning of file
	char BinaryInput[IN_file_size];			// create input file array

	// Read input file into input array.
	for (int INfileIndex = 0; INfileIndex <= IN_file_size; INfileIndex++)
		BinaryInput[INfileIndex] = fgetc(INfile);
    //Close input file.
	fclose(INfile);
    //Get size of rules file.
	fseek(RULESfile, 0, SEEK_END); 		        // seek to end of file
	rules_file_size = ftell(RULESfile); 		// get current file pointer
	fseek(RULESfile, 0, SEEK_SET); 		        // seek back to beginning of file
	char RulesInput[rules_file_size];			// create input file array

	// Read rules file into rules array.
	for (int RulesfileIndex = 0; RulesfileIndex <= rules_file_size; RulesfileIndex++)
		RulesInput[RulesfileIndex] = fgetc(RULESfile);
    //Close rules file.
	fclose(RULESfile);
/***********************************************************************************************/
    //Find stuck bits. If more than one error out and notify the user.
    char bitData = 0;
    char bitTest = 1;
    int i=0;
    char isStuck = 0;
    char numOfStuckBits = 0;
    char markedBits = 0;
    //Get the stuck bits if any.
    for(int b=0;b<8;b++){
        i=0;
        bitData = BinaryInput[i] & bitTest;
        for(i=0;i<IN_file_size;i++){
            if(bitData != (BinaryInput[i] & bitTest)){
                isStuck = 0;
                break;
            }
            else isStuck = 1;
        }
        if(isStuck){
            numOfStuckBits++;
            markedBits+=bitTest;
        }
        bitTest *= 2;
    }
    if(numOfStuckBits > 1){
        printf("Number of stuck bits is %d. I cannot fix this. :( \n", numOfStuckBits);
        return 0;
    }
    else if (!numOfStuckBits){
        printf("No stuck bits found. \n");
        return 0;
    }
    else printf("Found a stuck bit, attempting to fix. \n");

    int foundMatchs = 0;
    int totalCorrections = 0;
    int totalBad = 0;
    int totalDuplicats = 0;
    int totalMatched = 0;
    //Open a log file.
    FILE *Logfile = fopen("log.txt", "wb");
    fprintf(Logfile, "\n");
    fprintf(Logfile, "Logfile for Bit Unsticker Recovery. 2022 \n");
    fprintf(Logfile, "Input Binary File: ");
    fprintf(Logfile, argIN[1]);
    fprintf(Logfile, "\n");
    fprintf(Logfile, "Rules Binary File: ");
    fprintf(Logfile, argIN[2]);
    fprintf(Logfile, "\n");
    fprintf(Logfile, "\n");
    printf("Stuck Bit Value: %d \n", markedBits);
    for(int idx=0;idx<IN_file_size;idx++){
        foundMatchs = 0;
        for(int i=0;i<rules_file_size;i++){
            //Check bits against rules.
            if(BinaryInput[idx] == RulesInput[i]){
                foundMatchs++;
                totalMatched++;
                break;
            }
        }
        for(int i=0;i<rules_file_size;i++){
            //Check flipped bit against rules.
            if((BinaryInput[idx] ^ markedBits) == RulesInput[i]) {
                foundMatchs++;
                //Don't flip the bit if more than one combo is found. Keep the original.
                if(foundMatchs != 2){
                    totalCorrections++;
                    BinaryInput[idx] ^= markedBits;
                }
                break;
            }
        }
        //Log results if a single match wasn't found.
        if(foundMatchs == 2) {
            fprintf(Logfile, "Two matches found for address %d \n", idx);
            totalDuplicats++;
        }
        else if (foundMatchs == 0){
            fprintf(Logfile, "No match found for address %d \n", idx);
            totalBad++;
        }
    }

	//Output the hopefully fixed data to a new file.
    FILE *file = fopen("output.bin", "wb");
    for(int fileIndex=0; fileIndex<IN_file_size; fileIndex++)
        fputc(BinaryInput[fileIndex], file);
    printf("Done writing new data file. %d total bytes processed. \n", IN_file_size);
    printf("Total Corrected %d bytes.\n", totalCorrections);
    printf("Total Uncorrected %d bytes.\n", IN_file_size - totalCorrections);
    printf("Found %d bytes already matched rules. Nothing done with these. \n", totalMatched);
    printf("Found %d bytes that didn't have a match with rules. Uncorrected. \n", totalBad);
    printf("Found %d double possibilities with rules. Uncorrected. \n", totalDuplicats);

    fprintf(Logfile, "Done writing new data file. %d total bytes processed. \n", IN_file_size);
    fprintf(Logfile, "Total Corrected %d bytes.\n", totalCorrections);
    fprintf(Logfile, "Total Uncorrected %d bytes.\n", IN_file_size - totalCorrections);
    fprintf(Logfile, "Found %d bytes already matched rules. Nothing done with these. \n", totalMatched);
    fprintf(Logfile, "Found %d bytes that didn't have a match with rules. Uncorrected. \n", totalBad);
    fprintf(Logfile, "Found %d double possibilities with rules. Uncorrected. \n", totalDuplicats);

	return 0;
}

