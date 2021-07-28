#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char c;
    float result = 0, currNum = 0;
    while((c = getchar()) != EOF) {
        if (c == '-') {
            scanf("%f", &currNum);
            result -= currNum;
        }
        if (c <= '9' && c >= '0') {
            currNum = c - '0';
            while((c = getchar()) != '.') {
                currNum = currNum * 10 + (c - '0');
            }
            float temp = 0;
            float i = 10;
            while((c = getchar()) != ' ' && c != '\n') {
                temp += ((float)(c - '0'))/i;
                i *= 10;
            }
            result += (currNum + temp);
        }
        if (c == '\n') {
            write(STDOUT_FILENO, &result, sizeof(float));
            result = 0;
            currNum = 0;
        }
    }
    
}