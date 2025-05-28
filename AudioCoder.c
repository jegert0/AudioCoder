#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//containes header information for a wav file
typedef struct wav_header_s{
    char riff[4];
    int32_t flength;
    char wave[4];
    char fmt[4];
    int32_t chunk_size;
    int16_t format_tag;
    int16_t num_chans;
    int32_t srate;
    int32_t bytes_per_second;
    int16_t bytes_per_sample;
    int16_t bits_per_sample;
    char data[4];
    int32_t dlength;
} wav_header;

const int sample_rate = 44100; //44.1 khz
const char RIFF[4] = "RIFF";
const char WAVE[4] = "WAVE";
const char fmt[4] = "fmt ";
const char data[4] = "data";

wav_header create_Wav_Header(){
    //still missing dlength and flength
    wav_header wavh;
    //write header chars
    strncpy(wavh.riff, RIFF, 4);
    strncpy(wavh.wave, WAVE, 4);
    strncpy(wavh.fmt, fmt, 4);
    strncpy(wavh.data, data, 4);

    wavh.chunk_size = 16;
    wavh.format_tag = 1;
    wavh.num_chans = 1;
    wavh.srate = sample_rate;
    wavh.bits_per_sample = 16;
    wavh.bytes_per_sample = (wavh.bits_per_sample / 8) * wavh.num_chans;
    wavh.bytes_per_second = wavh.srate * wavh.bytes_per_sample;

    return wavh; //return started wav header
}

void complete_header(unsigned long long int * total_bytes, wav_header * head){
    head->dlength = *total_bytes;
    head->flength = head->dlength + 44;
}

//turn file into wav file
void encode(char * source, char * dest, wav_header * head){
    FILE * sp = fopen(source, "rb"); // source file pointer
    FILE * dp = fopen(dest, "wb"); // dest file pointer

    fseek(dp, 44, SEEK_SET); // skip size of header and start writing data
    unsigned char buffer[1024]; // 1kb buffer
    unsigned long long int total_bytes = 0; // stores the total bytes that will be read from source file 
    int bytes; // stores the number of bytes in the buffer

    // write data to destination file and count total bytes
    while((bytes = fread(buffer, sizeof(unsigned char), sizeof(buffer), sp)) != 0){
        total_bytes += bytes;
        fwrite(buffer, sizeof(unsigned char), bytes, dp); //write to dest
    }

    complete_header(&total_bytes, head);

    fseek(dp, 0, SEEK_SET); // go back to beginning to write header
    fwrite(head, 1, sizeof(*head), dp); // write header

    fclose(sp);
    fclose(dp);
}

//turn wav file to original file
void decode(char * source, char * dest){
    FILE * sp = fopen(source, "rb");
    FILE * dp = fopen(dest, "wb");

    unsigned char buffer[1024];

    fseek(sp, 44, SEEK_SET); // skip wav file header

    int bytes;

    while ((bytes = fread(buffer, sizeof(unsigned char), sizeof(buffer), sp)) != 0){
        fwrite(buffer, sizeof(unsigned char), bytes, dp);
    }

    fclose(sp);
    fclose(dp);
}

int main(int argc, char * argv[]){

    if (argc != 4){
        perror("Error usage: .\\AudioCoder <encode/decode> <source file> <destination file>");
        return 1;
    }

    if(!strcmp(argv[1], "encode")) {
        wav_header wavh = create_Wav_Header();
        encode(argv[2], argv[3], &wavh);
    }
    else if(!strcmp(argv[1], "decode")) decode(argv[2], argv[3]);
    else{
        perror("Please type only 'encode' or 'decode'");
        return 1;
    }

    return 0;
}