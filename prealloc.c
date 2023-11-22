#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smallchesslib.h"

// based on data
#define MAX_PGN_SIZE 1400

typedef struct
{
    char *str;
    size_t len;
} String;

void initString(String *s)
{
    s->str = malloc(1); // Allocate a single byte initially
    s->str[0] = '\0';   // Null-terminate the string
    s->len = 0;
}

void freeString(String *s)
{
    free(s->str);
}

void appendStringChar(String *s, char c)
{
    s->str = realloc(s->str, s->len + 2); // Allocate space for new char and null-terminator
    s->str[s->len++] = c;                 // Append the character
    s->str[s->len] = '\0';                // Null-terminate the string
}

void myPutChar(char c, void *arg)
{
    String *s = (String *)arg;
    appendStringChar(s, c);
}

// Usage example
int main()
{
    SCL_Record record;
    const char *SAN = "1.c4 c5 2.Nf3 Nf6 3.d4 cxd4 4.Nxd4 e6 5.g3 d5 6.Bg2 e5 7.Nc2 d4 8.O-O Nc6 9.Nd2 Bg4 10.Nf3 a5 11.Bg5 Bc5 12.e4 h6 13.Bxf6 Qxf6 14.Nce1 Qe6 15.Nd3 Be7 16.h3 Bxf3 17.Qxf3 Qxc4 18.Rfc1 Qe6 19.Qf5 Bd6 20.Qxe6+ fxe6 21.a3 Kd7 22.Bf1 a4 23.Ne1 Ra5 24.Rc2 Rc8 25.Rac1 Na7 26.Rxc8 Nxc8 27.Bc4 Be7 28.Nd3 Nd6 29.f3 Nxc4 30.Rxc4 Bd6 31.Kf2 Ra6 32.Ke2 g5 33.Kd1 Ra8 34.Ke2 h5 35.Rc1 Rh8 36.Rh1 h4 37.g4 b5 38.Rc1 Rb8 39.Kd1 Rb6 40.Rc2 Rb8 41.Rc1 Rc8";
    SCL_recordFromPGN(record, SAN);
    SCL_Board initialState = SCL_BOARD_START_STATE;

    // String pgnString;
    // initString(&pgnString);

    SCL_printPGN(record, myPutChar, initialState);
    // printf("%s\n", pgnString.str);

    // freeString(&pgnString);

    return 0;
}
