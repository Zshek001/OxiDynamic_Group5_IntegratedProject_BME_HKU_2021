#ifdef FILTER_H
#define FILTER_H

typedef struct
{
  unsigned int numStages;
  float *pState;
  float *pCoeffs;
  float *p_bx_Gain
} inst_f32;

int filter_main(
  unsigned int CASC_SECTION,    /* IIR Block No.(row No. of coefficient)  */
  float *iir_state,             /* State Var. Buffer region (Xnx and Ynx) */
  float *iir_coeffs,            /* coefficients                           */
  float *iir_gain               /* gain                                   */
  float *pInput,                /* input data block pointer               */
  float *pOutput,               /* output data block pointer              */
  unsigned int blockSize);      /* input data block size                  */

void filter_init(
  inst_f32 *S,
  uint8_t numStages,
  const float *pCoeffs,
  const float *p_bx_Gain,
  const float *pState);

void filter_implement(
  const inst_f32 *S,
  float *pSrc,                              /* to input data block    */
  float *pDst,                              /* to output data block   */
  unsigned int blockSize);

#endif
