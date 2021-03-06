#include "haar3D.h"

struct mem
{
    uint64_t val;
    uint64_t ord;
};

bool compari(const mem &A, const mem &B) {return A.val < B.val;}

void copyAsort(double *VX, double *CX, size_t K, size_t N, int64_t *C, uint64_t *W, uint64_t *val, uint64_t *ord)
{
    mem			*MEM = (mem *) malloc( N*sizeof(mem) );
    double		*VY = VX+N, *VZ = VY+N;
    //double		*CY = CX+N, *CZ = CY+N;
    uint64_t	vx, vy, vz;

    for(size_t i=0; i<N; i++)
    {
        vx = VZ[i];
        vy = VY[i];
        vz = VX[i];

        MEM[i].val =
                ((0x000001 & vx)    ) + ((0x000001 & vy)<< 1) + ((0x000001 & vz)<< 2) +
                ((0x000002 & vx)<< 2) + ((0x000002 & vy)<< 3) + ((0x000002 & vz)<< 4) +
                ((0x000004 & vx)<< 4) + ((0x000004 & vy)<< 5) + ((0x000004 & vz)<< 6) +
                ((0x000008 & vx)<< 6) + ((0x000008 & vy)<< 7) + ((0x000008 & vz)<< 8) +
                ((0x000010 & vx)<< 8) + ((0x000010 & vy)<< 9) + ((0x000010 & vz)<<10) +
                ((0x000020 & vx)<<10) + ((0x000020 & vy)<<11) + ((0x000020 & vz)<<12) +
                ((0x000040 & vx)<<12) + ((0x000040 & vy)<<13) + ((0x000040 & vz)<<14) +
                ((0x000080 & vx)<<14) + ((0x000080 & vy)<<15) + ((0x000080 & vz)<<16) +
                ((0x000100 & vx)<<16) + ((0x000100 & vy)<<17) + ((0x000100 & vz)<<18) +
                ((0x000200 & vx)<<18) + ((0x000200 & vy)<<19) + ((0x000200 & vz)<<20) +
                ((0x000400 & vx)<<20) + ((0x000400 & vy)<<21) + ((0x000400 & vz)<<22) +
                ((0x000800 & vx)<<22) + ((0x000800 & vy)<<23) + ((0x000800 & vz)<<24) +
                ((0x001000 & vx)<<24) + ((0x001000 & vy)<<25) + ((0x001000 & vz)<<26) +
                ((0x002000 & vx)<<26) + ((0x002000 & vy)<<27) + ((0x002000 & vz)<<28) +
                ((0x004000 & vx)<<28) + ((0x004000 & vy)<<29) + ((0x004000 & vz)<<30) +
                ((0x008000 & vx)<<30) + ((0x008000 & vy)<<31) + ((0x008000 & vz)<<32) +
                ((0x010000 & vx)<<32) + ((0x010000 & vy)<<33) + ((0x010000 & vz)<<34) +
                ((0x020000 & vx)<<34) + ((0x020000 & vy)<<35) + ((0x020000 & vz)<<36) +
                ((0x040000 & vx)<<36) + ((0x040000 & vy)<<37) + ((0x040000 & vz)<<38) +
                ((0x080000 & vx)<<38) + ((0x080000 & vy)<<39) + ((0x080000 & vz)<<40) +
                ((0x100000 & vx)<<40) + ((0x100000 & vy)<<41) + ((0x100000 & vz)<<42);
        MEM[i].ord = i;

        for(size_t k=0; k<K; k++)
            *(C++) = CX[i+k*N]*(0x1<<INTEGER_TRANSFORM_PRECISION);

        W[i] = 1;
    }
    //

    std::sort(MEM, MEM+N, compari);

    for(size_t i=0; i<N; i++)
    {
        val[i] = MEM[i].val;
        ord[i] = MEM[i].ord;
    }

    free(MEM);
}

int64_t roundIF(int64_t X)
{
    if( X>=0 )
    {
        X >>= INTEGER_TRANSFORM_PRECISION-1;
        if( X&0x1 )
            return (X>>1)+1;
        return X>>1;
    }
    else
    {
        X = (-X)>>(INTEGER_TRANSFORM_PRECISION-1);
        if( X&0x1 )
            return -(X>>1)-1;
        return -(X>>1);
    }
}

void transform(int64_t Qstep, uint64_t w0, uint64_t w1, int64_t *C0, int64_t *C1, int64_t *CT0, int64_t *CT1, size_t K)
{
    int64_t  b = (w1<<INTEGER_TRANSFORM_PRECISION)/(w0+w1);
    Qstep = sqrtIF(Qstep, w0, w1);

    while( K-- )
    {
        *CT1 = (*C1) - (*C0);
        *CT0 = (*C0) + (b*(*CT1))/(0x1<<INTEGER_TRANSFORM_PRECISION);

#if INVERSE_SQUARE_ROOT
        *CT1 = ((*CT1)*Qstep)/(0x1<<INTEGER_STEPSIZE_PRECISION);
#else
        *CT1 = ((*CT1)*(0x1<<INTEGER_STEPSIZE_PRECISION))/Qstep;
#endif
        *CT1 = roundIF(*CT1);

        C0++;
        C1++;
        CT0++;
        CT1++;
    }
}

void itransform(int64_t Qstep, uint64_t w0, uint64_t w1, int64_t *C0, int64_t *C1, int64_t *CT0, int64_t *CT1, size_t K)
{
    int64_t  b = (w1<<INTEGER_TRANSFORM_PRECISION)/(w0+w1);
    Qstep = sqrtIF(Qstep, w0, w1);

    while( K-- )
    {
#if INVERSE_SQUARE_ROOT
        *C0 = (*CT0) - (((b*(*CT1))/(0x1<<INTEGER_TRANSFORM_PRECISION))*(0x1<<INTEGER_STEPSIZE_PRECISION))/Qstep;
        *C1 = ((*CT1)*(0x1<<INTEGER_STEPSIZE_PRECISION))/Qstep + (*C0);
#else
        *C0 = (*CT0) - (((b*(*CT1))/(0x1<<INTEGER_TRANSFORM_PRECISION))*Qstep)/(0x1<<INTEGER_STEPSIZE_PRECISION);
        *C1 = ((*CT1)*Qstep)/(0x1<<INTEGER_STEPSIZE_PRECISION) + (*C0);
#endif

        C0++;
        C1++;
        CT0++;
        CT1++;
    }
}

void copyFromMEM(uint64_t *IN_VAL, uint64_t *IN_W, uint64_t *OUT_VAL, uint64_t *OUT_W, uint64_t M)
{
    while(M)
    {
        M--;
        IN_VAL[M] = OUT_VAL[M];
        IN_W[M]   = OUT_W[M];
    }
}

/* uint64	N = mxGetM(prhs[0]);
 * uint64	depth = *mxGetPr(prhs[2])
 * double	*inV = mxGetPr(prhs[0]);
 * double	*inC = mxGetPr(prhs[1]);
 * plhs[0] = mxCreateDoubleMatrix(NN, 3, mxREAL);
 * double	*outCT = mxGetPr(plhs[0])
 */
void haar3D(int64_t Qstep, double *inV, double *inC, size_t K, size_t N, size_t depth, intmax_t *outCT)
{
    size_t	NN=N;
    size_t	M=N, S, d, i, j;
    depth *= 3;

    int64_t		*C	  = (int64_t   *) malloc( N*K*sizeof(int64_t) );
    int64_t		*CT   = (int64_t   *) malloc( N*K*sizeof(int64_t) );
    uint64_t	*w	  = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*wT   = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*val  = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*valT = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*TMP  = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    int64_t		*TPV;

    copyAsort(inV, inC, K, N, CT, w, val, TMP);
    for(i=0; i<N; i++)
        for(size_t k=0; k<K; k++)
            C[i*K+k] = CT[TMP[i]*K+k];
    free(TMP);

    for(d=0; d<depth; d++)
    {
        i = 0;
        M = 0;
        S = N;

        while( i<S )
        {
            j = i+1;
            valT[M] = val[i] >> 1;

            if( j<S && ((val[i]&0xFFFFFFFFFFFFFFFE)==(val[j]&0xFFFFFFFFFFFFFFFE)) )
            {
                N--;

                wT[M] = w[i]+w[j];
                wT[N] = wT[M];

                transform(Qstep, w[i], w[j], C+i*K, C+j*K, CT+M*K, CT+N*K, K);

                i += 2;
            }
            else
            {
                wT[M] = w[i];
                for(size_t k=0; k<K; k++)
                    CT[M*K+k] = C[i*K+k];

                i += 1;
            }
            M++;
        }
        for(i=N; i<S; i++)
        {
            for(size_t k=0; k<K; k++)
                C[i*K+k] = CT[i*K+k];
            w[i] = wT[i];
        }

        TPV  = CT;
        CT   = C;
        C    = TPV;

        TMP  = valT;
        valT = val;
        val  = TMP;

        TMP  = wT;
        wT   = w;
        w    = TMP;
    }

    free(wT);
    free(val);
    free(valT);

    // Quantization of DC coefficients
    Qstep = sqrtIF(Qstep, w[0]);
    for(size_t k=0; k<K; k++)
#if INVERSE_SQUARE_ROOT
        C[k] = roundIF((C[k]*Qstep)/(0x1<<INTEGER_STEPSIZE_PRECISION));
#else
        C[k] = roundIF((C[k]*(0x1<<INTEGER_STEPSIZE_PRECISION))/Qstep);
#endif

    for(i=0; i<NN; i++)
        for(size_t k=0; k<K; k++)
            outCT[i+k*NN] = C[i*K+k];

    free(w);
    free(C);
    free(CT);
}

/* uint64	N = mxGetM(prhs[0])
 * uint64	depth = *mxGetPr(prhs[2]);
 * double	*inV = mxGetPr(prhs[0]);
 * double	*inCT = mxGetPr(prhs[1]);
 * plhs[0] = mxCreateDoubleMatrix(NN, 3, mxREAL);
 * double	*outC = mxGetPr(plhs[0])
 */
void inv_haar3D(int64_t Qstep, double *inV, double *inCT, size_t K, size_t N, size_t depth, double *outC)
{
    size_t	NN=N;
    size_t	M=N, S, d, i, j;
    depth *= 3;

    int64_t		*C	  = (int64_t   *) malloc( N*K*sizeof(int64_t) );
    int64_t		*CT   = (int64_t   *) malloc( N*K*sizeof(int64_t) );
    uint64_t	*w	  = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*wT   = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*val  = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*valT = (uint64_t  *) malloc( N*sizeof(uint64_t) );
    uint64_t	*ord  = (uint64_t  *) malloc( N*sizeof(uint64_t) );

    uint64_t	**VAL = (uint64_t **) malloc( depth*sizeof(uint64_t *) );
    uint64_t	**iW  = (uint64_t **) malloc( depth*sizeof(uint64_t *) );
    size_t		*iM   = (size_t    *) malloc( depth*sizeof(size_t) );

    uint64_t  *TMP;

    copyAsort(inV, inCT, K, N, CT, w, val, ord);

    // Dequantization of DC coefficients
    {
        int64_t Qstep2 = sqrtIF(Qstep, N);
        for(size_t k=0; k<K; k++)
#if INVERSE_SQUARE_ROOT
            CT[k] = (CT[k]*(0x1<<INTEGER_STEPSIZE_PRECISION))/Qstep2;
#else
            CT[k] = (CT[k]*Qstep2)/(0x1<<INTEGER_STEPSIZE_PRECISION);
#endif
    }

    // memcpy?
    for(i=0; i<N; i++)
        for(size_t k=0; k<K; k++)
            C[i*K+k] = CT[i*K+k];

    // Transformada direta (partes)
    // executa a mesma ordem de passos da transformada direta, apenas armazenando
    // os valores do pesos e do vetor de valor. Estes valores são armazenados para
    // executar os passos em ordem inversa na transformada inversa
    for(d=0; d<depth; d++)
    {
        VAL[d] = (uint64_t *) malloc( M*sizeof(uint64_t) );
        iW[d]  = (uint64_t *) malloc( M*sizeof(uint64_t) );

        copyFromMEM(VAL[d], iW[d], val, w, M);
        iM[d] = M;

        i = 0;
        S = M;
        M = 0;

        while( i<S )
        {
            valT[M] = val[i] >> 1;
            j = i+1;

            if( j<S && ((val[i]&0xFFFFFFFFFFFFFFFE)==(val[j]&0xFFFFFFFFFFFFFFFE)) )
            {
                wT[M] = w[i]+w[j];
                i += 2;
            }
            else
            {
                wT[M] = w[i];
                i += 1;
            }
            M++;
        }

        TMP  = valT;
        valT = val;
        val  = TMP;

        TMP = wT;
        wT  = w;
        w   = TMP;
    }

    // Transformada inversa
    while( d )
    {
        d--;
        S = iM[d];
        M = d?iM[d-1]:NN;

        copyFromMEM(val, w, VAL[d], iW[d], S);
        for(i=S; i<M; i++)
            for(size_t k=0; k<K; k++)
                C[i*K+k] = CT[i*k+K];

        free(VAL[d]);
        free(iW[d]);

        M = 0;
        N = S;
        i = 0;

        while( i<S )
        {
            j = i+1;

            if( j<S && ((val[i]&0xFFFFFFFFFFFFFFFE)==(val[j]&0xFFFFFFFFFFFFFFFE)) )
            {
                N--;
                itransform(Qstep, w[i], w[j], C+i*K, C+j*K, CT+M*K, CT+N*K, K);
                i += 2;
            }
            else
            {
                for(size_t k=0; k<K; k++)
                    C[i*K+k] = CT[M*K+k];
                i += 1;
            }
            M++;
        }

        for(i=0; i<S; i++)
            for(size_t k=0; k<K; k++)
                CT[i*K+k] = C[i*K+k];
    }

    // Copia dados para saida
    free(iM);
    free(iW);
    free(VAL);
    free(valT);
    free(val);
    free(wT);
    free(w);
    free(CT);

    //double *CX = outC, *CY = CX+NN, *CZ = CY+NN;

    for(i=0; i<NN; i++)
        for(size_t k=0; k<K; k++)
            outC[ord[i]+NN*k] = (double) C[i*K+k]/(0x1<<INTEGER_TRANSFORM_PRECISION);

    free(C);
    free(ord);
}
