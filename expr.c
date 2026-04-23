/*
 * ============================================================
 *  Projet DIC1 - Langage C
 *  UCAD / ESP / Département Génie Informatique
 *  Année universitaire 2025/2026
 *
 *  Analyse et évaluation d'expressions arithmétiques
 *
 *  Grammaire BNF :
 *    expression              -> terme op-additif expression | terme
 *    terme                   -> facteur op-multiplicatif terme | facteur
 *    facteur                 -> nombre | '(' expression ')'
 *    nombre                  -> chiffre nombre | chiffre
 *    chiffre                 -> '0' | '1' | ... | '9'
 *    op-additif              -> '+' | '-'
 *    op-multiplicatif        -> '*' | '/'
 *
 *  Règles d'évaluation :
 *    - Opérateurs de même priorité : évaluation droite -> gauche.
 *    - Ex : 3 + 5 * 125 / 7 - 6 + 10  =>  72  (exemple du sujet)
 *
 *  Interface :
 *    - Chaque expression se termine par '='
 *    - La session se termine par '.'
 *    - Espaces/tabulations ignorés partout sauf intérieur d'un nombre
 * ============================================================
 */

#include <stdio.h>
#include <limits.h>

#define ERREUR LONG_MIN //LONG_MAX pour les erreurs d'overflow, mais on peut aussi utiliser LONG_MIN pour les différencier des erreurs de syntaxe.

int calu;   /* caractère courant */

/* ---- prototypes ---- */
 void lire_utile(void);
 void vider_ligne(void);
 long nombre(void);
 long facteur(void);
long terme(void);
long expression(void);

/* ---- lire_utile : prochain char non-blanc (sauf \n) ---- */
void lire_utile(void)
{
    do { calu = getchar(); }
    while (calu == ' ' || calu == '\t' || calu == '\n' || calu == '\r'); 
}

/* ---- vider_ligne : consomme jusqu'a \n ou EOF ---- */
void vider_ligne(void)
{
    while (calu != '\n' && calu != EOF)
        calu = getchar();
}

/* ---- nombre : accumule les chiffres consécutifs ---- */
 long nombre(void)
{
    long val = 0;
    while (calu >= '0' && calu <= '9') {
        val = val * 10 + (calu - '0');
        calu = getchar();
    }
    /* sauter les espaces après le nombre */
    while (calu == ' ' || calu == '\t' || calu == '\n' || calu == '\r') 
        calu = getchar();
    return val;
}

/* ---- facteur : nombre | '(' expression ')' ---- */
long facteur(void)
{
    long val;
    if (calu >= '0' && calu <= '9') {
        return nombre();
    } else if (calu == '(') {
        lire_utile();
        val = expression();
        if (val == ERREUR) return ERREUR;
        if (calu != ')') {
            printf("la syntaxe de l'expression est erronee\n");
            vider_ligne();
            return ERREUR;
        }
        lire_utile();
        return val;
    } else {
        printf("la syntaxe de l'expression est erronee\n");
        vider_ligne();
        return ERREUR;
    }
}

/* ---- terme : facteur [*|/ terme] (recursion droite) ---- */
long terme(void)
{
    long g, d;
    char op;
    g = facteur();
    if (g == ERREUR) return ERREUR;
    if (calu == '*' || calu == '/') {
        op = (char)calu;
        lire_utile();
        d = terme();
        if (d == ERREUR) return ERREUR;
        if (op == '*') return g * d;
        if (d == 0) {
            printf("la syntaxe de l'expression est erronee (division par zero)\n");
            vider_ligne();
            return ERREUR;
        }
        return g / d; //facteur() / terme()
    }
    return g;
}

/* ---- expression : terme [+|- expression] (recursion droite) ---- */
long expression(void)
{
    long g, d;
    char op;
    g = terme();
    if (g == ERREUR) return ERREUR;
    if (calu == '+' || calu == '-') {
        op = (char)calu;
        lire_utile();
        d = expression();
        if (d == ERREUR) return ERREUR;
        return (op == '+') ? g + d : g - d;
    }
    return g;
}

/* ---- main ---- */
int main(void)
{
    long valeur;
    
    printf("=== Analyseur d'expressions arithmetiques ===\n");
    printf("    Terminez chaque expression par '='\n");
    printf("    Saisissez '.' pour quitter\n\n");

    for (;;) {
        printf("A toi : ");
        fflush(stdout);
        lire_utile();
        if (calu == '.')  { 
            vider_ligne(); 
            printf("Au revoir...\n"); 
            break; 
        }
        if (calu == EOF)  { 
            printf("Au revoir...\n"); 
            break; 
        }
        if (calu == '\n') { continue; }

        valeur = expression();
        if (valeur == ERREUR) { continue; }

        if (calu != '=') {
            printf("la syntaxe de l'expression est erronee\n");
            vider_ligne();
            continue;
        }
        vider_ligne();
        printf("la syntaxe de l'expression est correcte\n");
        printf("sa valeur est %ld\n", valeur);
    }
    return 0;
}
