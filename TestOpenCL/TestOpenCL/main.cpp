//
//  main.cpp
//  TestOpenCL
//
//  Created by ＰＭＳＤ on 2021/2/17.
//  Copyright © 2021 ＰＭＳＤ. All rights reserved.
//


#include "AddBigNum/AddBigNum.h"
#include "KernelSearch/kernel_search.h"

int Allocation(cl_basic_struct &basic_struct)
{
    cl_int &err = basic_struct.err;
    cl_uint num;
    basic_struct.err = clGetPlatformIDs(0, 0, &num);
    if(err != CL_SUCCESS) {

        std::cerr << "Unable to get platforms\n";
        return 0;

    }
    // Obtain platfrom ID
    std::vector<cl_platform_id> platforms(num);
    err = clGetPlatformIDs(num, &platforms[0], &num);
    if(err != CL_SUCCESS) {

        std::cerr << "Unable to get platform ID\n";

        return 0;

    }
#ifndef __APPLE__
    cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0 };
    // Use system default CL device
    cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, NULL);
    if(context == 0) {

        std::cerr << "Can't create OpenCL context\n";
        return 0;

    }
#else

    
    cl_context &context = basic_struct.context;
    // Use a GPU context
    context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);
    if(context == 0) {
        printf("Can't create GPU context\n");
        return 0;
    }
#endif
    // get a list of devices
    cl_device_id *&devices = basic_struct.devices;
    char *&devname = basic_struct.dev_name;
    size_t &cb = basic_struct.num_devices;
     
     
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    devices = (cl_device_id*) malloc(cb);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, 0);

    // show the name of the first device
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    devname = (char*) malloc(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, devname, 0);
      
    cl_uint addr_data;
    /* Extension data */
    char ext_data[4096];

    /* Access device address size */
    clGetDeviceInfo(devices[0], CL_DEVICE_ADDRESS_BITS,
       sizeof(addr_data), &addr_data, NULL);

    /* Access device extensions */
    clGetDeviceInfo(devices[0], CL_DEVICE_EXTENSIONS,
       4096 * sizeof(char), ext_data, NULL);

    printf("NAME: %s\nADDRESS_WIDTH: %u\nEXTENSIONS: %s\n",
       devname, addr_data, ext_data);

    // Create command queue to queue operation for the device.
    // The following code is create a command queue for device[0], each device owns a command queue itself.
    cl_command_queue &queue = basic_struct.queue;
    queue = clCreateCommandQueue(context, devices[0], 0, 0);
    if(queue == 0) {

        std::cerr << "Can't create command queue\n";
        clReleaseContext(context);
        return 0;

    }
    return 0;
}

int Release(cl_basic_struct &basic_struct)
{
    // release everything
    clReleaseCommandQueue(basic_struct.queue);

    free(basic_struct.dev_name);
    free(basic_struct.devices);
    clReleaseContext(basic_struct.context);
    return 0;
}

int main (int argc, const char * argv[])
{
    printf("Path relative to the working directory is: %s\n", argv[0]);
    // Obtain platform ID numbers
    cl_basic_struct basic_struct;
    int allocateion_result = Allocation(basic_struct);
    if(allocateion_result == -1)
    {
        return -1;
    }
    //int result = AddBigNum(basic_struct);
    int result = KernelSearch(basic_struct);
    if(result == -1)
    {
      
    
    }
    
    int release_result = Release(basic_struct);
    return 0;
}
