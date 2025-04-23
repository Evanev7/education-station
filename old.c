// malloc(size) -> pointer to [size] bytes of allocated memory
// free(ptr) -> deletes data at that pointer
// type x[] -> makes an array on the stack
// type *x -> pointer to an object of type
// These are the same!


void square() {
    int x; //Initialises variable
    printf("Provide a number \n");
    scanf_s("%d", &x);
    printf("The square is %d\n", x*x);
    return;
}

void fibber() {
    // x[3] = 4
    int f = 1;
    for (int i = 0; i < 10;) {
        if (1) {
            printf("%d, ", i);
        } 
        else {
            printf("%d", i);          
        }
        int temp = i;
        i = f;
        f = f + temp;
    }
    printf("\n");
}

// Create List that's Fibber -> Add value to Fibber -> Update value + update what value of fibber you're addingt -> add new value to Fibber
// int* fibber2(int num);

// Create num which is how long the list should be
// Get how long the list should be
// Create list based on how long it should be
// 
// Create first value in fibber sequence, plus the first value required to increment it.
// 
// For Loop: Create a where the position on the list increments until it reaches the desired length
// Add the fibber value to the list
// Define temp to allow incrementation
// Increment the value -> return to beginning of for loop
// 
// Print the list
// Free the malloc

int* fibber2(int num) {
    /*
    int num;
    printf("Please input a number\n");
    scanf_s ("%d", &num);
    */
    int* answer = malloc(num * sizeof(int));
    int i = 0;
    int f = 1;
    for (int p = 0; p < num; p++) {
        answer[p] = i;
        int temp = i;
        i = f;
        f = f + temp;
    }
    
    printf("[");
    for (int i = 0; i < num-1; i++) {
        printf("%d, ", answer[i]);
    }
    printf("%d]\n", answer[num-1]);
    return answer;
}

int oldish_main() {
    int* answer = fibber2(1);
    free(answer);
    int* (*fun)(int) = &fibber2;
    int* answer2 = (*fun)(14);
    free(answer2);
    exit(0);
}

int old_main() {
    printf("Hello World!\n");
    fibber();
    return 0;
}

/* 
    i, f
    temp = i
    i <- f
    f <- f + temp
    char: 0, 1, 2, ..., 127, -128, -127, -126, ..., -2, -1
*/