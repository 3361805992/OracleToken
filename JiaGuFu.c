// Usage (encryption): JiaGuFu -C/c plaintext.file ciphertext.file password
// Usage (decryption): JiaGuFu -P/p ciphertext.file plaintext.file password
// Compiled on MacOS, Linux and *BSD in X86_64 platform.
// Talk is SO EASY, show you my GOD.
// Simple is beautiful.

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// swap the values of ausJiaGuFu
void swapValues(unsigned long long *pulJiaGuFu, unsigned char *pucPassword, unsigned long long ulPasswordLength)
{
    for(unsigned long long i = 0; i < 16384; ++i)
    {
        unsigned long long *pulSwap1 = pulJiaGuFu, *pulSwap2 = pulJiaGuFu, ulIndex, ulTemp;

        ulIndex = pucPassword[i % ulPasswordLength] % 16384;

        ulTemp = pulSwap1[i];

        pulSwap1[i] = pulSwap2[ulIndex];

        pulSwap2[ulIndex] = ulTemp;
    }
}

// use the values of ausJiaGuFu to change the password
void changePassword(unsigned short *pusJiaGuFu, unsigned char *pucPassword, unsigned long long ulPasswordLength)
{
    for(unsigned long long j = 0; j < ulPasswordLength; ++j)
    {
        pucPassword[j] = (unsigned char)pusJiaGuFu[pucPassword[j]];
    }
}

int main(int argc, char *argv[])
{
    if(argv[1][0] == '-' && (argv[1][1] == 'C' || argv[1][1] == 'c' || argv[1][1] == 'P' || argv[1][1] == 'p'))
    {
// any password length
        unsigned long  long ulPasswordLength = 0;

        while(argv[4][++ulPasswordLength]);

// initialize random seed
        srand(*(unsigned int*)argv[4] % RAND_MAX);

// initialize JiaGuFu
        unsigned short ausJiaGuFu[65536];

        for(unsigned long long i = 0; i < 65536; ++i) ausJiaGuFu[i] = (31 * i + 37) % 65536;

        struct stat statFileSize;

        stat(argv[2], &statFileSize);

// get the plaintext or ciphertext file size
        unsigned long long ulFileSize = statFileSize.st_size;

// allocate storage space
        unsigned char *pucPlaintextOrCiphertext = (unsigned char*)malloc(ulFileSize), *pucCiphertextOrPlaintext = NULL;

// open the plaintext or ciphertext file
        int iPlaintextOrCiphertext = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR);

// read data from the plaintext or ciphertext file
        read(iPlaintextOrCiphertext, pucPlaintextOrCiphertext, ulFileSize);

        close(iPlaintextOrCiphertext);

        if(argv[1][1] == 'c' || argv[1][1] == 'C')
        {
            pucCiphertextOrPlaintext = (unsigned char*)malloc(2 * ulFileSize);

            for(unsigned long long i = 0; i < ulFileSize; i += 256)
            {
                swapValues((unsigned long long*)ausJiaGuFu, (unsigned char*)argv[4], ulPasswordLength);

// use XOR to process the 256 bytes of plaintext at a time
                for(unsigned long long j = 0, k = 0; j < 256 && i + j < ulFileSize; ++j, ++k)
                    ((unsigned short*)pucCiphertextOrPlaintext)[i + j] = ausJiaGuFu[256 * k + pucPlaintextOrCiphertext[i + j]] ^ ausJiaGuFu[256 * k + rand() % 256];

                changePassword(ausJiaGuFu, (unsigned char*)argv[4], ulPasswordLength);
            }

            ulFileSize *= 2;
        }
        else if(argv[1][1] == 'p' || argv[1][1] == 'P')
        {
            ulFileSize /= 2;

            pucCiphertextOrPlaintext = (unsigned char*)malloc(ulFileSize);

            for(unsigned long long i = 0; i < ulFileSize; i += 256)
            {
                 swapValues((unsigned long long*)ausJiaGuFu, (unsigned char*)argv[4], ulPasswordLength);

// use XOR to process the 256 bytes of ciphertext at a time
                for(unsigned long long j = 0, k = 0; j < 256 && i + j < ulFileSize; ++j, ++k)
                {
                    unsigned short usCipher = ((unsigned short*)pucPlaintextOrCiphertext)[i + j] ^ ausJiaGuFu[256 * k + rand() % 256];

                    for(unsigned long long l = 0; l < 256; ++l)
                    {
                        if(usCipher == ausJiaGuFu[256 * k + l])
                        {
                            pucCiphertextOrPlaintext[i + j] = l;

                            break;
                        }
                    }
                }

                changePassword(ausJiaGuFu, (unsigned char*)argv[4], ulPasswordLength);
            }
        }

// open the ciphertext or plaintext file
        iPlaintextOrCiphertext = open(argv[3], O_CREAT | O_WRONLY, S_IREAD | S_IWRITE);

// write data to the ciphertext or plaintext file
        write(iPlaintextOrCiphertext, pucCiphertextOrPlaintext, ulFileSize);

        close(iPlaintextOrCiphertext);

        free(pucCiphertextOrPlaintext);

        free(pucPlaintextOrCiphertext);
    }

    return 0;
}