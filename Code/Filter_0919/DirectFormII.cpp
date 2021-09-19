
/*


float DirectFormIISOS(
  float input,     //real time single feed input
  float *pCoeffs,
  float *p_bx_Gain
)
{
  int stage;
  float Xn, Yn;                //input & output for stages
  float a1, a2, g, b0, b1, b2; //coefficients for stages
  float Wn1 = 0, Wn2 = 0;      //middle variable w(n-1), w(n-2)
  float Wn0;                   //middle variable w(n)

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

  return Yn;
}

*/
