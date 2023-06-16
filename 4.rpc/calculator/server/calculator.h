#ifndef CALCULATOR_H_INCLUDED
#define CALCULATOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calculator(int operand1, int operand2, char operator) {
    int result;

    switch (operator) {
        case '-':
            result = operand1 - operand2;
            break;
        case '+':
            result = operand1 + operand2;
            break;
        case '/':
            if(operand2 == 0){
                result = 0;
                break;
            }
            result = operand1 / operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        default:
            printf("Invalid operator\n");
            return 0;
    }

    return result;
}

#endif // CALCULATOR_H_INCLUDED
