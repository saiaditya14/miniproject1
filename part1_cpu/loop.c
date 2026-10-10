#include <stdio.h>
#include <immintrin.h>

#define M 16
#define K 16
#define N 16

float A[M][K], B[K][N], C[M][N];

void lmao(){
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            for(int k=0;k<K;k++){
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}

void nulmao(){
    for(int i=0;i<M;i++){
        for(int k=0;k<K;k++){
            for(int j=0;j<N;j++){
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}

#define tile 3
#define min(a,b) ((a)<(b)?(a):(b))

void tiled(){
    for(int ti=0;ti<M;ti+=tile){
        for(int tk = 0;tk<K;tk+=tile){
            for(int tj=0;tj<N;tj+=tile){
                for(int i=ti;i<min(M,ti+tile);i++){
                    for(int k=tk;k<min(K,tk+tile);k++){
                        for(int j=tj;j<min(N,tj+tile);j++){
                            C[i][j] += A[i][k]*B[k][j];
                        }
                    }
                }
            }
        }
    }
}

float ap[tile][tile];
float bp[tile][tile];
float cp[tile][tile];

void packed(){
    for(int ti=0;ti<M;ti+=tile){
        int mr = min(M,ti+tile)-ti;
        for(int tj=0;tj<N;tj+=tile){
            int nr = min(N,tj+tile)-tj;

            for(int ii=0;ii<mr;ii++)
                for(int jj=0;jj<nr;jj++)
                    cp[ii][jj] = 0;

            for(int tk=0;tk<K;tk+=tile){
                int kr = min(K,tk+tile)-tk;

                for(int ii=0;ii<mr;ii++)
                    for(int kk=0;kk<kr;kk++)
                        ap[ii][kk] = A[ti+ii][tk+kk];

                for(int kk=0;kk<kr;kk++)
                    for(int jj=0;jj<nr;jj++)
                        bp[kk][jj] = B[tk+kk][tj+jj];

                for(int ii=0;ii<mr;ii++)
                    for(int kk=0;kk<kr;kk++)
                        for(int jj=0;jj<nr;jj++)
                            cp[ii][jj] += ap[ii][kk]*bp[kk][jj];
            }

            for(int ii=0;ii<mr;ii++)
                for(int jj=0;jj<nr;jj++)
                    C[ti+ii][tj+jj] += cp[ii][jj];
        }
    }
}

typedef struct { float p00,p01,p10,p11; } prod4;
//helps to remove "call" instruction in assembly
static inline prod4 micro2x2(int i0, int j0, int k){
    float a0 = A[i0][k],   a1 = A[i0+1][k];
    float b0 = B[k][j0],   b1 = B[k][j0+1];
    prod4 p = { a0*b0, a0*b1, a1*b0, a1*b1 };
    return p;
}

void micro2x2_tile(int i0, int j0){
    float c00=0, c01=0, c10=0, c11=0;
    for(int k=0;k<K;k++){
        prod4 p = micro2x2(i0,j0,k);
        c00 += p.p00;  c01 += p.p01;
        c10 += p.p10;  c11 += p.p11;
    }
    C[i0][j0]   += c00;  C[i0][j0+1]   += c01;
    C[i0+1][j0] += c10;  C[i0+1][j0+1] += c11;
}

//
//
void micro1x8(int i0, int j0){
    __m256 acc = _mm256_setzero_ps();
    __m256 acc1 = _mm256_setzero_ps();
    for(int k=0;k<K;k++){
        __m256 va  = _mm256_set1_ps(A[i0][k]);
        __m256 va2 = _mm256_set1_ps(A[i0+1][k]);
        //can add  more to make it 8x8 later
        __m256 vb  = _mm256_loadu_ps(&B[k][j0]);
        acc = _mm256_fmadd_ps(va, vb, acc);
        acc1 = _mm256_fmadd_ps(va2, vb, acc1);
        //and so on
    }
    _mm256_storeu_ps(&C[i0][j0],   acc);
    _mm256_storeu_ps(&C[i0+1][j0], acc1);
}



void fillAB(){
    for(int i=0;i<M;i++)
        for(int k=0;k<K;k++)
            A[i][k] = i*K+k;
    for(int k=0;k<K;k++)
        for(int j=0;j<N;j++)
            B[k][j] = k*N+j;
}

void zeroC(){
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
            C[i][j] = 0;
}

void printC(){
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++)
            printf("%.1f ", C[i][j]);
        printf("\n");
    }
}

int main(){
    fillAB();

    zeroC();
    lmao();
    printf("lmao:\n");
    printC();

    zeroC();
    nulmao();
    printf("nulmao:\n");
    printC();

    zeroC();
    tiled();
    printf("tiled:\n");
    printC();

    zeroC();
    packed();
    printf("packed:\n");
    printC();

    zeroC();
    micro2x2_tile(0,0);
    printf("micro2x2_tile(0,0) -- only top-left 2x2 corner is meaningful:\n");
    printC();

    zeroC();
    micro1x8(0,0);
    printf("micro1x8(0,0) -- only rows 0,1 cols 0..7 are meaningful:\n");
    printC();

    return 0;
}

