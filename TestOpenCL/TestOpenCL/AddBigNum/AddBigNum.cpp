//
//  AddBigNum.cpp
//  TestOpenCL
//
//  Created by ＰＭＳＤ on 2021/3/15.
//  Copyright © 2021 ＰＭＳＤ. All rights reserved.
//

#include "AddBigNum.h"

// Simple compute kernel which computes the square of an input array
//
const char *KernelSource = "\n" \
"kernel void adder(                                                       \n" \
"   global float* a,                                               \n"\
"   global float* b,                                              \n" \
"   global float* output                                             \n" \
"   )                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   output[i] = a[i] + b[i];                                \n" \
"}                                                                      \n" \
"\n";


int AddBigNum(cl_basic_struct &basic_struct)
{
    cl_context &context = basic_struct.context;
    cl_int &err = basic_struct.err;
    cl_device_id *&devices = basic_struct.devices;
    cl_command_queue &queue = basic_struct.queue;
    

    // Generate big test data
    const int DATA_SIZE = 1048576;
    std::vector<float> a(DATA_SIZE), b(DATA_SIZE), res(DATA_SIZE);
    for(int i = 0; i < DATA_SIZE; i++) {

        a[i] = std::rand();
        b[i] = std::rand();

    }

    // Allocate memory and copy data
    // When the operation run on OpenCL device, it needs allocate memoery on OpenCL device and copy data from main memory to device.
    // Some OpenCL device can access data directly from main memory but the speed is slow since OpenCL device (such as display card) has its high speed memory.
    // CL_MEM_WRITE_ONLY
    // Device can write memory but cannot read.
    // CL_MEM_COPY_HOST_PTR
    // This pararmeter is effective when host_ptr is not NULL. It requires OpenCL allocate memory space for the storage and copy the content which host_ptr pointed to the crresponding storage.
    // If it has isolated GPU, it can allocate a space in main memory, create and initialize cl_men object, then free this space to save main memoery and use GPU's memory.
    // CL_MEM_ALLOC_HOST_PTR
    // This parameter makes OpenCL can invite storage to put buffer in main program. It is exclusive with CL_MEM_USE_HOST_PTR
    // CL_MEM_USE_HOST_PTR
    // This pararmeter is effective when host_ptr is not NULL. The storage which pointered by host_ptr is used to cl_mem object. The content which host_ptr point to will be cached to corresponding device. Then kernel can use this content while executing.
    // If kernel is run on CPU, it can avoid unnecessary cache operation.
    cl_mem cl_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &a[0], NULL);
    cl_mem cl_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &b[0], NULL);
    cl_mem cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * DATA_SIZE, NULL, NULL);
    if(cl_a == 0 || cl_b == 0 || cl_res == 0) {

        std::cerr << "Can't create OpenCL buffer\n";
        clReleaseMemObject(cl_a);
        clReleaseMemObject(cl_b);
        clReleaseMemObject(cl_res);
        return -1;

    }

    // Create the compute program from the source buffer
    //
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel

    cl_mem input;                       // device memory used for the input array
    cl_mem output;                      // device memory used for the output array

    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }

    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return -1;
    }

    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "adder", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        return -1;
    }

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_a);

    clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_b);

    clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_res);


    size_t work_size = DATA_SIZE;

    err = clEnqueueNDRangeKernel(queue, kernel, 1, 0, &work_size, 0, 0, 0, 0);


    if(err == CL_SUCCESS) {

        err = clEnqueueReadBuffer(queue, cl_res, CL_TRUE, 0, sizeof(float) * DATA_SIZE, &res[0], 0, 0, 0);

    }


    if(err == CL_SUCCESS) {

        bool correct = true;

        for(int i = 0; i < DATA_SIZE; i++) {

            if(a[i] + b[i] != res[i]) {

                correct = false;

                break;

            }

        }
        if(correct) {
            std::cout << "Data is correct\n";
        }
        else {
            std::cout << "Data is incorrect\n";
        }
    }
    else
    {
        std::cerr << "Can't run kernel or read back data\n";
    }
    return 0;
}
