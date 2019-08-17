#include <stdio.h>

typedef char *  string;

int series(int data[], int keta, int maxItem, int bunsi, int bunbo, int plusMinus) {
    int  bunbo2;
    int  item[260], itemDiv[260];
    int  len;
    int  i, j, k;
    int  x;

    len = (keta-1)/4 + 2 + 1;
    i = 0;
    while (i < len) {
        item[i] = 0;
        i = i + 1;
    }

    item[0] = bunsi * bunbo * 4;
    bunbo2  = bunbo * bunbo;

    j = 1;
    while (maxItem > 0) {
        maxItem = maxItem - 1;
        x = 0;
        i = 0;
        while (i < len) {
            x = x * 10000 + item[i];
            item[i] = x / bunbo2;
            x = x % bunbo2;
            i = i + 1;
        }
        if (j == 1) {
            i = 0;
            while (i < len) {
                itemDiv[i] = item[i];
                i = i + 1;
            }
        } else {
            x = 0;
            i = 0;
            while (i < len) {
                x = x * 10000 + item[i];
                itemDiv[i] = x / j;
                x = x % j;
                i = i + 1;
            }
        }
        k = 0;
        if (plusMinus % 2 == 1) {
            i = len - 1;
            while (i >= 0) {
                data[i] = data[i] + itemDiv[i] + k;
                if (data[i] < 10000) k = 0;
                else { data[i] = data[i] - 10000; k = 1; }
                i = i - 1;
            }
        } else {
            i = len - 1;
            while (i >= 0) {
                data[i] = data[i] - (itemDiv[i] + k);
                if (data[i] >= 0) k = 0;
                else { data[i] = data[i] + 10000; k = 1; }
                i = i - 1;
            }
        }
        plusMinus = plusMinus + 1;
        j = j + 2;
    }
    return 0;
}

int pi(int data[], int keta) {
    int maxItem_5, maxItem_239, i;

    maxItem_5   = keta * 500 /  699 + 2;
    maxItem_239 = keta * 500 / 2378 + 2;
    i = 0;
    while (i < (keta-1)/4+2) {
        data[i] = 0;
        i = i + 1;
    }
    if (series(data, keta, maxItem_5, 4, 5, 1) != 0) return 1;
    if (series(data, keta, maxItem_239, 1, 239, 0) != 0) return 1;
    return 0;
}

int main() {
    int n, k;
    int data[10000];    
    string pai1;
    string pai2; 
    pai1 = "3.1415 9265 3589 7932 3846 2643 3832 7950 2884 1971 6939 9375 1058 2097 ";
    pai2 = "4944 5923 0781 6406 2862 0899 8628 0348 2534 2117 0679 8214 8086 5132 8230 6647";

    n = 1000;
    printf("n=%d%c", n, 10);

    k = 0;
    while (k < 10000) {
        data[k] = 0;
        k = k + 1;
    }

    pi(data, n);
    k = 0;
    while (k < (n-1)/4 + 2) {
        printf("%04d ", data[k]);
        k = k + 1;
    }
    printf("%c‰ð:%s%s%c", 10, pai1, pai2, 10);
    return 0;
} 
