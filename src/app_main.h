#ifndef INCLUDE_APP_MAIN_H
#define INCLUDE_APP_MAIN_H

// Putting the main logic here allows us to call it separately for testing
// purposes.
int app_main(int argc, char **argv);

// Used for passing arg information to pthreads.
typedef struct ArgsStruct
{
    int argc;
    char **argv;
} Args;

#endif // INCLUDE_APP_MAIN_H