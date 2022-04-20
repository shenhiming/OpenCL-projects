//
//  basic_struct.hpp
//  TestOpenCL
//
//  Created by ＰＭＳＤ on 2021/3/18.
//  Copyright © 2021 ＰＭＳＤ. All rights reserved.
//

#ifndef basic_struct_h
#define basic_struct_h
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/types.h>

typedef struct _cl_basic_struct
{
    cl_context context;
    cl_int err;
    cl_device_id *devices;
    char *dev_name;
    size_t num_devices;
    cl_command_queue queue;
}cl_basic_struct;
#endif /* basic_struct_hpp */
