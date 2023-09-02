#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT(i, j) ((i - 1) * number_of_nodes + j)

/*aloc memorie pentru matricea de adiacenta*/
int **aloc_memory(int number_of_nodes) {
    int **adjacent_matrix = (int **)malloc(sizeof(int *) * number_of_nodes);
    for (int i = 0; i < number_of_nodes; i++) {
        adjacent_matrix[i] = calloc(number_of_nodes, sizeof(int));
    }
    return adjacent_matrix;
}

/*citesc din fisier graful neorientat*/
int **read_from_file(char *file_name, int *number_of_nodes, int *k) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
        printf("file not found");
    char line[256];
    int line_number = 0;
    int nodes = 0;
    int **adjacent_matrix = NULL;
    
    while(fgets(line, sizeof(line), file)) {
        printf("%s", line);
        char *token = strtok(line, " \n");
        while (token != NULL) {
            if (line_number == 0) {
                *number_of_nodes = atoi(token);
                printf("%d ", *number_of_nodes);
                token = strtok(NULL, " \n");
                *k = atoi(token);
                printf("%d ", *k);
                adjacent_matrix = aloc_memory(*number_of_nodes);
                
            } else {
                nodes = atoi(token);
                /*indicii matricei incep de la 0*/
                adjacent_matrix[line_number - 1][nodes - 1] = 1;
                adjacent_matrix[nodes - 1][line_number - 1] = 1;
                
            }
            token = strtok(NULL, " \n");      
        }
        line_number ++; 
        if (line_number == *number_of_nodes)
            break;
    }
    fclose(file);
    return adjacent_matrix;
}

/*calculez numarul de noduri neadiacente*/
int non_adjacent_nodes(int **adjacent_matrix, int number_of_nodes) {
    int non_adjacent_nodes = 0;
    for (int i = 0 ; i < number_of_nodes - 1 ; i++) {
        for (int j = i + 1 ; j < number_of_nodes ; j++) {
            if (adjacent_matrix[i][j] == 0)
                non_adjacent_nodes ++;
        }
    }
    return non_adjacent_nodes;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        printf("wrong number of parameters");
    else {
        char *file_name = argv[1];
        int number_of_nodes = 0;
        int k = 0;
        int **adjacent_matrix = read_from_file(file_name, &number_of_nodes, &k);   
        int number_of_variables = number_of_nodes * k;
        int number_of_clauses = 0;
        number_of_clauses += k;
        number_of_clauses += (((k - 1) * k) / 2) * number_of_nodes;
        number_of_clauses += non_adjacent_nodes(adjacent_matrix, number_of_nodes) * ((k - 1) * k);

        /*scriu in fisierul de output SAT-ul*/
        FILE *file = fopen(argv[2], "w+");
        if (file == NULL)
            printf("file not found");
        fprintf(file, "p cnf %d %d\n", number_of_variables, number_of_clauses);

        /*SAT care asigura ca exista un "al i-lea nod" in clica
        pentru fiecare i (de la 1 la k) vom avea cate o clauza de forma
        (x1j sau x2j sau ... sau xkj), unde j = nod din graf
        deci ne asiguram ca minim o variabila va fi 1, deci exista
        cel putin un "al i-lea nod" in clica
        vom avea k clauze*/
        for (int i = 1 ; i <= k ; i++) {
            for (int j = 1 ; j <= number_of_nodes ; j++) {
                fprintf(file, "%d ", FORMAT(i, j));
            }
            fprintf(file, "0\n");
        }

        /*SAT care asigura ca toate elementele clicii sunt unice
        intr-o clauza nu pot fi ambele variabile (xiv si xjv) 1,
        deci elementele clicii vor fi unice*/
        for (int v = 1 ; v <= number_of_nodes ; v++) {
            for (int i = 1 ; i <= k - 1 ; i++) {
                for (int j = i + 1 ; j <= k ; j++) {
                    fprintf(file, "-%d -%d 0\n", FORMAT(i, v), FORMAT(j, v));
                }
            }
        }

        /*SAT care asigura ca intre oricare 2 noduri din clica exista o muchie;
        daca nu exista muchie intre 2 noduri, atunci nu pot fi amandoua in clica;
        intr-o clauza nu pot fi ambele variabile (xiv si xju / xiu si xjv) 1, deoarece ar insemna
        ca nodurile v si u ar fi in clica chiar daca nu exista muchie intre ele*/
        for (int u = 1 ; u <= number_of_nodes - 1 ; u++) {
            for (int v = u + 1 ; v <= number_of_nodes ; v++) {
                if (adjacent_matrix[u - 1][v - 1] == 0) {
                    for (int i = 1 ; i <= k - 1 ; i++) {
                        for (int j = i + 1 ; j <= k ; j++) {
                            fprintf(file, "-%d -%d 0\n", FORMAT(i, v), FORMAT(j, u));
                            fprintf(file, "-%d -%d 0\n", FORMAT(i, u), FORMAT(j, v));
                        }
                    }
                }
            }
        }
        fclose(file);
        for (int i = 0; i < number_of_nodes; i++) {
            free(adjacent_matrix[i]);
        }
        free(adjacent_matrix); 
    }
    return 0;  
}
