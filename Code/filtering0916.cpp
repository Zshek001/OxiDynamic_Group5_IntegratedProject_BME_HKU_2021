typedef struct
{
  unsigned int numStages;
  float *pStates;
  float *pCoeffs;
} inst_f32


void filter_init(
  inst_f32 *S,
  uint8_t numStages,
const float *pCoeffs,
      float *pState)
{
  /* Assign filter stages */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always 4 * numStages */
  memset(pState, 0, (4U * (uint32_t) numStages) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;
}

void filter_main(
  const inst_f32 * S,
  float * pSrc,                              /* to input data block    */
  float * pDst,                              /* to output data block   */
  unsigned int blockSize)
{
  float *pIn = pSrc;                         /* source pointer         */
  float *pOut = pDst;                        /* destination pointer    */
  float *pState = S->pState;                 /* state pointer          */
  float *pCoeffs = S->pCoeffs;               /* coefficient pointer    */
  float acc;                                 /* accumulator            */
  float b0, b1, b2, a1, a2;                  /* filter coefficients    */
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

  Xn1 = pState[0];
  Xn2 = pState[1];
  Yn1 = pState[2];
  Yn2 = pState[3];

  sample = blockSize >> 2u;

  while(sample > 0u)
  {
    /* Read the first input */
    Xn = *pIn++;

    /* acc = b0 * Xn + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
    Yn2 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);

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
    Yn1 = (b0 * Xn2) + (b1 * Xn) + (b2 * Xn1) + (a1 * Yn2) + (a2 * Yn1);

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
    Yn1 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn2) + (a2 * Yn1);

    /* store the result to accumulator in target buffer */
    *pOut++ = Yn1;

    /* State Update to: */
    /* Xn2 = Xn1 */
    /* Xn1 = Xn  */
    /* Yn2 = Yn1 */
    /* Yn1 = acc */
    Xn2 = Xn1;
    Xn1 = Xn;

    sample--;

    while(sample > 0u)
    {
      /* Read input */
      Xn = *pIn++;
      acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);
      *pOut++ = acc;
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = acc;

      sample--;
    }

    /* store updated state variables to pState */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

    /* initially we transfer input buffer to output buffer,
    then we do numStages right in output buffer. */
    pIn = pDst;

    /* initialize output pointer */
    pOut = pDst;

    stage--;
  } while(stage > 0u);
}
