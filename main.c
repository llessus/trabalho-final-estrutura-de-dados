#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "expressao.h"

#define MAX_SIZE 512 

int main() {
    char expressao[MAX_SIZE];
    int opcao;

    do {
        printf("\n=== AVALIADOR DE EXPRESSOES ===\n");
        printf("1) Converter INFIXA -> POSFIXA\n");
        printf("2) Converter POSFIXA -> INFIXA\n");
        printf("3) Calcular valor de expressao INFIXA\n");
        printf("4) Calcular valor de expressao POSFIXA\n");
        printf("0) Sair\n");
        printf("Escolha uma opcao: ");

        // Loop para garantir entrada de número válido
        while (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida. Por favor, digite um numero: ");
            while (getchar() != '\n'); // Limpa o buffer de entrada
        }
        getchar(); // consumir o ENTER que fica no buffer após scanf

        char* resultado_str = NULL; // Usado para armazenar resultados de string que precisam ser liberados

        switch(opcao) {
            case 1:
                printf("Digite a expressao INFIXA (com espacos entre operadores e operandos): ");
                fgets(expressao, MAX_SIZE, stdin);
                expressao[strcspn(expressao, "\n")] = '\0';
                
                resultado_str = getFormaPosFixa(expressao);
                printf("Forma POSFIXA: %s\n", resultado_str);
            
                break;

            case 2:
                printf("Digite a expressao POSFIXA (com espacos entre tokens): ");
                fgets(expressao, MAX_SIZE, stdin);
                expressao[strcspn(expressao, "\n")] = '\0';
                
                resultado_str = getFormaInFixa(expressao);
                printf("Forma INFIXA: %s\n", resultado_str);
                // ATENÇÃO: REMOVA a linha `free(resultado_str);` se estiver usando a versão de expressao.c com `static char result[]`.
                break;

            case 3:
                printf("Digite a expressao INFIXA (com espacos entre operadores e operandos): ");
                fgets(expressao, MAX_SIZE, stdin);
                expressao[strcspn(expressao, "\n")] = '\0';
                printf("Valor calculado: %.4f\n", getValorInFixa(expressao));
                break;

            case 4:
                printf("Digite a expressao POSFIXA (com espacos entre tokens): ");
                fgets(expressao, MAX_SIZE, stdin);
                expressao[strcspn(expressao, "\n")] = '\0';
                printf("Valor calculado: %.4f\n", getValorPosFixa(expressao));
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida. Por favor, escolha uma opcao valida.\n");
        }

    } while(opcao != 0);

    return 0;
}
