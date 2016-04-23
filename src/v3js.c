#include <v3_core.h>
#include <stdio.h>
#include <string.h>

#define PROMPT "> "
#define V3_VERSION "0.1"
static void show_version_info();

int main(int argc, char ** argv)
{
    FILE        *stream;
    char        buf[4096];
    stream = stdin;
    show_version_info();
    // v3_init_global();
    // init global 

    // pending
    // resolve external file passed by argv
        // parse
        // execute
        // show result or err
        // return 

    // read input
    printf(PROMPT);
    while (fgets(buf, sizeof(buf), stream) != NULL) {
        // parse 
        // execute
        // show result or err
        // read input
        printf(PROMPT);
    }

    printf("bye \n");
    return 0;
}

static void show_version_info()
{
    printf("welcome to v3 javascript interpretor %s\n", V3_VERSION);
    printf("\n");
}
