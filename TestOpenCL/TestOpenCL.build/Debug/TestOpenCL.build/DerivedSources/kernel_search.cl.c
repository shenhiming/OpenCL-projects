/***** GCL Generated File *********************/
/* Automatically generated file, do not edit! */
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dispatch/dispatch.h>
#include <OpenCL/opencl.h>
#include <OpenCL/gcl_priv.h>
#include "kernel_search.cl.h"

static void initBlocks(void);

// Initialize static data structures
static block_kernel_pair pair_map[4] = {
      { NULL, NULL },
      { NULL, NULL },
      { NULL, NULL },
      { NULL, NULL }
};

static block_kernel_map bmap = { 0, 4, initBlocks, pair_map };

// Block function
void (^add_kernel)(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) =
^(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) {
  int err = 0;
  cl_kernel k = bmap.map[0].kernel;
  if (!k) {
    initBlocks();
    k = bmap.map[0].kernel;
  }
  if (!k)
    gcl_log_fatal("kernel add does not exist for device");
  kargs_struct kargs;
  gclCreateArgsAPPLE(k, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 0, a, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 1, b, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 2, c, &kargs);
  gcl_log_cl_fatal(err, "setting argument for add failed");
  err = gclExecKernelAPPLE(k, ndrange, &kargs);
  gcl_log_cl_fatal(err, "Executing add failed");
  gclDeleteArgsAPPLE(k, &kargs);
};

void (^sub_kernel)(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) =
^(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) {
  int err = 0;
  cl_kernel k = bmap.map[1].kernel;
  if (!k) {
    initBlocks();
    k = bmap.map[1].kernel;
  }
  if (!k)
    gcl_log_fatal("kernel sub does not exist for device");
  kargs_struct kargs;
  gclCreateArgsAPPLE(k, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 0, a, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 1, b, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 2, c, &kargs);
  gcl_log_cl_fatal(err, "setting argument for sub failed");
  err = gclExecKernelAPPLE(k, ndrange, &kargs);
  gcl_log_cl_fatal(err, "Executing sub failed");
  gclDeleteArgsAPPLE(k, &kargs);
};

void (^mult_kernel)(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) =
^(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) {
  int err = 0;
  cl_kernel k = bmap.map[2].kernel;
  if (!k) {
    initBlocks();
    k = bmap.map[2].kernel;
  }
  if (!k)
    gcl_log_fatal("kernel mult does not exist for device");
  kargs_struct kargs;
  gclCreateArgsAPPLE(k, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 0, a, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 1, b, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 2, c, &kargs);
  gcl_log_cl_fatal(err, "setting argument for mult failed");
  err = gclExecKernelAPPLE(k, ndrange, &kargs);
  gcl_log_cl_fatal(err, "Executing mult failed");
  gclDeleteArgsAPPLE(k, &kargs);
};

void (^div_kernel)(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) =
^(const cl_ndrange *ndrange, cl_float* a, cl_float* b, cl_float* c) {
  int err = 0;
  cl_kernel k = bmap.map[3].kernel;
  if (!k) {
    initBlocks();
    k = bmap.map[3].kernel;
  }
  if (!k)
    gcl_log_fatal("kernel div does not exist for device");
  kargs_struct kargs;
  gclCreateArgsAPPLE(k, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 0, a, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 1, b, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 2, c, &kargs);
  gcl_log_cl_fatal(err, "setting argument for div failed");
  err = gclExecKernelAPPLE(k, ndrange, &kargs);
  gcl_log_cl_fatal(err, "Executing div failed");
  gclDeleteArgsAPPLE(k, &kargs);
};

// Initialization functions
static void initBlocks(void) {
  const char* build_opts = "";
  static dispatch_once_t once;
  dispatch_once(&once,
    ^{ int err = gclBuildProgramBinaryAPPLE("OpenCL/kernel_search.cl", "", &bmap, build_opts);
       if (!err) {
          assert(bmap.map[0].block_ptr == add_kernel && "mismatch block");
          bmap.map[0].kernel = clCreateKernel(bmap.program, "add", &err);
          assert(bmap.map[1].block_ptr == sub_kernel && "mismatch block");
          bmap.map[1].kernel = clCreateKernel(bmap.program, "sub", &err);
          assert(bmap.map[2].block_ptr == mult_kernel && "mismatch block");
          bmap.map[2].kernel = clCreateKernel(bmap.program, "mult", &err);
          assert(bmap.map[3].block_ptr == div_kernel && "mismatch block");
          bmap.map[3].kernel = clCreateKernel(bmap.program, "div", &err);
       }
     });
}

__attribute__((constructor))
static void RegisterMap(void) {
  gclRegisterBlockKernelMap(&bmap);
  bmap.map[0].block_ptr = add_kernel;
  bmap.map[1].block_ptr = sub_kernel;
  bmap.map[2].block_ptr = mult_kernel;
  bmap.map[3].block_ptr = div_kernel;
}

