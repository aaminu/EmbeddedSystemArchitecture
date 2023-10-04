#include <stdio.h>
#include <stdint.h>

#define KEY_SIZE 256

int add_world();

const uint8_t __attribute__((used, section(".keys"))) private_key[KEY_SIZE] = {0};

int main(void)
{
    int a = 5;
    int b = 7;
    int c = add_world(a, b);
    return 0;
}
