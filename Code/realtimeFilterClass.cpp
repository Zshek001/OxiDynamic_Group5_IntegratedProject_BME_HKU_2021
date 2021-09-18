#include <Arduino.h>


class Filter
{
  const static int _numStages = 13; //need change to input-defined
  float *_pCoeffs;
  float *_p_bx_Gain;
  float *_pState; //need malloc(), 5 * sizeof(float * _numStages)
  float *_pBuffer; //need malloc(), 4 * sizeof(float * _numStages)
  int _counter = 0; //need to be integrated into filtering

  public: void filter_init(
    // unsigned int numStages,
    float *pCoeffs,
    float *p_bx_Gain
  )
  {
    // _numStages = numStages;
    _pCoeffs = pCoeffs;
    _p_bx_Gain = p_bx_Gain;
    

  };

  public: void filtering(
      float *pIn, // need increment
      float *pOut // automatic increment
    )
    {

      int _stage = 0;
      float b0, b1, b2, a1, a2;
      float g;
      float Xn0, Xn1, Xn2, Yn1, Yn2;

      do
      {
        b0 = *_pCoeffs++;
        b1 = *_pCoeffs++;
        b2 = *_pCoeffs++;
        a1 = *_pCoeffs++;
        a2 = *_pCoeffs++;
        g = *_p_bx_Gain++;

        Xn0 = _pState[0];
        Xn1 = _pState[1];
        Xn2 = _pState[2];
        Yn1 = _pState[3];
        Yn2 = _pState[4];

        switch (_counter % 4){
          case 0:
          if (_stage == 0){
            Xn0 = *pIn;
          }
          else
          {
            Xn0 = _pBuffer[4 * (_stage - 1)];
          }
          Yn2 = g * ((b0 * Xn0) + (b1 * Xn1) + (b2 * Xn2)) - (a1 * Yn1) - (a2 * Yn2);
          _pBuffer[4 * _stage] = Yn2;
          break;

          case 1:
          if (_stage == 0){
            Xn2 = *pIn;
          }
          else
          {
            Xn2 = _pBuffer[4 * (_stage - 1) + 1];
          }
          Yn1 = (b0 * Xn2) + (b1 * Xn0) + (b2 * Xn1) + (a1 * Yn2) + (a2 * Yn1);
          _pBuffer[4 * _stage + 1] = Yn1;
          break;

          case 2:
          if (_stage == 0){
            Xn1 = *pIn;
          }
          else
          {
            Xn1 = _pBuffer[4 * (_stage - 1) + 2];
          }
          Yn2 = (b0 * Xn1) + (b1 * Xn2) + (b2 * Xn0) + (a1 * Yn1) + (a2 * Yn2);
          _pBuffer[4 * _stage + 2] = Yn2;
          break;

          case 3:
          if (_stage == 0){
            Xn0 = *pIn;
          }
          else
          {
            Xn0 = _pBuffer[4 * (_stage-1) + 3];
          }
          Yn1 = (b0 * Xn0) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn2) + (a2 * Yn1);
          _pBuffer[4 * _stage + 3] = Yn1;
          Xn2 = Xn1;
          Xn1 = Xn0;
          break;
        }

        *_pState++ = Xn0;
        *_pState++ = Xn1;
        *_pState++ = Xn2;
        *_pState++ = Yn1;
        *_pState++ = Yn2;

        /* increment the loop counter */
        _stage++;
      }while(_stage < _numStages);

      *pOut++ = *(_pBuffer + 4 * (_numStages - 1) + _counter % 4);

      _pState -= _numStages * 5 + 1;
      _counter++;
    };
};
