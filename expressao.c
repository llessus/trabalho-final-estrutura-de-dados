#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "expressao.h"

#define MAX_EXPRESSAO_LEN 512 
#define PI 3.14159265

// ===== Pilha de nÃºmeros (operands) =====
typedef struct No {
    float valor;
    struct No *prox;
} No;

typedef struct {
    No *topo;
} PilhaNum;

void inicializarNum(PilhaNum *p) {
    p->topo = NULL;
}

int vaziaNum(PilhaNum *p) {
    return p->topo == NULL;
}

void empilharNum(PilhaNum *p, float v) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria (PilhaNum)!\n");
        exit(EXIT_FAILURE);
    }
    novo->valor = v;
    novo->prox = p->topo;
    p->topo = novo;
}

float desempilharNum(PilhaNum *p) {
    if (vaziaNum(p)) {
        fprintf(stderr, "Pilha de numeros vazia! Expressao invalida.\n");
        exit(EXIT_FAILURE);
    }
    No *temp = p->topo;
    float v = temp->valor;
    p->topo = temp->prox;
    free(temp);
    return v;
}

// ===== Pilha de caracteres (operadores para Infixa -> Posfixa) =====
typedef struct NoChar {
    char operador;
    struct NoChar *prox;
} NoChar;

typedef struct {
    NoChar *topo;
} PilhaChar;

void inicializarChar(PilhaChar *p) {
    p->topo = NULL;
}

int vaziaChar(PilhaChar *p) {
    return p->topo == NULL;
}

void empilharChar(PilhaChar *p, char op) {
    NoChar *novo = (NoChar*) malloc(sizeof(NoChar));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria (PilhaChar)!\n");
        exit(EXIT_FAILURE);
    }
    novo->operador = op;
    novo->prox = p->topo;
    p->topo = novo;
}

char desempilharChar(PilhaChar *p) {
    if (vaziaChar(p)) {
        fprintf(stderr, "Pilha de operadores vazia! Expressao invalida ou erro de logica.\n");
        exit(EXIT_FAILURE);
    }
    NoChar *temp = p->topo;
    char op = temp->operador;
    p->topo = temp->prox;
    free(temp);
    return op;
}

char topoChar(PilhaChar *p) {
    if (vaziaChar(p)) {
        return '\0'; // Indica pilha vazia
    }
    return p->topo->operador;
}

// ===== Pilha de strings (para Posfixa -> Infixa) =====
typedef struct NoS {
    char *str;
    struct NoS *prox;
} NoS;

typedef struct {
    NoS *topo;
} PilhaStr;

void inicializarStr(PilhaStr *p) {
    p->topo = NULL;
}

int vaziaStr(PilhaStr *p) {
    return p->topo == NULL;
}

void empilharStr(PilhaStr *p, char *s) {
    NoS *novo = (NoS*) malloc(sizeof(NoS));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria (PilhaStr)!\n");
        exit(EXIT_FAILURE);
    }
    novo->str = strdup(s); // Duplica a string para alocar memÃ³ria
    if (novo->str == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria (strdup - PilhaStr)!\n");
        free(novo);
        exit(EXIT_FAILURE);
    }
    novo->prox = p->topo;
    p->topo = novo;
}

char *desempilharStr(PilhaStr *p) {
    if (vaziaStr(p)) {
        fprintf(stderr, "Pilha de string vazia! Expressao invalida.\n");
        exit(EXIT_FAILURE);
    }
    NoS *temp = p->topo;
    char *s = temp->str;
    p->topo = temp->prox;
    free(temp);
    return s; // Retorna a string desempilhada, o chamador deve liberar esta memÃ³ria
}

// ===== Verifica prioridade =====
int prioridade(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    // FunÃ§Ãµes unÃ¡rias e potÃªncia terÃ£o a maior prioridade para desempilhamento na conversÃ£o
    if (op == '^' || op == 'l' || op == 's' || op == 'c' || op == 't' || op == 'r') return 3; // 'l' for log, 's' for sen, etc.
    return 0; // Para parÃªnteses e outros
}

// Verifica se Ã© um operador de duas vias
int isOperadorBinario(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^');
}

// FunÃ§Ã£o auxiliar para obter o nome da funÃ§Ã£o a partir do caractere
const char* getFunctionName(char op_char) {
    switch (op_char) {
        case 'l': return "log";
        case 's': return "sen";
        case 'c': return "cos";
        case 't': return "tg";
        case 'r': return "raiz";
        default: return NULL;
    }
}


// ======= FunÃ§Ã£o 1: Infixa -> PÃ³s-Fixa =======
// Adicionado suporte para funÃ§Ãµes unÃ¡rias (sen, cos, tg, log, raiz)
char *getFormaPosFixa(char *Str) {

    char *pos = (char*) malloc(MAX_EXPRESSAO_LEN * 2); // Pode ser maior que a infixa
    if (pos == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para posfixa!\n");
        exit(EXIT_FAILURE);
    }
    pos[0] = '\0';

    PilhaChar operadores;
    inicializarChar(&operadores);

    int i = 0;
    char token[MAX_EXPRESSAO_LEN];
    int token_idx = 0;

    // while (Str[i] == ' ') i++;

    while (Str[i] != '\0') {
        char c = Str[i];

        if (c == ' ') {
            i++;
            continue;
        }

        if ((c >= '0' && c <= '9') || c == '.') {
           
            token_idx = 0;
            while ((Str[i] >= '0' && Str[i] <= '9') || Str[i] == '.') {
                token[token_idx++] = Str[i++];
            }
            token[token_idx] = '\0';
            strcat(pos, token);
            strcat(pos, " ");
            continue; // JÃ¡ incrementou i no loop interno
        } else if (c == '(') {
            empilharChar(&operadores, c);
        } else if (c == ')') {
            while (!vaziaChar(&operadores) && topoChar(&operadores) != '(') {
                char op = desempilharChar(&operadores);
                const char* func_name = getFunctionName(op);
                if (func_name) { // Se for uma funÃ§Ã£o unÃ¡ria
                    strcat(pos, func_name);
                } else {
                    char op_str[2] = {op, '\0'};
                    strcat(pos, op_str);
                }
                strcat(pos, " ");
            }
            if (!vaziaChar(&operadores) && topoChar(&operadores) == '(') {
                desempilharChar(&operadores); // descarta '('
            } else {
                fprintf(stderr, "Erro: ParÃªnteses desbalanceados ou operadores inesperados.\n");
                free(pos);
                exit(EXIT_FAILURE);
            }
        } else if (isOperadorBinario(c)) { // Operadores binÃ¡rios: +, -, *, /, %, ^
            while (!vaziaChar(&operadores) && prioridade(topoChar(&operadores)) >= prioridade(c)) {
                char op = desempilharChar(&operadores);
                const char* func_name = getFunctionName(op);
                if (func_name) {
                    strcat(pos, func_name);
                } else {
                    char op_str[2] = {op, '\0'};
                    strcat(pos, op_str);
                }
                strcat(pos, " ");
            }
            empilharChar(&operadores, c);
        } else if (strncmp(&Str[i], "log", 3) == 0) {
            empilharChar(&operadores, 'l'); // 'l' representa log
            i += 2; // AvanÃ§a 2 caracteres ('o', 'g') para o prÃ³ximo loop
        } else if (strncmp(&Str[i], "sen", 3) == 0) {
            empilharChar(&operadores, 's'); // 's' representa sen
            i += 2;
        } else if (strncmp(&Str[i], "cos", 3) == 0) {
            empilharChar(&operadores, 'c'); // 'c' representa cos
            i += 2;
        } else if (strncmp(&Str[i], "tg", 2) == 0) {
            empilharChar(&operadores, 't'); // 't' representa tg
            i += 1;
        } else if (strncmp(&Str[i], "raiz", 4) == 0) {
            empilharChar(&operadores, 'r'); // 'r' representa raiz
            i += 3;
        } else {
            fprintf(stderr, "Caractere ou token desconhecido na expressao infixa: %c\n", c);
            free(pos);
            exit(EXIT_FAILURE);
        }
        i++;
    }

    while (!vaziaChar(&operadores)) {
        char op = desempilharChar(&operadores);
        if (op == '(') {
            fprintf(stderr, "Erro: ParÃªnteses desbalanceados na expressao infixa.\n");
            free(pos);
            exit(EXIT_FAILURE);
        }
        const char* func_name = getFunctionName(op);
        if (func_name) {
            strcat(pos, func_name);
        } else {
            char op_str[2] = {op, '\0'};
            strcat(pos, op_str);
        }
        strcat(pos, " ");
    }
    
    // Remover o espaÃ§o extra final, se houver
    if (strlen(pos) > 0 && pos[strlen(pos)-1] == ' ') {
        pos[strlen(pos)-1] = '\0';
    }

    return pos;
}


// ======= FunÃ§Ã£o 2: PÃ³s-Fixa -> Infixa (bÃ¡sica, com melhoria nos parÃªnteses) =======
// Esta funÃ§Ã£o Ã© mais complexa de ser perfeita sem um analisador de precedÃªncia
// Ela vai gerar parÃªnteses em excesso para garantir correÃ§Ã£o.
char *getFormaInFixa(char *Str) {
    PilhaStr p;
    inicializarStr(&p);

    char *str_copy = strdup(Str); 
    if (str_copy == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para copia de string!\n");
        exit(EXIT_FAILURE);
    }
    char *token = strtok(str_copy, " ");

    while (token != NULL) {
        if ((token[0] >= '0' && token[0] <= '9') || (token[0] == '-' && strlen(token) > 1 && (token[1] >= '0' && token[1] <= '9')) || token[0] == '.') {
            // Ã‰ um nÃºmero (incluindo negativos)
            empilharStr(&p, token);
        } else if (strlen(token) == 1 && isOperadorBinario(token[0])) { // Operadores binÃ¡rios
            if (vaziaStr(&p)) { fprintf(stderr, "Erro: Pilha vazia para operador binÃ¡rio em PosFixa -> Infixa.\n"); free(str_copy); exit(EXIT_FAILURE); }
            char *b = desempilharStr(&p);
            if (vaziaStr(&p)) { fprintf(stderr, "Erro: Pilha vazia para segundo operando em PosFixa -> Infixa.\n"); free(b); free(str_copy); exit(EXIT_FAILURE); }
            char *a = desempilharStr(&p);

            char res[MAX_EXPRESSAO_LEN]; // Buffer temporÃ¡rio
            snprintf(res, MAX_EXPRESSAO_LEN, "(%s %c %s)", a, token[0], b);
            
            free(a); free(b); // Libera as strings desempilhadas
            empilharStr(&p, res);
        } else { // Deve ser uma funÃ§Ã£o unÃ¡ria
            if (vaziaStr(&p)) { fprintf(stderr, "Erro: Pilha vazia para funÃ§Ã£o unÃ¡ria em PosFixa -> Infixa.\n"); free(str_copy); exit(EXIT_FAILURE); }
            char *arg = desempilharStr(&p);

            char res[MAX_EXPRESSAO_LEN]; // Buffer temporÃ¡rio
            snprintf(res, MAX_EXPRESSAO_LEN, "%s(%s)", token, arg);
            
            free(arg); // Libera a string desempilhada
            empilharStr(&p, res);
        }
        token = strtok(NULL, " ");
    }
    free(str_copy); // Libera a cÃ³pia da string original

    if (vaziaStr(&p)) { fprintf(stderr, "Erro: Expressao PosFixa vazia ou invalida.\n"); exit(EXIT_FAILURE); }
    char *result = desempilharStr(&p);
    
    // Se a expressÃ£o final for (X), remove os parÃªnteses externos desnecessÃ¡rios
    if (result[0] == '(' && result[strlen(result) - 1] == ')') {
        int balance = 0;
        int i;
        for (i = 1; i < strlen(result) - 1; i++) {
            if (result[i] == '(') balance++;
            else if (result[i] == ')') balance--;
            if (balance < 0) break; // Significa que o ')' interno fechou algo antes do final
        }
        if (balance == 0) { // Se o balanceamento for 0, os parÃªnteses externos sÃ£o os do resultado final
            char *temp = strdup(result + 1); // Copia a string sem o '(' inicial
            temp[strlen(temp) - 1] = '\0'; // Remove o ')' final
            free(result);
            result = temp;
        }
    }


    return result; 
}


// ======= FunÃ§Ã£o 3: Avaliar PÃ³s-Fixa =======
// Adicionado suporte para funÃ§Ãµes unÃ¡rias
float getValorPosFixa(char *StrPosFixa) {
    PilhaNum p;
    inicializarNum(&p);

    char *str_copy = strdup(StrPosFixa); // Copia para nÃ£o modificar o original
    if (str_copy == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para copia de string!\n");
        exit(EXIT_FAILURE);
    }
    char *token = strtok(str_copy, " ");

    while (token != NULL) {
        if ((token[0] >= '0' && token[0] <= '9') || (token[0] == '-' && strlen(token) > 1 && (token[1] >= '0' && token[1] <= '9')) || token[0] == '.') {
            // Ã‰ um nÃºmero (incluindo negativos)
            empilharNum(&p, atof(token));
        } else if (strlen(token) == 1 && isOperadorBinario(token[0])) { // Operadores binÃ¡rios
            if (vaziaNum(&p)) { fprintf(stderr, "Erro: Pilha vazia para operador binÃ¡rio '%c'.\n", token[0]); free(str_copy); exit(EXIT_FAILURE); }
            float b = desempilharNum(&p);
            if (vaziaNum(&p)) { fprintf(stderr, "Erro: Pilha vazia para segundo operando de '%c'.\n", token[0]); free(str_copy); exit(EXIT_FAILURE); }
            float a = desempilharNum(&p);
            
            switch (token[0]) {
                case '+': empilharNum(&p, a + b); break;
                case '-': empilharNum(&p, a - b); break;
                case '*': empilharNum(&p, a * b); break;
                case '/': 
                    if (b == 0) { fprintf(stderr, "Erro: Divisao por zero.\n"); free(str_copy); exit(EXIT_FAILURE); }
                    empilharNum(&p, a / b); 
                    break;
                case '%': empilharNum(&p, fmod(a, b)); break;
                case '^': empilharNum(&p, pow(a, b)); break;
                default:
                    fprintf(stderr, "Operador binario desconhecido: %s\n", token);
                    free(str_copy);
                    exit(EXIT_FAILURE);
            }
        } else { // FunÃ§Ãµes unÃ¡rias (log, sen, cos, tg, raiz)
            if (vaziaNum(&p)) { fprintf(stderr, "Erro: Pilha vazia para funcao unaria '%s'.\n", token); free(str_copy); exit(EXIT_FAILURE); }
            float val = desempilharNum(&p);

            if (strcmp(token, "log") == 0) {
                if (val <= 0) { fprintf(stderr, "Erro: Logaritmo de numero nao positivo (%f).\n", val); free(str_copy); exit(EXIT_FAILURE); }
                empilharNum(&p, log10(val));
            } else if (strcmp(token, "sen") == 0) {
                empilharNum(&p, sin(val * PI / 180.0)); // Converte graus para radianos 
            } else if (strcmp(token, "cos") == 0) {
                empilharNum(&p, cos(val * PI / 180.0)); // Converte graus para radianos 
            } else if (strcmp(token, "tg") == 0) {
                // Verificar casos de tangente de 90+k*180
                double rad_val = val * PI / 180.0;
                if (fmod(val, 90.0) == 0 && fmod(val / 90.0, 2.0) != 0) { // Multiplos impares de 90 (90, 270, etc.)
                    fprintf(stderr, "Erro: Tangente de %f graus e indefinida.\n", val);
                    free(str_copy);
                    exit(EXIT_FAILURE);
                }
                empilharNum(&p, tan(rad_val));
            } else if (strcmp(token, "raiz") == 0) {
                if (val < 0) { fprintf(stderr, "Erro: Raiz quadrada de numero negativo (%f).\n", val); free(str_copy); exit(EXIT_FAILURE); }
                empilharNum(&p, sqrt(val));
            } else {
                fprintf(stderr, "Funcao unaria desconhecida: %s\n", token);
                free(str_copy);
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " ");
    }
    free(str_copy); // Libera a cÃ³pia da string

    if (vaziaNum(&p)) { fprintf(stderr, "Erro: Expressao PosFixa invalida, nenhum valor final.\n"); exit(EXIT_FAILURE); }
    float resultado = desempilharNum(&p);
    if (!vaziaNum(&p)) { fprintf(stderr, "Erro: Expressao PosFixa invalida, operandos restantes na pilha.\n"); exit(EXIT_FAILURE); }
    return resultado;
}

// ======= FunÃ§Ã£o 4: Avaliar Infixa =======
float getValorInFixa(char *StrInFixa) {
    char *pos = getFormaPosFixa(StrInFixa); // Converte para posfixa primeiro
    float v = getValorPosFixa(pos);         // Avalia a expressao posfixa
    free(pos); // Libera a memoria alocada por getFormaPosFixa
    return v;
}
