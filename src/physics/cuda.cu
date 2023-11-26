#include "cuda.hpp"

__device__ float4 fetch_utils(CUDABody_t body) {
    return make_float4(body.pos.x, body.pos.y, 1, body.mass);
}

__device__ float2 operator+(const float2 &a, const float2 &b) {
    return make_float2(a.x+b.x, a.y+b.y);
}
__device__ float2 operator+=(const float2 &a, const float2 &b) {
    return make_float2(a.x+b.x, a.y+b.y);
}
__device__ float2 operator*(const float2 &a, const float2 &b) {
    return make_float2(a.x*b.x, a.y*b.y);
}
__device__ float2 operator-(const float2 &a, const float2 &b) {
    return make_float2(a.x-b.x, a.y-b.y);
}
__device__ float2 operator/(const float2 &a, const float2 &b) {
    return make_float2(a.x/b.x, a.y/b.y);
}

__device__ float2 operator*(const float2 &a, const float &b) {
    return make_float2(a.x*b, a.y*b);
}
__device__ float2 operator/(const float2 &a, const float &b) {
    return make_float2(a.x/b, a.y/b);
}
__device__ float2 operator*(const float &a, const float2 &b) {
    return b * a;
}
__device__ float2 operator/(const float &a, const float2 &b) {
    return make_float2(a/b.x, a/b.y);
}

__device__ double norm(const float2 &a) {
    return sqrtf(a.x*a.x+a.y*a.y);
}

__device__ float2 operator-(const float2 &b) {
    return make_float2(-b.x, -b.y);
}




__device__ float2 accumulateAccel(float4 bi, float4 bj, float2 acc, float G) {
    float2 r;
    r.x = bj.x - bi.x;
    r.y = bj.y - bi.y;

    float distp = r.x * r.x + r.y * r.y + CUDA_EPSILON;
    
    float dist = sqrtf(distp);
    
    if (dist < 1.0f)
        return acc;

    float accn = G * bj.w / (dist * dist * dist);

    acc.x += r.x * accn;
    acc.y += r.y * accn; 
    return acc;
}

__global__ void updateBodies(CUDABody_t *bodies, size_t length, float G, float dt) {
    size_t true_idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (true_idx >= length)
        return;
    
    float2 acc = {0, 0};
    for (size_t j = 0; j < length; j++) {
        if (true_idx != j) {
            acc = accumulateAccel(
                fetch_utils(bodies[true_idx]),
                fetch_utils(bodies[j]),
                acc,
                G
            );
        }
    }
    // if (true_idx == -1) {
    //     printf("accel=%f %f\n", acc.x, acc.y);
    //     printf("vel=%f %f\n", bodies[0].vel.x, bodies[0].vel.y);
    //     printf("pos=%f %f\n", bodies[0].pos.x, bodies[0].pos.y);
    // }
    bodies[true_idx].vel.x += acc.x * dt/2;
    bodies[true_idx].vel.y += acc.y * dt/2;

    bodies[true_idx].pos.x += bodies[true_idx].vel.x * dt;
    bodies[true_idx].pos.y += bodies[true_idx].vel.y * dt;

    bodies[true_idx].vel.x += acc.x * dt/2;
    bodies[true_idx].vel.y += acc.y * dt/2;

    // bodies[true_idx].vel.x *= 0.9999;
    // bodies[true_idx].vel.y *= 0.9999;
}

__host__ void walkCUDADevice(CUDABody_t *host_bodies, size_t length, float G, float dt) {
    CUDABody_t *bodies;

    cudaMalloc(&bodies, sizeof(CUDABody_t) * length);
    cudaMemcpy(bodies, host_bodies, sizeof(CUDABody_t) * length, cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (length + threadsPerBlock - 1) / threadsPerBlock;
    updateBodies<<<blocksPerGrid, threadsPerBlock>>>(bodies, length, G, dt);

    cudaMemcpy(host_bodies, bodies, sizeof(CUDABody_t) * length, cudaMemcpyDeviceToHost);
    cudaFree(bodies);
}