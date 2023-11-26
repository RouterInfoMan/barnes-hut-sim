#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>

#define CUDA_EPSILON 0.1f 

#define CUDA_THREADS_PER_BLOCK 1024
#define CUDA_MAX_BLOCKS 65535

// Structure used to handle the host - device communication
typedef struct CUDABody {
    float2 pos;
    float2 vel;
    float mass;
} CUDABody_t;

typedef struct CUDABarnesHutNode {


    struct CUDABarnesHutNode *tl, *tr, *bl, *br;
} CUDABarnesHutNode_t;

__host__ void walkCUDADevice(CUDABody_t *host_bodies, size_t length, float G, float dt);



