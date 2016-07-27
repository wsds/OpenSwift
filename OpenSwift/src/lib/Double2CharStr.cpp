#include "Double2CharStr.h"


/*
   Double to ASCII Conversion without sprintf.
   Roughly equivalent to: sprintf(s, "%.14g", n);
*/

#include <math.h>
#include <string.h>
// For printf
#include <stdio.h>

static double PRECISION = 0.00000000000001;


/**
 * Double to ASCII
 */
char * Double2CharStr(char *char_string, double number) {
    // handle special cases
    if (__isnan(number)) {
        strcpy(char_string, "nan");
    } else if (isinf(number)) {
        strcpy(char_string, "inf");
    } else if (number == 0.0) {
        strcpy(char_string, "0");
    } else {
        int digit, m, m1;
        char *c = char_string;
        int neg = (number < 0);
        if (neg)
            number = -number;
        // calculate magnitude
        m = log10(number);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            number = number / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (number > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(number / weight);
                number -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && number > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return char_string;
}




