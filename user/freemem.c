#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    printf("Free memory: %d bytes\n", freemem());
    return 0;
}