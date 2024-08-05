#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define ld long double
typedef struct {
    int row;
    int col;
    int common_dim;
    ld **matrixA;
    ld **matrixB;
    ld **result;
} ThreadData;

void *multiply_element(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    ld sum = 0;
    for (int k = 0; k < data->common_dim; k++) {
        sum += data->matrixA[data->row][k] * data->matrixB[k][data->col];
    }
    data->result[data->row][data->col] = sum;
    pthread_exit(0);
}

int main() {
    int l1, c1, l2, c2;

    // Read the dimensions of the matrices

    if (scanf("%d %d", &l1, &c1) != 2) {
        fprintf(stderr, "Error reading matrix dimensions.\n");
        return EXIT_FAILURE;
    }

    // Allocate memory for matrix A

    ld **matrixA = (ld **)malloc(l1 * sizeof(ld *));
    for (int i = 0; i < l1; i++) {
        matrixA[i] = (ld *)malloc(c1 * sizeof(ld));
    }

    // Read matrix A

    for (int i = 0; i < l1; i++) {
        for (int j = 0; j < c1; j++) {
            if (scanf("%Le", &matrixA[i][j]) != 1) {
                fprintf(stderr, "Error reading matrix A.\n");
                return EXIT_FAILURE;
            }
        }
    }

    // Read the dimensions of the second matrix

    if (scanf("%d %d", &l2, &c2) != 2) {
        fprintf(stderr, "Error reading matrix dimensions.\n");
        return EXIT_FAILURE;
    }

    // Allocate memory for matrix B

    ld **matrixB = (ld **)malloc(l2 * sizeof(ld *));
    for (int i = 0; i < l2; i++) {
        matrixB[i] = (ld *)malloc(c2 * sizeof(ld));
    }

    // Read matrix B

    for (int i = 0; i < l2; i++) {
        for (int j = 0; j < c2; j++) {
            if (scanf("%Le", &matrixB[i][j]) != 1) {
                fprintf(stderr, "Error reading matrix B.\n");
                return EXIT_FAILURE;
            }
        }
    }

    // Check if multiplication is possible

    if (c1 != l2) {
        printf("Nao e possivel multiplicar\n");
        return EXIT_SUCCESS;
    }

    // Allocate memory for result matrix

    ld **result = (ld **)malloc(l1 * sizeof(ld *));
    for (int i = 0; i < l1; i++) {
        result[i] = (ld *)malloc(c2 * sizeof(ld));
    }

    // Fork to create a new process for matrix multiplication

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Child process
        pthread_t threads[l1 * c2];
        ThreadData thread_data[l1 * c2];
        int thread_count = 0;

        // Create threads to calculate each element of the result matrix

        for (int i = 0; i < l1; i++) {
            for (int j = 0; j < c2; j++) {
                thread_data[thread_count].row = i;
                thread_data[thread_count].col = j;
                thread_data[thread_count].common_dim = c1;
                thread_data[thread_count].matrixA = matrixA;
                thread_data[thread_count].matrixB = matrixB;
                thread_data[thread_count].result = result;
                pthread_create(&threads[thread_count], NULL, multiply_element, &thread_data[thread_count]);
                thread_count++;
            }
        }

        // Join threads
        for (int i = 0; i < thread_count; i++) {
            pthread_join(threads[i], NULL);
        }

        // Print the result matrix

        for (int i = 0; i < l1; i++) {
            for (int j = 0; j < c2; j++) {
                printf("%7.2Lf ", result[i][j]);
            }
            printf("\n");
        }
        exit(EXIT_SUCCESS);
    } else { // Parent process
        wait(NULL); // Wait for the child process to finish
    }

    // Free allocated memory

    for (int i = 0; i < l1; i++) {
        free(matrixA[i]);
    }
    free(matrixA);

    for (int i = 0; i < l2; i++) {
        free(matrixB[i]);
    }
    free(matrixB);

    for (int i = 0; i < l1; i++) {
        free(result[i]);
    }
    free(result);

    return EXIT_SUCCESS;
}
