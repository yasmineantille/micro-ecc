#include "ecc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void vli_print(uint8_t *p_vli, unsigned int p_size)
{
    while(p_size)
    {
        printf("%02X ", (unsigned)p_vli[p_size - 1]);
        --p_size;
    }
}

int randfd;

int RNG(uint8_t *p_dest, unsigned p_size)
{
    if(read(randfd, p_dest, p_size) != (int)p_size)
    {
        printf("Failed to get random bytes.\n");
        return 0;
    }
    return 1;
}

int main()
{
    int i;
    
    uint8_t l_private1[ECC_BYTES];
    uint8_t l_private2[ECC_BYTES];
    
    uint8_t l_public1[ECC_BYTES + 1];
    uint8_t l_public2[ECC_BYTES + 1];
    
    uint8_t l_secret1[ECC_BYTES];
    uint8_t l_secret2[ECC_BYTES];
    
    randfd = open("/dev/urandom", O_RDONLY);
    if(randfd == -1)
    {
        printf("No access to urandom\n");
        return -1;
    }
    
    ecc_set_rng(&RNG);
    
    printf("Testing 256 random private key pairs\n");

    for(i=0; i<256; ++i)
    {
        printf(".");
        fflush(stdout);

        ecc_make_key(l_public1, l_private1);
        ecc_make_key(l_public2, l_private2);

        if(!ecdh_shared_secret(l_public2, l_private1, l_secret1))
        {
            printf("shared_secret() failed (1)\n");
            return 1;
        }

        if(!ecdh_shared_secret(l_public1, l_private2, l_secret2))
        {
            printf("shared_secret() failed (2)\n");
            return 1;
        }
        
        if(memcmp(l_secret1, l_secret2, sizeof(l_secret1)) != 0)
        {
            printf("Shared secrets are not identical!\n");
            printf("Shared secret 1 = ");
            vli_print(l_secret1, ECC_BYTES);
            printf("\n");
            printf("Shared secret 2 = ");
            vli_print(l_secret2, ECC_BYTES);
            printf("\n");
            printf("Private key 1 = ");
            vli_print(l_private1, ECC_BYTES);
            printf("\n");
            printf("Private key 2 = ");
            vli_print(l_private2, ECC_BYTES);
            printf("\n");
        }
    }
    printf("\n");
    
    return 0;
}
