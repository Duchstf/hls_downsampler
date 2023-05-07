##################
## DOWN SAMPLER ##
##################
import numpy as np 

def DownSample(input_data, mod, sampling_freq):
    """
    INPUT: input_data       | list of five 16bit data samples
           mod              |   "average" or "decimation" 
           sampling_freq    | integer representing  sampling frequency , needs to be multiple of 5 and <= 5e5
    
    OUTPUT: ouput_data | list of five 16bit data samples, averaged or decimated     
    """
    ## INITIALIZING VARIABLES 
    ready_to_output = False 
    factor = int(500e6 / sampling_freq) # 500e6 is the intrinsic sampling frequency of the ADC
    output_data = np.empty(5) # prepare output data

    ## Check that the factor is valid 
    if factor > 5e5:
        raise ValueError("Downsampling factor must be maximum 500k")
    if (factor % 5 != 0) and (factor != 1):
        raise ValueError("Downsampling factor must be a multiple of 5 (or 1)")
    ## Check that mod is valid
    if mod != "average" and mod != "decimation":
        raise ValueError("Mod must be either 'average' or 'decimation'")

    sample_counter = 0 # Counts from 0 to factor, then resets to 0
    output_counter = 0 # Counts from 0 to 4, then resets to 0
    running_average = 0  # Running average of FACTOR data samples

    ## In Verilog, this will be something like: always @(posedge clk) begin 
    ## which will make sure that new input_data arrives at every clock cycle (100MHz)'
    while not ready_to_output:

        ## If factor = 1, no Decimation or Average necessary 
        if factor == 1:
            output_data = input_data
            ready_to_output = True
        
        else:
            running_average += np.sum(input_data)/factor 
            sample_counter += 5

            if sample_counter == factor:
                if mod == "decimation": # keep only the last of FACTOR samples
                    output_data[output_counter] = input_data[-1]
                else: # mod == "average"
                    output_data[output_counter] = running_average

                output_counter += 1
                sample_counter = 0
                running_average = 0

                if output_counter == 5:
                    ready_to_output = True
                    output_counter = 0
    
    return output_data