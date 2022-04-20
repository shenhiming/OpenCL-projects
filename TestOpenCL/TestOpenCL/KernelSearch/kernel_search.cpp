//
//  kernel_serach.cpp
//  TestOpenCL
//
//  Created by ＰＭＳＤ on 2021/3/18.
//  Copyright © 2021 ＰＭＳＤ. All rights reserved.
//
/*
    
OpenCL memory object 之選擇傳輸path
對應用程序來說，選擇合適的memory object傳輸path可以有效提高程序性能。

 

下面先看一寫buffer bandwidth的例子：

 

1.  clEnqueueWriteBuffer()以及clEnqueueReadBuffer()

 

      如果應用程序已經通過malloc 或者mmap分配內存，CL_MEM_USE_HOST_PTR是個理想的選擇。

有兩種使用這種方式的方法：

 

第一種：

a. pinnedBuffer = clCreateBuffer( CL_MEM_ALLOC_HOST_PTR or CL_MEM_USE_HOST_PTR )
b. deviceBuffer = clCreateBuffer()
c. void *pinnedMemory = clEnqueueMapBuffer( pinnedBuffer )
d. clEnqueueRead/WriteBuffer( deviceBuffer, pinnedMemory )
e. clEnqueueUnmapMemObject( pinnedBuffer, pinnedMemory )

 

     pinning開銷在步驟a產生，步驟d沒有任何pinning開銷。通常應用立即程序執行a,b,c,e步驟，而在步驟d之後，要反覆讀和修改pinnedMemory中的數據，

 

 

第二種：

   clEnqueueRead/WriteBuffer 直接在用戶的memory buffer中被使用。在copy(host->device)數據前，首先需要pin(lock page)操作，然後才能執行傳輸操作。這條path大概是peak interconnect bandwidth的2/3。


2. 在pre-pinned host buffer上使用clEnqueueCopyBuffer()

 

    和1類似，clEnqueueCopyBuffer在pre-pinned buffer上以peak interconnect bandwidth執行傳輸操作：

 

a. pinnedBuffer = clCreateBuffer( CL_MEM_ALLOC_HOST_PTR or CL_MEM_USE_HOST_PTR )
b. deviceBuffer = clCreateBuffer()
c. void *memory = clEnqueueMapBuffer( pinnedBuffer )
d. Application writes or modifies memory.
e. clEnqueueUnmapMemObject( pinnedBuffer, memory )
f. clEnqueueCopyBuffer( pinnedBuffer, deviceBuffer )
或者通過:
g. clEnqueueCopyBuffer( deviceBuffer, pinnedBuffer )
h. void *memory = clEnqueueMapBuffer( pinnedBuffer )
i. Application reads memory.
j. clEnqueueUnmapMemObject( pinnedBuffer, memory )

 

   

     由於pinned memory駐留在host memroy，所以clMap() 以及 clUnmap()調用不會導致數據傳輸。cpu可以以host memory帶寬來操作這些pinned buffer。

 
3、在device buffer上執行 clEnqueueMapBuffer() and clEnqueueUnmapMemObject()

 

     對於已經通過malloc和mmap分配空間的buffer，傳輸開銷除了interconnect傳輸外，還要包括一個memcpy過程，該過程把buffer拷貝進mapped device buffer。


a. Data transfer from host to device buffer.


1.

ptr = clEnqueueMapBuffer( .., buf, .., CL_MAP_WRITE, .. )
     由於緩沖被映射為write-only,所以沒有數據從device傳輸到host，映射開銷比較低。一個指向pinned host buffer的指針被返回。

 

 

2. 應用程序通過memset(ptr)填充host buffer
    memcpy ( ptr, srcptr ), fread( ptr ), 或者直接CPU寫， 這些操作以host memory全速帶寬讀寫。


3. clEnqueueUnmapMemObject( .., buf, ptr, .. )
    pre-pinned buffer以peak interconnect速度被傳輸到GPU device。

 
b. Data transfer from device buffer to host.


1. ptr = clEnqueueMapBuffer(.., buf, .., CL_MAP_READ, .. )
    這個命令啟動devcie到host數據傳輸，數據以peak interconnect bandwidth傳輸到一個pre-pinned的臨時緩沖中。返回一個指向pinned memory的指針。
2. 應用程序讀、處理數據或者執行 memcpy( dstptr, ptr ), fwrite (ptr), 或者其它類似的函數時候，由於buffer駐留在host memory中，所以操作以host memory bandwidth執行。

3. clEnqueueUnmapMemObject( .., buf, ptr, .. )

由於buffer被映射成只讀的，沒有實際數據傳輸，所以unmap操作的cost很低。


4. host直接訪問設備zero copy buffer

   這個訪問允許數據傳輸和GPU計算同時執行（overlapped），在一些稀疏(sparse)的寫或者更新情況下，比較有用。

a. 一個device上的 zero copy buffer通過下面的命令被創建：


buf = clCreateBuffer ( .., CL_MEM_USE_PERSISTENT_MEM_AMD, .. )


CPU能夠通過uncached WC path直接訪問該buffer。 通常可以使用雙緩沖機制，gpu在處理一個緩沖中的數據，cpu同時在填充另一個緩沖中的數據。

A zero copy device buffer can also be used to for sparse updates, such as assembling sub-rows of a larger matrix into a smaller, contiguous block for GPU processing. Due to the WC path, it is a good design choice to try to align writes to the cache line size, and to pick the write block size as large as possible.

 

b. Transfer from the host to the device.
1. ptr = clEnqueueMapBuffer( .., buf, .., CL_MAP_WRITE, .. )
This operation is low cost because the zero copy device buffer is directly mapped into the host address space.


2. The application transfers data via memset( ptr ), memcpy( ptr, srcptr ), or direct CPU writes.
The CPU writes directly across the interconnect into the zero copy device buffer. Depending on the chipset, the bandwidth can be of the same order of magnitude as the interconnect bandwidth, although it typically is lower than peak.


3. clEnqueueUnmapMemObject( .., buf, ptr, .. )
As with the preceding map, this operation is low cost because the buffer continues to reside on the device.
c. If the buffer content must be read back later, use clEnqueueReadBuffer( .., buf, ..)  or clEnqueueCopyBuffer( .., buf, zero copy host buffer, .. ).


This bypasses slow host reads through the uncached path.


5 - GPU直接訪問host zero copy memory

 

This option allows direct reads or writes of host memory by the GPU. A GPU kernel can import data from the host without explicit transfer, and write data directly back to host memory. An ideal use is to perform small I/Os straight from the kernel, or to integrate the transfer latency directly into the kernel execution time.


a：The application creates a zero copy host buffer.
     buf = clCreateBuffer( .., CL_MEM_ALLOC_HOST_PTR, .. )
b：Next, the application modifies or reads the zero copy host buffer.


     1. ptr = clEnqueueMapBuffer( .., buf, .., CL_MAP_READ | CL_MAP_WRITE, .. )

This operation is very low cost because it is a map of a buffer already residing in host memory.
      2. The application modifies the data through memset( ptr ), memcpy( in either direction ), sparse or dense CPU reads or writes. Since the application is modifying a host buffer, these operations take place at host memory bandwidth.
      3. clEnqueueUnmapMemObject( .., buf, ptr, .. )

As with the preceding map, this operation is very low cost because the buffer continues to reside in host memory.
c. The application runs clEnqueueNDRangeKernel(), using buffers of this type as input or output. GPU kernel reads and writes go across the interconnect to host memory, and the data transfer becomes part of the
kernel execution.


The achievable bandwidth depends on the platform and chipset, but can be of the same order of magnitude as the peak interconnect bandwidth.


For discrete graphics cards, it is important to note that resulting GPU kernel bandwidth is an order of magnitude lower compared to a kernel accessing a regular device buffer located on the device.


d. Following kernel execution, the application can access data in the host buffer in the same manner as described above.
 
*/

 

#include "kernel_search.h"
#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "kernel_search.cl"


int KernelSearch(cl_basic_struct &basic_struct)
{
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    cl_kernel *kernels, found_kernel;
    char kernel_name[20];
    cl_uint i, num_kernels;
    
    std::string path = std::string("..//TestOpenCL//KernelSearch//") + std::string(PROGRAM_FILE);
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
    //cl_command_queue queue;
    
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
    /* Find out how many kernels are in the source file */
    err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);
    if(err < 0) {
        perror("Couldn't find any kernels");
        return -1;
    }

    /* Create a kernel for each function */
    kernels = (cl_kernel*) malloc(num_kernels * sizeof(cl_kernel));
    clCreateKernelsInProgram(program, num_kernels, kernels, NULL);

    /* Search for the named kernel */
    for(i=0; i<num_kernels; i++) {
       clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME,
             sizeof(kernel_name), kernel_name, NULL);
       //if(strcmp(kernel_name, "mult") == 0) {
          //found_kernel = kernels[i];
          //printf("Found mult kernel at index %u.\n", i);
          //break;
       //}
        printf("%dth kernel name = %s\n", i, kernel_name);
    }

    for(i=0; i<num_kernels; i++)
       clReleaseKernel(kernels[i]);
    free(kernels);
    clReleaseProgram(program);

    return 0;
}






