// Usage (encryption): OracleToken -C/c plaintext.file ciphertext.file LuckyNumber
// Usage (decryption): OracleToken -P/p ciphertext.file plaintext.file LuckyNumber
// Compiled on MacOS, Linux and *BSD in X86_64 platform.
// Talk is SO EASY, show you my GOD.
// Simple is beautiful.

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if(argv[1][0] == '-' && (argv[1][1] == 'C' || argv[1][1] == 'c' || argv[1][1] == 'P' || argv[1][1] == 'p'))
    {
// get the Lucky Number
        unsigned int uiLuckyNumber = 0;

        for(unsigned long long j = 0; argv[4][j]; ++j) uiLuckyNumber = 10 * uiLuckyNumber + argv[4][j] - 48;

// initialize random seed
        srand(uiLuckyNumber % RAND_MAX);

// initialize Oracle Tokens
        unsigned short ausOracleTokens[65536];

        for(unsigned long long i = 0; i < 65536; ++i) ausOracleTokens[i] = (13 * i + 17) % 65536;

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

        if(argv[1][1] == 'C' || argv[1][1] == 'c')
        {
            pucCiphertextOrPlaintext = (unsigned char*)malloc(2 * ulFileSize);

            for(unsigned long long i = 0; i < ulFileSize; i += 256)
            {
// use XOR to process the 256 bytes of plaintext at a time
                for(unsigned long long j = 0, k = 0; j < 256 && i + j < ulFileSize; ++j, ++k)
                    ((unsigned short*)pucCiphertextOrPlaintext)[i + j] = ausOracleTokens[256 * k + pucPlaintextOrCiphertext[i + j]] ^ ausOracleTokens[256 * k + rand() % 256];
            }

            ulFileSize *= 2;
        }
        else if(argv[1][1] == 'P' || argv[1][1] == 'p')
        {
            ulFileSize /= 2;

            pucCiphertextOrPlaintext = (unsigned char*)malloc(ulFileSize);

            for(unsigned long long i = 0; i < ulFileSize; i += 256)
            {
// use XOR to process the 256 bytes of ciphertext at a time
                for(unsigned long long j = 0, k = 0; j < 256 && i + j < ulFileSize; ++j, ++k)
                {
                    unsigned short usCipher = ((unsigned short*)pucPlaintextOrCiphertext)[i + j] ^ ausOracleTokens[256 * k + rand() % 256];

                    for(unsigned long long l = 0; l < 256; ++l)
                    {
                        if(usCipher == ausOracleTokens[256 * k + l])
                        {
                            pucCiphertextOrPlaintext[i + j] = l;

                            break;
                        }
                    }
                }
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
