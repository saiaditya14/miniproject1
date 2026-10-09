#define main llama_main
#include "run.c"
#undef main

#include <time.h>
#include "student_gemm.h"

static double now_s(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec*1e-9;
}

static double g_gemm = 0.0, g_samp = 0.0;
static void tgemm(const float *A, const float *Bm, float *C, int M, int K, int N){
    double t0 = now_s();
    student_gemm(A, Bm, C, M, K, N);
    g_gemm += now_s() - t0;
}

static void gather(float *X, float **vecs, int n, int B){
    for(int b=0; b<B; b++){
        const float *v = vecs[b];
        for(int k=0; k<n; k++) X[(size_t)k*B + b] = v[k];
    }
}

static void forward_batch(Transformer *t, RunState *S, const int *tok, const int *pos, int B,
                          float *X, float *Y, float *H, float **ptr){
    Config *p = &t->config;
    TransformerWeights *w = &t->weights;
    int dim = p->dim;
    int kv_dim = (dim * p->n_kv_heads) / p->n_heads;
    int kv_mul = p->n_heads / p->n_kv_heads;
    int hidden = p->hidden_dim;
    int head_size = dim / p->n_heads;

    for(int b=0; b<B; b++)
        memcpy(S[b].x, w->token_embedding_table + (size_t)tok[b]*dim, dim*sizeof(float));

    for(int l=0; l<p->n_layers; l++){
        int loff = l * p->seq_len * kv_dim;

        for(int b=0; b<B; b++){
            rmsnorm(S[b].xb, S[b].x, w->rms_att_weight + (size_t)l*dim, dim);
            ptr[b] = S[b].xb;
        }
        gather(X, ptr, dim, B);

        tgemm(w->wq + (size_t)l*dim*dim, X, Y, dim, dim, B);
        for(int b=0; b<B; b++)
            for(int i=0; i<dim; i++) S[b].q[i] = Y[(size_t)i*B + b];

        tgemm(w->wk + (size_t)l*dim*kv_dim, X, Y, kv_dim, dim, B);
        for(int b=0; b<B; b++){
            float *k = S[b].key_cache + loff + (size_t)pos[b]*kv_dim;
            for(int i=0; i<kv_dim; i++) k[i] = Y[(size_t)i*B + b];
        }

        tgemm(w->wv + (size_t)l*dim*kv_dim, X, Y, kv_dim, dim, B);
        for(int b=0; b<B; b++){
            float *v = S[b].value_cache + loff + (size_t)pos[b]*kv_dim;
            for(int i=0; i<kv_dim; i++) v[i] = Y[(size_t)i*B + b];
        }

        for(int b=0; b<B; b++){
            RunState *s = &S[b];
            int ps = pos[b];
            float *kcur = s->key_cache + loff + (size_t)ps*kv_dim;

            for(int i=0; i<dim; i+=2){
                int head_dim = i % head_size;
                float freq = 1.0f / powf(10000.0f, head_dim / (float)head_size);
                float val = ps * freq;
                float fcr = cosf(val);
                float fci = sinf(val);
                int rotn = i < kv_dim ? 2 : 1;
                for(int v=0; v<rotn; v++){
                    float *vec = v == 0 ? s->q : kcur;
                    float v0 = vec[i];
                    float v1 = vec[i+1];
                    vec[i]   = v0*fcr - v1*fci;
                    vec[i+1] = v0*fci + v1*fcr;
                }
            }

            for(int h=0; h<p->n_heads; h++){
                float *q = s->q + h*head_size;
                float *att = s->att + h*p->seq_len;
                for(int tt=0; tt<=ps; tt++){
                    float *k = s->key_cache + loff + (size_t)tt*kv_dim + (h/kv_mul)*head_size;
                    float score = 0.0f;
                    for(int i=0; i<head_size; i++) score += q[i]*k[i];
                    att[tt] = score / sqrtf(head_size);
                }
                softmax(att, ps+1);
                float *xb = s->xb + h*head_size;
                memset(xb, 0, head_size*sizeof(float));
                for(int tt=0; tt<=ps; tt++){
                    float *v = s->value_cache + loff + (size_t)tt*kv_dim + (h/kv_mul)*head_size;
                    float a = att[tt];
                    for(int i=0; i<head_size; i++) xb[i] += a*v[i];
                }
            }
            ptr[b] = s->xb;
        }
        gather(X, ptr, dim, B);

        tgemm(w->wo + (size_t)l*dim*dim, X, Y, dim, dim, B);
        for(int b=0; b<B; b++)
            for(int i=0; i<dim; i++) S[b].x[i] += Y[(size_t)i*B + b];

        for(int b=0; b<B; b++){
            rmsnorm(S[b].xb, S[b].x, w->rms_ffn_weight + (size_t)l*dim, dim);
            ptr[b] = S[b].xb;
        }
        gather(X, ptr, dim, B);

        tgemm(w->w1 + (size_t)l*dim*hidden, X, Y, hidden, dim, B);
        tgemm(w->w3 + (size_t)l*dim*hidden, X, H, hidden, dim, B);
        for(size_t i=0; i<(size_t)hidden*B; i++){
            float val = Y[i];
            val *= (1.0f / (1.0f + expf(-val)));
            Y[i] = val * H[i];
        }
        tgemm(w->w2 + (size_t)l*dim*hidden, Y, H, dim, hidden, B);
        for(int b=0; b<B; b++)
            for(int i=0; i<dim; i++) S[b].x[i] += H[(size_t)i*B + b];
    }

    for(int b=0; b<B; b++){
        rmsnorm(S[b].x, S[b].x, w->rms_final_weight, dim);
        ptr[b] = S[b].x;
    }
    gather(X, ptr, dim, B);
    tgemm(w->wcls, X, Y, p->vocab_size, dim, B);
    for(int b=0; b<B; b++)
        for(int i=0; i<p->vocab_size; i++) S[b].logits[i] = Y[(size_t)i*B + b];
}

int main(int argc, char **argv){
    if(argc < 4){ fprintf(stderr, "usage: %s model.bin B steps [check|show]\n", argv[0]); return 1; }
    char *ckpt = argv[1];
    int B = atoi(argv[2]);
    int steps = atoi(argv[3]);
    int check = argc > 4 && argv[4][0] == 'c';
    int show = argc > 4 && argv[4][0] == 's';

    Transformer t;
    build_transformer(&t, ckpt);
    Config *p = &t.config;
    if(steps > p->seq_len || steps <= 0) steps = p->seq_len;
    Tokenizer tk;
    build_tokenizer(&tk, "tokenizer.bin", p->vocab_size);

    RunState *S = (RunState *)calloc(B, sizeof(RunState));
    for(int b=0; b<B; b++) malloc_run_state(&S[b], p);
    size_t ymax = (size_t)(p->vocab_size > p->hidden_dim ? p->vocab_size : p->hidden_dim) * B;
    float *X = (float *)malloc((size_t)p->dim*B*sizeof(float));
    float *Y = (float *)malloc(ymax*sizeof(float));
    float *H = (float *)malloc((size_t)p->hidden_dim*B*sizeof(float));
    float **ptr = (float **)malloc(B*sizeof(float *));

    if(check){
        RunState *R = (RunState *)calloc(B, sizeof(RunState));
        for(int b=0; b<B; b++) malloc_run_state(&R[b], p);
        double worst = 0.0; int argmax_diff = 0, total = 0;
        int *tokv = (int *)malloc(B*sizeof(int)), *posv = (int *)malloc(B*sizeof(int));
        for(int b=0; b<B; b++) tokv[b] = (b*37 + 5) % p->vocab_size;
        for(int step=0; step<16; step++){
            for(int b=0; b<B; b++) posv[b] = step;
            forward_batch(&t, S, tokv, posv, B, X, Y, H, ptr);
            for(int b=0; b<B; b++){
                t.state = R[b];
                float *ref = forward(&t, tokv[b], step);
                float *got = S[b].logits;
                int ra = 0, ga = 0;
                for(int i=0; i<p->vocab_size; i++){
                    double d = fabs((double)ref[i] - got[i]);
                    if(d > worst) worst = d;
                    if(ref[i] > ref[ra]) ra = i;
                    if(got[i] > got[ga]) ga = i;
                }
                total++; if(ra != ga) argmax_diff++;
                tokv[b] = ra;
            }
        }
        printf("check B=%d: worst |logit diff| vs stock forward = %.3e, argmax mismatches %d/%d\n",
               B, worst, argmax_diff, total);
        return 0;
    }

    int pt[64], npt = 0;
    encode(&tk, "Once upon a time", 1, 0, pt, &npt);
    Sampler *sm = (Sampler *)calloc(B, sizeof(Sampler));
    for(int b=0; b<B; b++) build_sampler(&sm[b], p->vocab_size, 0.9f, 1.0f, 1234ULL + b);

    int *token = (int *)malloc(B*sizeof(int)), *posv = (int *)malloc(B*sizeof(int));
    for(int b=0; b<B; b++) token[b] = pt[0];

    double t0 = 0.0;
    int ended = 0;
    for(int pos=0; pos<steps; pos++){
        for(int b=0; b<B; b++) posv[b] = pos;
        forward_batch(&t, S, token, posv, B, X, Y, H, ptr);
        for(int b=0; b<B; b++){
            double ts = now_s();
            int next = pos < npt-1 ? pt[pos+1] : sample(&sm[b], S[b].logits);
            g_samp += now_s() - ts;
            if(show && b == 0 && !ended){
                if(next == 1) ended = 1;
                else safe_printf(decode(&tk, token[b], next));
            }
            token[b] = next;
        }
        if(pos == 0) t0 = now_s();
    }
    double dt = now_s() - t0;
    if(show) printf("\n");
    double toks = (double)B*(steps-1);
    printf("N=%-3d steps=%d  aggregate %8.1f tok/s   per-stream %7.1f tok/s   gemm %4.1f%%  sampling %4.1f%%  other %4.1f%%\n", B, steps, toks/dt, (steps-1)/dt, 100.0*g_gemm/dt, 100.0*g_samp/dt, 100.0*(1.0-(g_gemm+g_samp)/dt));
    return 0;
}
