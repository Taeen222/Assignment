#include <stdio.h>

//XOR cipher function
void xorCipher(char* data, char key) {
    
    for (int i = 0; data[i] != '\0'; ++i) {
        data[i] ^= key;
    }
    
}

int main() {

    char filename[100];

    printf("Enter balance file name ");

    scanf("%s", filename);

    FILE *f = fopen(filename, "r");

    if (!f){

    printf("File not found!\n"); 
    
    return 1; 

    }

    char buffer[32];


    if (!fgets(buffer, sizeof(buffer), f)){

        printf("Error reading!\n");
            
        fclose(f);
            
        return 1;

    }

    fclose(f);

    xorCipher(buffer, 0x4F); 

    printf("Decoded balance: %s\n", buffer);

    return 0;
}
