#include <Arduino.h>
#include <Filter.h>

struct inst_f32
{
  unsigned int numStages;
  float *pState;
  float *pCoeffs;
  float *p_bx_Gain;
};

void filter_init(
  inst_f32 *S,
  uint8_t numStages,
  float *pCoeffs,                     /* const? */
  float *p_bx_Gain,
  float *pState)
{
  /* Assign filter stages */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Assign gain pointer */
  S->p_bx_Gain = p_bx_Gain;

  /* Clear state buffer and size is always 4 * numStages */
  memset(pState, 0, (4U * (uint32_t) numStages) * sizeof(float));

  /* Assign state pointer */
  S->pState = pState;
}

void filter_implement(
  const inst_f32 *S,
  float *pSrc,                              /* to input data block    */
  float *pDst,                              /* to output data block   */
  unsigned int blockSize)
{
  float *pIn = pSrc;                         /* source pointer         */
  float *pOut = pDst;                        /* destination pointer    */
  float *pState = S->pState;                 /* state pointer          */
  float *pCoeffs = S->pCoeffs;               /* coefficient pointer    */
  float *p_bx_Gain = S->p_bx_Gain;           /* gain pointer           */
  float acc;                                 /* accumulator            */
  float b0, b1, b2, a1, a2, g;               /* filter coefficients    */
  float Xn1, Xn2, Yn1, Yn2;                  /* filter state variables */
  float Xn;                                  /* temporary input        */
  unsigned int sample, stage = S->numStages; /* loop count             */
  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;
    g = *p_bx_Gain++;

    Xn1 = pState[0];
    Xn2 = pState[1];
    Yn1 = pState[2];
    Yn2 = pState[3];

    sample = blockSize >> 2u;                 /* equal to divided by 4  */

    while(sample > 0u)
    {
      /* Read the first input */
      Xn = *pIn++;

      /* acc = b0 * Xn + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      Yn2 = g * ((b0 * Xn) + (b1 * Xn1) + (b2 * Xn2)) + (a1 * Yn1) + (a2 * Yn2);

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = Yn2;

      /* State Update to: */
      /* Xn2 = Xn1 */
      /* Xn1 = Xn  */
      /* Yn2 = Yn1 */
      /* Yn1 = acc */

      /* Read the second input */
      Xn2 = *pIn++;

      /* acc = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      Yn1 = g * ((b0 * Xn2) + (b1 * Xn1) + (b2 * Xn1)) + (a1 * Yn2) + (a2 * Yn1); /* ??? */

      /* store the result to accumulator in target buffer */
      *pOut++ = Yn1;

      /* State Update to: */
      /* Xn2 = Xn1 */
      /* Xn1 = Xn  */
      /* Yn2 = Yn1 */
      /* Yn1 = acc */

      /* Read the third input */
      Xn1 = *pIn++;

      /* acc = b0 * X[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      Yn1 = g * ((b0 * Xn) + (b1 * Xn1) + (b2 * Xn2)) + (a1 * Yn1) + (a2 * Yn2);

      /* store the result to accumulator in target buffer */
      *pOut++ = Yn2;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as: */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc   */

      /* Read the forth input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      Yn1 = g * ((b0 * Xn) + (b1 * Xn1) + (b2 * Xn2)) + (a1 * Yn2) + (a2 * Yn1);

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = Yn1;

      Xn2 = Xn1;
      Xn1 = Xn;

      sample--;
    }
      /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
       ** No loop unrolling is used. */
    sample = blockSize & 0x3u;

    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      acc = g * ((b0 * Xn) + (b1 * Xn1) + (b2 * Xn2)) + (a1 * Yn1) + (a2 * Yn2);

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = acc;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:    */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc   */
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = acc;

      /* decrement the loop counter */
      sample--;

    }

    /*  Store the updated state variables back into the pState array */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent numStages  occur in-place in the output buffer */
    pIn = pDst;

    /* Reset the output pointer */
    pOut = pDst;

    /* decrement the loop counter */
    stage--;

  } while(stage > 0u);
}

int filter_main(
  unsigned int CASC_SECTION,    /* IIR Block No.(row No. of coefficient)  */
  float *iir_state,             /* State Var. Buffer region (Xnx and Ynx) */
  float *iir_coeffs,            /* coefficients                           */
  float *iir_gain,              /* gain                                   */
  float *pInput,                /* input data block pointer               */
  float *pOutput,               /* output data block pointer              */
  unsigned int blockSize)       /* input data block size                  */
{
  inst_f32 S;
  filter_init(&S, CASC_SECTION, (float *)&iir_coeffs[0], (float *)&iir_gain, (float *)&iir_state[0]);
  filter_implement(&S, pInput, pOutput, blockSize);
}
