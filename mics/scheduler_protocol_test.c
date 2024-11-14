#include "def.h"
#include "scheduler_def.h"
#include "scheduler.h"
#include "utils.h"
#include "wrapper.h"


static void
test_protocol(void)
{
    char *json_string;

    json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
    printf("An example `request` operation send to the gloabl scheduling module.\n");
    printf("%s\n\n", json_string);
    free(json_string);

    json_string = __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
    printf("An example `acknowledge` operation send to the gloabl scheduling module.\n");
    printf("%s\n\n", json_string);
    free(json_string);
}


int
main(int argc, char *argv[])
{
    test_protocol();
    return 0;
}
