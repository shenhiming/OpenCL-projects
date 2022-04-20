//
//  kernel_serach.cpp
//  TestOpenCL
//
//  Created by ＰＭＳＤ on 2021/3/18.
//  Copyright © 2021 ＰＭＳＤ. All rights reserved.
//

#include "map_copy.h"
#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "map_copy.cl"
#define KERNEL_FUNC "blank"
int MapCopy(cl_basic_struct &basic_struct)
{
    /* Data and buffers */
    float data_one[100], data_two[100], result_array[100];
    void* mapped_memory;

    /* Initialize arrays */
    for(int i=0; i<100; i++) {
       data_one[i] = 1.0f*i;
       data_two[i] = -1.0f*i;
       result_array[i] = 0.0f;
    }
    
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    cl_kernel *kernels, found_kernel;
    char kernel_name[20];
    cl_uint i, num_kernels;
    
    std::string path = std::string("..//TestOpenCL//ＭapCopy//") + std::string(PROGRAM_FILE);
    program_handle = fopen(path.c_str(), "r");
    if(program_handle == NULL) {
       perror("Couldn't find the program file");
        return -1;
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size+1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);
    
    cl_context context = basic_struct.context;
    cl_int &err = basic_struct.err;
    cl_device_id &device = *basic_struct.devices;
    //size_t num_devices;
    cl_command_queue &queue = basic_struct.queue;
    
    /* Create program from file */
    cl_program program = clCreateProgramWithSource(context, 1,
       (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        perror("Couldn't create the program");
        return -1;
    }
    free(program_buffer);
    
    /* Build program */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if(err < 0) {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
             0, NULL, &log_size);
        program_log = (char*) malloc(log_size+1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
             log_size+1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        return -1;
    }
   
    cl_kernel kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    if(err < 0) {
       perror("Couldn't create a kernel");
        return -1;
    };
    
    // CL_MEM_READ_WRITE    Kernel's authority to memory buffer.
    // CL_MEM_COPY_HOST_PTR Allocate memory and copy data to the position which was host ptr pointer to. (copy by value)
    
    // CL_MEM_ALLOC_HOST_PTR Allocate a pinned memory
    
    // CL_MEM_USE_HOST_PTR
    //maintains a reference to that memory area and depending on the implementation it might access it directly while kernels are executing or it might cache it. You must use mapbuffer to provide synchronization points if you want to write cross platform code using this.
    
    cl_mem buffer_one = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
    if(err <0)
    {
        perror("Couldn't create a buffer object");
        return -1;
    }
    
    
    cl_mem buffer_two = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);
    if(err <0)
    {
        perror("Couldn't create a buffer object");
        return -1;
    }

    /* Set buffers as arguments to the kernel */
    cl_uint arg_idx0 = 0;
    cl_uint arg_idx1 = 1;
    err = clSetKernelArg(kernel, arg_idx0, sizeof(cl_mem), &buffer_one);
    err |= clSetKernelArg(kernel, arg_idx1, sizeof(cl_mem), &buffer_two);
    if(err <0)
    {
        perror("Couldn't set the buffer as the kernel argument");
        return -1;
    }
    /* Create a command queue */
    cl_command_queue_properties properties = 0;
    queue = clCreateCommandQueue(context, device, properties, &err);
    if(err < 0) {
        perror("Couldn't create a command queue");
        return -1;
    };
    /* Enqueue kernel */
    cl_uint num_events_in_wait_list = 0;
    err = clEnqueueTask(queue, kernel, num_events_in_wait_list, NULL, NULL);
    if(err < 0) {
        perror("Couldn't enqueue the kernel");
        exit(1);
    }

     /* Enqueue command to copy buffer one to buffer two */
     err = clEnqueueCopyBuffer(queue, buffer_one, buffer_two, 0, 0,
           sizeof(data_one), num_events_in_wait_list, NULL, NULL);
     if(err < 0) {
        perror("Couldn't perform the buffer copy");
        exit(1);
     }

     /* Enqueue command to map buffer two to host memory */

     mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE,
           CL_MAP_READ, 0, sizeof(data_two), 0, NULL, NULL, &err);
     if(err < 0) {
        perror("Couldn't map the buffer to host memory");
        exit(1);
     }
    
    clReleaseProgram(program);

    return 0;
}






