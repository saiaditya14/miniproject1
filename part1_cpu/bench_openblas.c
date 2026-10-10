#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <openblas/cblas.h>

void student_gemm(const float *A, const float *B, float *C, int M, int K, int N);

static double now(void){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts); return ts.tv_sec+ts.tv_nsec*1e-9; }
static int cmp_d(const void*a,const void*b){ double x=*(const double*)a,y=*(const double*)b; return (x>y)-(x<y); }

#define ITERS 51

static void run(const char*name,int M,int K,int N){
    float *A=malloc((size_t)M*K*4),*B=malloc((size_t)K*N*4);
    float *C1=malloc((size_t)M*N*4),*C2=malloc((size_t)M*N*4);
    srand(7);
    for(size_t i=0;i<(size_t)M*K;i++) A[i]=((rand()%2000)-1000)/1000.0f;
    for(size_t i=0;i<(size_t)K*N;i++) B[i]=((rand()%2000)-1000)/1000.0f;

    double to[ITERS], tb[ITERS];
    for(int it=0; it<ITERS; it++){ double t0=now(); student_gemm(A,B,C1,M,K,N); to[it]=now()-t0; }
    for(int it=0; it<ITERS; it++){
        double t0=now();
        if(N==1) cblas_sgemv(CblasRowMajor,CblasNoTrans,M,K,1.0f,A,K,B,1,0.0f,C2,1);
        else cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,1.0f,A,K,B,N,0.0f,C2,N);
        tb[it]=now()-t0;
    }
    qsort(to,ITERS,sizeof(double),cmp_d);
    qsort(tb,ITERS,sizeof(double),cmp_d);

    double maxdiff=0;
    for(size_t i=0;i<(size_t)M*N;i++){ double d=fabs(C1[i]-C2[i]); if(d>maxdiff) maxdiff=d; }

    double fl=2.0*M*K*N, go=fl/to[ITERS/2]/1e9, gb=fl/tb[ITERS/2]/1e9;
    printf("%-11s M=%-6d K=%-4d N=%-3d ours %8.2f GFLOP/s | OpenBLAS %8.2f GFLOP/s | ratio %.2f | maxdiff %.5f\n",
           name,M,K,N,go,gb,go/gb,maxdiff);
    free(A);free(B);free(C1);free(C2);
}

int main(void){
    int shapes[4][2]={{288,288},{768,288},{288,768},{32000,288}};
    const char*names[4]={"attn-proj","ffn-up","ffn-down","classifier"};
    int ns[4]={1,8,32,64};
    for(int s=0;s<4;s++) for(int n=0;n<4;n++) run(names[s],shapes[s][0],shapes[s][1],ns[n]);
    return 0;
}
