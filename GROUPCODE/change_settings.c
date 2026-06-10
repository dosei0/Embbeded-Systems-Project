#include "audio_interface.h"

void change_resolution(int resolution){


    switch (resolution)
    {
        case 16:

            writeSetting(0b0000111000000001); 
            break;

        case 20:

            writeSetting(0b0000111000000101); 
            break;
        
        case 24:

            writeSetting(0b0000111000001001); 
            break;
    }

}

void change_sample_rate(int rate){

    


}