#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/* Acu tkhedmed dagi :) !!?
   
   La grammaire :
   
    S-> S+S | S-S | S*S | S/S | (S) | id | const
  
   Apres l'elimination de recursivite a gauche :
  
    S-> T I
    I-> + T I / - T I / epsilon
    T-> F H
    H-> * F H | / F H
    F-> (S) / id / const 

*/


typedef struct Element {
    char val;
    struct Element *next;
} Element;

typedef Element* Stack;

// Stack functions
bool is_empty(Stack stack) {
    return stack == NULL;
}

char top(Stack stack) {
    return is_empty(stack) ? '\0' : stack->val;
}

void push(Stack *stack, char val) {
    Stack new_elem = (Stack)malloc(sizeof(Element));
    if (!new_elem) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_elem->val = val;
    new_elem->next = *stack;
    *stack = new_elem;
}

char pop(Stack *stack) {
    if (is_empty(*stack)) return '\0';
    char val = (*stack)->val;
    Stack temp = *stack;
    *stack = (*stack)->next;
    free(temp);
    return val;
}

// Helper function to print stack contents (for debugging)
void print_stack(Stack stack) {
    printf("Stack: ");
    while (stack) {
        printf("%c ", stack->val);
        stack = stack->next;
    }
    printf("\n");
}

// Grammar rules implemented as parsing actions
void apply_rule(Stack *stack, char non_terminal, char input) {
    pop(stack); // Remove the non-terminal being processed

    // Grammar rules
    switch (non_terminal) {
        case 'S':  // S -> T I
            push(stack, 'I'); // Push I first, then T
            push(stack, 'T');
            break;
        case 'I':  // I -> + T I | - T I | epsilon
            if (input == '+') {
                push(stack, 'I');
                push(stack, 'T');
                push(stack, '+');
            } else if (input == '-') {
                push(stack, 'I');
                push(stack, 'T');
                push(stack, '-');
            } // epsilon: do nothing
            break;
        case 'T':  // T -> F H
            push(stack, 'H'); // Push H first, then F
            push(stack, 'F');
            break;
        case 'H':  // H -> * F H | / F H | epsilon
            if (input == '*') {
                push(stack, 'H');
                push(stack, 'F');
                push(stack, '*');
            } else if (input == '/') {
                push(stack, 'H');
                push(stack, 'F');
                push(stack, '/');
            } // epsilon: do nothing
            break;
        case 'F':  // F -> ( S ) | id/const
            if (input == '(') {
                push(stack, ')');
                push(stack, 'S');
                push(stack, '(');
            } else {
                push(stack, input);  // 'i' represents id/const
            }
            break;
    }
}

// Validation: Determines input type
bool is_terminal(char c) {
    return isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '$';
}

// Main parser function
bool parse(const char *input) {
    Stack stack = NULL;

    // Initialize stack with the starting rule
    push(&stack, '$');  // End marker
    push(&stack, 'S');  // Start symbol

    int i = 0;
    while (!is_empty(stack)) {
        print_stack(stack);

        char stack_top = top(stack);
        char current_char = input[i];

        // Check if stack_top is a terminal
        if (is_terminal(stack_top)) {
            if (stack_top == current_char) {
                pop(&stack);  // Consume terminal
                i++;          // Move to next input character
            } else {
                printf("Error: Terminal mismatch. Expected '%c', found '%c'\n", stack_top, current_char);
                return false;
            }
        } else {
            // Apply grammar rule for the non-terminal
            apply_rule(&stack, stack_top, current_char);
        }
    }

    // If stack is empty and input is fully consumed, parsing is successful
    return input[i] == '\0';
}

int main() {
    const char *input = "(5)$";

    if (parse(input)) {
        printf("Parsing successful!\n");
    } else {
        printf("Parsing failed!\n");
    }

    return 0;
}
