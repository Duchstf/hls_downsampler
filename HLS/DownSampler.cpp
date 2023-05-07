#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

// Libraries included in vivado hls
#include "ap_int.h"
#include "ap_fixed.h"

using namespace std;

// Define some relevant types
// and number of links
typedef ap_int<80> LinkBit;
typedef ap_int<16> WordBit;

#define N_LINKS_IN 4
#define N_LINKS_OUT 4

FILE * fp;

/*
Take the 16x5 word and then average them
*/

void AverageDownSampler(const LinkBit input, LinkBit &output, const int sampling_freq)
{   

    bool ready_to_output = false;
    int factor = int(500e+6 / sampling_freq);
    float factor_divide = 1./factor;

    fp = fopen("output.txt", "w+");
    fprintf(fp, "Factor: %d \n", factor);
    fprintf(fp, "Factor divide: %.10f \n", factor_divide);

    int sample_counter = 0;  // Counts from 0 to factor, then resets to 0
    int output_counter = 0;  // Counts from 0 to 4, then resets to 0
    float running_average = 0; // Running average of FACTOR data samples

    //To get each individual word in the link
    WordBit word;

    while (!ready_to_output)
    {

        if (factor == 1)
        {   
            output = input;
            ready_to_output = true;
        }
        else
        {   
            //Calculate the running average
            for(int i0=0; i0 < 5; i0++){

                word(15,0) = input(16*i0+15, 16*i0);
                fprintf(fp, "Word: %.10f \n", word.to_float());

                running_average += word.to_float()*factor_divide;
                fprintf(fp, "Running average: %.10f \n", running_average);
            }
           

            sample_counter += 5;
            fprintf(fp, "Sample counter: %d \n", sample_counter);

            // if (sample_counter == factor){
            //     fprintf(fp, "Running average END: %.10f \n", running_average);
            //     output[output_counter] = LinkBit(running_average);
            // }

            fprintf(fp, "Running average END: %.10f \n", running_average);
            output = LinkBit(running_average*500e+6);

            output_counter += 1;
            sample_counter = 0;
            running_average = 0;

            if (output_counter == 5){
                ready_to_output = true;
                output_counter = 0;
            }
        }
    }
}

/*
DownSampler takes in 4 links of data.
Data on each link is a 80 bit word.

The outputs are written into 4 links as well.
*/
void AverageDownSamplerWrapper(const LinkBit inputs[N_LINKS_IN], LinkBit outputs[N_LINKS_OUT])
{

#pragma HLS ARRAY_PARTITION variable = inputs complete
#pragma HLS ARRAY_PARTITION variable = outputs complete

#pragma HLS data_pack variable = inputs
#pragma HLS data_pack variable = outputs

#pragma HLS INTERFACE ap_none port = output
#pragma HLS DATAFLOW

//Downsample each link separately
AverageDownSampler(inputs[0], outputs[0], 500);
AverageDownSampler(inputs[1], outputs[1], 500);
AverageDownSampler(inputs[2], outputs[2], 500);
AverageDownSampler(inputs[3], outputs[3], 500);

}

/*
Main function to perform testbench. It is not synthesized into Verilog code.
Print out the output from the algorithm (this is a C++ simulation so there would be no delay)
If you run this on a real FPGA board there would be some delay between the input/output
    of the algorithm depending on the latency/initiation interval of the algorithm.
*/
int main(int argc, char **argv)
{   
    
    LinkBit input_test[N_LINKS_IN];
    LinkBit output_test[N_LINKS_OUT];

    fp = fopen("output.txt", "w+");

    // First read in the test data
    // Each line in the text file is 1 clock cycle from the DAQ
    ifstream in("../../../../../data/TestWaveForm.txt"); //data file relative to the compiled DownSampler
    string str;
    while(getline(in, str))
    {   
        cout<<str<<endl;
    }
    return 0;

    fprintf(fp, "Input bits: \n");
    for(int i=0; i < N_LINKS_IN; i++){
        input_test[i] = 0xFFFF;
        fprintf(fp, "Link(%d): %.10f \n", i, input_test[i].to_float());
    }
    fprintf(fp, "--------------\n");
    
    // Pass the data into the algorithm
    AverageDownSamplerWrapper(input_test, output_test);

    fprintf(fp, "--------------\n");
    fprintf(fp, "Output bits: \n");
    for(int i=0; i < N_LINKS_OUT; i++){
        fprintf(fp, "Link(%d): %.10f \n", i, output_test[i].to_float());
    }
    fprintf(fp, "--------------\n");

    fclose(fp);
    return 0;
}