import random

def main():

    #Generate 1500e+6 samples (equivalent to 3 seconds of waveform)
    #Just using random number for now
    wave_list = ['0x{0:0>4x}'.format(random.randint(0,10000)) for i in range(0,int(100))]

    with open('../data/TestWaveForm.txt','w') as tfile:
        tfile.write('\n'.join(wave_list))


main()