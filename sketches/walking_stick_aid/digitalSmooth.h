/* digitalSmooth
 Paul Badger 2007
 A digital smoothing filter for smoothing sensor jitter 
 This filter accepts one new piece of data each time through a loop, which the 
 filter inputs into a rolling array, replacing the oldest data with the latest reading.
 The array is then transferred to another array, and that array is sorted from low to high. 
 Then the highest and lowest %15 of samples are thrown out. The remaining data is averaged
 and the result is returned.

 Every sensor used with the digitalSmooth function needs to have its own array to hold 
 the raw sensor values. This array is then passed to the function, for it's use.
 This is done with the name of the array associated with the particular sensor.
 */

#ifndef DIGITALSMOOTH_H_
#define DIGITALSMOOTH_H_

#ifndef filterSamples
  #define filterSamples   13              // filterSamples should  be an odd number, no smaller than 3
#endif

#define BUTTOM_CLIP_PERCENT   30
#define TOP_CLIP_PERCENT      70

unsigned int digitalSmooth(int rawIn, int *sensSmoothArray){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }
  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * BUTTOM_CLIP_PERCENT)  / 100), 1); 
  top = min((((filterSamples * TOP_CLIP_PERCENT) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
  }
  return total / k;    // divide by number of samples
}


#endif // end of DIGITALSMOOTH_H_
