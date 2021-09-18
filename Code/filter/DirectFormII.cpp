const int numStage = 13;
float *Wn_store = new float[numStage * 2]; // Wn of all stages are stored here. global.

void DirectFormIISOS(
  float input,
  float *pOutput,
  float *pCoeffs,
  float *p_bx_Gain
)
{
  int stage;
  float Xn, Yn;
  float a1, a2, g, b0, b1, b2;
  float Wn1 = 0, Wn2 = 0;
  float Wn0;

  stage = 0;

  Yn = input;

  do {

    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;
    g = *p_bx_Gain++;


    Wn1 = Wn_store[stage * 2];
    Wn2 = Wn_store[stage * 2 + 1];
    Xn = Yn;

    Wn0 = - a1 * Wn1 - a2 * Wn2 + Xn;
    Yn = g * (b0 * Wn0 + b1 * Wn1 + b2 * Wn2);

    Wn_store[stage * 2] = Wn0;
    Wn_store[stage * 2 + 1] = Wn1;

    stage++;

  } while(stage < numStage);

  *pOutput++ = Yn;
};
