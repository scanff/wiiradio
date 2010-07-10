// Pretty much copied from Wii DMP :)

#include <malloc.h>
class fft {

        public:

        float *real;
        float *imaginary1;
        float *real2;
        float *imaginary2;
        int dataSize;

        #define START 5
        #define DIVISIONS 24
        #define VARIANCE 0
        #define SCALE_VALUE_BASE 3000
        #define SCALE_VALUE_MUL 2500
        #define FINAL_SCALE 0.333

        public:

        fft() : dataSize(MAX_FFT_SAMPLE)
        {

            real        = (float*)memalign(32,dataSize*sizeof(float));
            imaginary1  = (float*)memalign(32,dataSize*sizeof(float));
            real2       = (float*)memalign(32,dataSize*sizeof(float));
            imaginary2  = (float*)memalign(32,dataSize*sizeof(float));


        }

        ~fft()
        {
            free(real);
            free(imaginary1);
            free(real2);
            free(imaginary2);

        }


        unsigned NumberOfBitsNeeded ( unsigned PowerOfTwo )
        {
            unsigned i;

            for ( i=0; ; i++ )
            {
                if ( PowerOfTwo & (1 << i) )
                    return i;
            }
        }

        unsigned ReverseBits ( unsigned index, unsigned NumBits )
        {
            unsigned i, rev;

            for ( i=rev=0; i < NumBits; i++ )
            {
                rev = (rev << 1) | (index & 1);
                index >>= 1;
            }

            return rev;
        }

        double Index_to_frequency ( unsigned NumSamples, unsigned Index )
        {
            if ( Index >= NumSamples )
                return 0.0;
            else if ( Index <= NumSamples/2 )
                return (double)Index / (double)NumSamples;

            return -(double)(NumSamples-Index) / (double)NumSamples;
        }

    void inline fft_float (
        unsigned  NumSamples,
        int       InverseTransform,
        float    *RealIn,
        float    *ImagIn,
        float    *RealOut,
        float    *ImagOut )
    {
        int NumBits;    /* Number of bits needed to store indices */
        int i, j, k, n;
        int BlockSize, BlockEnd;

        double angle_numerator = 2.0 * (3.14159265359);
        double tr, ti;     /* temp real, temp imaginary */

     /*   if ( InverseTransform )
            angle_numerator = -angle_numerator;
*/
        NumBits = NumberOfBitsNeeded ( NumSamples );

        /*
        **   Do simultaneous data copy and bit-reversal ordering into outputs...
        */

        for ( i=0; i < NumSamples; i++ )
        {
            j = ReverseBits ( i, NumBits );
            RealOut[j] = RealIn[i];
            ImagOut[j] = (ImagIn == NULL) ? 0.0 : ImagIn[i];
        }

        /*
        **   Do the FFT itself...
        */

        BlockEnd = 1;
        for ( BlockSize = 2; BlockSize <= NumSamples; BlockSize <<= 1 )
        {
            double delta_angle = angle_numerator / (double)BlockSize;
            double sm2 = sin ( -2 * delta_angle );
            double sm1 = sin ( -delta_angle );
            double cm2 = cos ( -2 * delta_angle );
            double cm1 = cos ( -delta_angle );
            double w = 2 * cm1;
            double ar[3], ai[3];

            for ( i=0; i < NumSamples; i += BlockSize )
            {
                ar[2] = cm2;
                ar[1] = cm1;

                ai[2] = sm2;
                ai[1] = sm1;

                for ( j=i, n=0; n < BlockEnd; j++, n++ )
                {
                    ar[0] = w*ar[1] - ar[2];
                    ar[2] = ar[1];
                    ar[1] = ar[0];

                    ai[0] = w*ai[1] - ai[2];
                    ai[2] = ai[1];
                    ai[1] = ai[0];

                    k = j + BlockEnd;

                    if (k >= dataSize || k < 0 || j < 0 || j >= dataSize)
                    {
                        break;
                    }


                    tr = ar[0]*RealOut[k] - ai[0]*ImagOut[k];
                    ti = ar[0]*ImagOut[k] + ai[0]*RealOut[k];

                    RealOut[k] = RealOut[j] - tr;
                    ImagOut[k] = ImagOut[j] - ti;

                    RealOut[j] += tr;
                    ImagOut[j] += ti;
                }
            }

            BlockEnd = BlockSize;
        }

        /*
        **   Need to normalize if inverse transform...
        */

    /*    if ( InverseTransform )
        {
            double denom = (double)NumSamples;

            for ( i=0; i < NumSamples; i++ )
            {
                RealOut[i] /= denom;
                ImagOut[i] /= denom;
            }
        }*/
    }

    float getPosition(int pos)
    {
        float fft_item;

        if (pos >= dataSize) return 0;

       // SDL_LockMutex(fft_lock);

        fft_item = real[pos];

        //SDL_UnlockMutex(fft_lock);

        return fft_item;

    }

    int getPeak()
    {
        int fft_peak;

        fft_peak = 0;

//        SDL_LockMutex(fft_lock);

        loopi(MAX_FFT_SAMPLE) if (real[i] > fft_peak) fft_peak = (int)real[i];

//        SDL_UnlockMutex(fft_lock);

        return fft_peak;
    }

    void setAudioData(short *data,int max)
    {

//        SDL_LockMutex(fft_lock);

        // Convert from stereo to mono and set to real, setting imaginary to blank
        for(int i = 0; i < dataSize; i++)
        {
            if ((i<<1) >= max-1) continue;

            // Average and normalized
            real[i] = (float)((float)(data[i << 1]) + (float)(data[(i << 1) + 1])) / (float)(32767 * 2);
            imaginary1[i] = 0.0;
        }

//        SDL_UnlockMutex(fft_lock);
    }

    void getFFT(int *out)
    {
        //SDL_LockMutex(fft_lock);

        fft_float(dataSize, 0, real, imaginary1, real2, imaginary2);

        float* tmp;

        tmp = real;
        real = real2;
        real2 = tmp;

        tmp = imaginary1;
        imaginary1 = imaginary2;
        imaginary2 = tmp;

        // Ensure we have the right count
        int index = 0;

        // Combine into 16 groups
        for(int j = 0; j < DIVISIONS; j++)
        {
            if (index >= dataSize) break;

            // Smoothing
            int numSamples = ((2 * VARIANCE * index) / dataSize) + (dataSize / (DIVISIONS * 2)) - (VARIANCE / 2);

            // Remember before average
            int sampleTotal = 0;

            for(int i = 0; i < numSamples; i++)
            {
                // z = sqrt(real² + imaginary²)
                sampleTotal += (int)(sqrt((real[index] * real[index]) + (imaginary1[index] * imaginary1[index])) * FINAL_SCALE * (float)(SCALE_VALUE_BASE + (j * SCALE_VALUE_MUL)));

                // Move to next index
                index++;
            }

            // Place
            int k = j- START;
            if(k >= 0 && k < MAX_FFT_RES)
            {
                // Divide sample by number of samples to get average and store
                out[k] = sampleTotal / numSamples;

                // Sanity check
                if(out[k] < 0) { out[k] = 0; }
                if(out[k] > 32767) { out[k] = 32767; }
            }
        }

        //SDL_UnlockMutex(fft_lock);
    };
};
