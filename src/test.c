#include "flow_parse.h"

int main(int argc, char *argv[])
{
    kdk_uint32                   ret_code;
    flow_branch_collection_t    *collection;
    flow_branch_t               *main_branch;

    collection = flow_branch_collection_create(NULL, 1024, 3);
    if(collection == KDK_NULL)
    {
        fprintf(stderr, "flow_branch_collection_create error!\n");
        return -1;
    }
    fprintf(stderr, "flow_branch_collection_create success!\n");

    kdk_char32  flow_stream[1024] = "001-002[DEF,00E-00F-00G]-003[DEF,00E-00F-00G][092,00G-00H][DEF,00E-00F-00G]-004[DEF,00E-00F-00G]-005[DEF,00E-00F-00G][DEF,00E-00F-00G]-006";
    //kdk_char32  flow_stream[1024] = "001-002[DEF,00E-00F-00G]-003[DEF,00E-00F-00G-004]-004[DEF,00E-00F-00G]-005[DEF,00E-00F-00G][092,003-004]-006";

    //kdk_char32  flow_stream[1024] = "001-002-003-004-005-006";

    ret_code = flow_branch_set(collection, "SEV001", flow_stream);
    if(ret_code)
    {
        fprintf(stderr, "flow_branch_set error!\n");
        return -1;
    }
    fprintf(stderr, "flow_branch_set success!\n");

    main_branch = flow_branch_get(collection, "SEV001");
    if(main_branch == KDK_NULL)
    {
        fprintf(stderr, "flow_branch_get error!\n");
        return -1;
    }
    fprintf(stderr, "flow_branch_get success!\n");

    flow_branch_print(main_branch);
    
    return 0;
}
