#ifndef WORLEYH
#define WORLEYH

#include "vec3.h"

// Worley noise

class worley 
{
 public:

  vec3 CellNoiseFunc(const vec3 &input, const int seed)
  {
    int lastRandom, numberFeaturePoints;
    vec3 randomDiff, featurePoint;
    int cubeX, cubeY, cubeZ;
    
  
    // 1. Determine which cube the evaluation point is in

    // 2. Generate a RNG for the cube

    // 3. Determine how many feature points are in cube using Poisson distribution
  
  

    // 4. Randomly place the feature points in the cube

    // 5. Fine the feature point closes to the evaluation point 

    // 6. Check neighboring cubes to ensure that there are no closer evaluation points
  
  }

 private:
  



};

#endif
