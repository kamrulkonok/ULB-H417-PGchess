#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smallchesslib.h"

// Function to remove newlines from a string
void removeNewlines(char *str)
{
    char *dest = str;

    while (*str)
    {
        if (*str != '\n' && *str != '\r')
        {
            *dest++ = *str;
        }
        str++;
    }
    *dest = '\0';
}

// Function to trim leading and trailing whitespaces
char *trimWhitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    *(end + 1) = 0;

    return str;
}

// Function to convert PGN to CSV
void convertPGNtoCSV(const char *inputFilePath, const char *outputFilePath)
{
    FILE *inputFile = fopen(inputFilePath, "r");
    FILE *outputFile = fopen(outputFilePath, "w");

    if (!inputFile || !outputFile)
    {
        // Handle file open error
        return;
    }

    fprintf(outputFile, "SAN,FEN\n"); // Write CSV header

    char line[1024];
    char sanMoves[4096] = ""; // Buffer to store all SAN moves
    char fen[4096];
    int isReadingMoves = 0;

    while (fgets(line, sizeof(line), inputFile))
    {
        // Check if line is a metadata tag
        if (line[0] == '[')
        {
            isReadingMoves = 0;
            continue;
        }

        // Check if line contains moves
        if (strchr(line, '.'))
        {
            isReadingMoves = 1;
            strcat(sanMoves, line);
            continue;
        }

        // Process the SAN moves to generate FEN
        if (isReadingMoves)
        {
            // Remove newlines from sanMoves
            removeNewlines(sanMoves);

            SCL_Record r;
            SCL_recordInit(r);
            SCL_recordFromPGN(r, sanMoves);
            SCL_Board board = SCL_BOARD_START_STATE;
            SCL_recordApply(r, board, 100);
            SCL_boardToFEN(board, fen);

            // Trim and write to CSV file
            fprintf(outputFile, "%s,%s\n", trimWhitespace(sanMoves), trimWhitespace(fen));

            // Reset for next game
            memset(sanMoves, 0, sizeof(sanMoves));
            isReadingMoves = 0;
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}

int main()
{
    convertPGNtoCSV("Pgndemo.pgn", "Pgndemo.csv");
    return 0;
}
