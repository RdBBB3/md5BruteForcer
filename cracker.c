#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

int cmpHash(const char *user_hash, unsigned char *calculated_hash, unsigned int hash_len) {
    char calculated_hash_hex[2 * EVP_MAX_MD_SIZE + 1];
    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(&calculated_hash_hex[i * 2], "%02x", calculated_hash[i]);
    }
    calculated_hash_hex[hash_len * 2] = '\0';
    if (strcmp(user_hash, calculated_hash_hex) == 0) {
        return 0;
    }
    return -1; 
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage : %s <ur hash> <wordlist>\n", argv[0]);
        return -1;
    }

    FILE *wordlist = fopen(argv[2], "r");
    if (wordlist == NULL) {
        perror("Erreur lors de l'ouverture de la wordlist");
        return -1;
    }

    int result;
    int i = 0;
    char buffer[256];
    const char *myHash = argv[1];

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    while (fgets(buffer, sizeof(buffer), wordlist) != NULL) {
        i++;
        buffer[strcspn(buffer, "\n")] = 0;
        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
        EVP_DigestUpdate(mdctx, buffer, strlen(buffer));
        EVP_DigestFinal_ex(mdctx, hash, &hash_len);
        EVP_MD_CTX_free(mdctx);

        result = cmpHash(myHash, hash, hash_len);
        if (result == 0) {
            printf("\nVotre mot de passe hashé est : %s\n", buffer);
            fclose(wordlist);
            return 0;
        }

        printf("\rTry %d", i);
        fflush(stdout);
    }

    printf("\nAucun mot de passe n'a été trouvé pour ce hash\n");
    fclose(wordlist);
    return 0;
}
