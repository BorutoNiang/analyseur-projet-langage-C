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
 *
 *  Architecture : parseur descendant récursif — chaque non-terminal
 *  de la grammaire correspond à une fonction C. La variable globale
 *  `calu` joue le rôle de lookahead : elle contient toujours le
 *  prochain caractère à analyser, déjà lu mais pas encore consommé.
 * ============================================================
 */

#include <stdio.h>
#include <limits.h>

/* Valeur sentinelle retournée par toutes les fonctions d'analyse pour
 * signaler une erreur (syntaxe incorrecte ou division par zéro).
 * LONG_MIN est choisi car aucune expression valide ne peut produire
 * cette valeur dans le contexte du sujet. */
#define ERREUR LONG_MIN

/* Caractère courant (lookahead), mis à jour par lire_utile(), vider_ligne()
 * et nombre(). Toutes les fonctions supposent que calu contient déjà
 * le prochain caractère significatif à examiner. */
int calu;

/* ---- prototypes ---- */
 void lire_utile(void);
 void vider_ligne(void);
 long nombre(void);
 long facteur(void);
long terme(void);
long expression(void);

/* ---- lire_utile : lit caractères jusqu'au prochain non-blanc (hors \n) ---- */
void lire_utile(void)
{
    do { calu = getchar(); }
    while (calu == ' ' || calu == '\t' || calu == '\n' || calu == '\r'); 
}

/* ---- vider_ligne : consomme tous les caractères jusqu'à \n ou EOF.
 *      Appelée après chaque erreur pour nettoyer le tampon d'entrée
 *      et permettre à la boucle principale de reprendre proprement. ---- */
void vider_ligne(void)
{
    while (calu != '\n' && calu != EOF)
        calu = getchar();
}

/* ---- nombre : lit et retourne la valeur d'un entier (suite de chiffres).
 *      Précondition : calu est un chiffre '0'..'9'.
 *      Postcondition : calu pointe sur le premier caractère non chiffre
 *      et non espace qui suit le nombre. ---- */
 long nombre(void)
{
    long val = 0;

    /* Accumulation chiffre par chiffre : val = val*10 + chiffre */
    while (calu >= '0' && calu <= '9') {
        val = val * 10 + (calu - '0');
        calu = getchar();
    }
    /* Sauter les espaces/tabulations après le nombre pour que calu
     * pointe sur le prochain token (opérateur, '=', ')', etc.) */
    while (calu == ' ' || calu == '\t' || calu == '\n' || calu == '\r') 
        calu = getchar();
    return val;
}

/* ---- facteur : analyse   nombre | '(' expression ')'
 *      Retourne la valeur du facteur, ou ERREUR en cas d'échec. ---- */
long facteur(void)
{
    long val;
    if (calu >= '0' && calu <= '9') {
        /* Cas 1 : facteur est un nombre entier */
        return nombre();
    } else if (calu == '(') {
        lire_utile();               /* consommer '(' et avancer */
        val = expression();         /* analyser la sous-expression */
        if (val == ERREUR) return ERREUR;
        if (calu != ')') {          /* le ')' fermant est obligatoire */
            printf("la syntaxe de l'expression est erronee\n");
            vider_ligne();
            return ERREUR;
        }
        lire_utile();               /* consommer ')' et avancer */
        return val;
    } else {
        /* Cas 3 : caractère inattendu, ni chiffre ni '(' */
        printf("la syntaxe de l'expression est erronee\n");
        vider_ligne();
        return ERREUR;
    }
}

/* ---- terme : analyse   facteur [*|/ terme]
 *      La récursion droite produit une évaluation de droite vers la gauche
 *      pour les opérateurs de même priorité (*  et /).
 *      Retourne la valeur du terme, ou ERREUR en cas d'échec. ---- */
long terme(void)
{
    long g, d;
    char op;

    g = facteur();
    if (g == ERREUR) return ERREUR;

    if (calu == '*' || calu == '/') {
        op = (char)calu;            /* mémoriser l'opérateur */
        lire_utile();               /* consommer l'opérateur */
        d = terme();                /* récursion droite sur le reste */
        if (d == ERREUR) return ERREUR;
        if (op == '*') return g * d;
        if (d == 0) {               /* interdire la division par zéro */
            printf("la syntaxe de l'expression est erronee (division par zero)\n");
            vider_ligne();
            return ERREUR;
        }
        return g / d; //facteur() / terme()
    }
    return g;
}

/* ---- expression : analyse   terme [+|- expression]
 *      La récursion droite produit une évaluation de droite vers la gauche
 *      pour les opérateurs de même priorité (+ et -).
 *      Retourne la valeur de l'expression, ou ERREUR en cas d'échec. ---- */
long expression(void)
{
    long g, d;
    char op;

    g = terme();
    if (g == ERREUR) return ERREUR;

    if (calu == '+' || calu == '-') {
        op = (char)calu;            /* mémoriser l'opérateur */
        lire_utile();               /* consommer l'opérateur */
        d = expression();           /* récursion droite sur le reste */
        if (d == ERREUR) return ERREUR;
        return (op == '+') ? g + d : g - d;
    }
    return g;
}

/* ---- main : boucle principale — une itération par expression saisie ---- */
int main(void)
{
    long valeur;
    
    printf("=== Analyseur d'expressions arithmetiques ===\n");
    printf("    Terminez chaque expression par '='\n");
    printf("    Saisissez '.' pour quitter\n\n");

    for (;;) {
        printf("A toi : ");
        fflush(stdout);             /* forcer l'affichage avant getchar() */
        lire_utile();               /* lire le premier token de la ligne */

        if (calu == '.')  { 
            vider_ligne();          /* consommer le reste de la ligne */
            printf("Au revoir...\n"); 
            break; 
        }
        if (calu == EOF)  {         /* fin de fichier (ex. redirection) */
            printf("Au revoir...\n"); 
            break; 
        }
        if (calu == '\n') { continue; } /* ligne vide : réafficher l'invite */

        /* Analyser et évaluer l'expression ; en cas d'erreur, expression()
         * affiche le message et retourne ERREUR. */
        valeur = expression();
        if (valeur == ERREUR) { continue; }

        /* Après l'expression, le prochain caractère doit être '=' */
        if (calu != '=') {
            printf("la syntaxe de l'expression est erronee\n");
            vider_ligne();
            continue;
        }
        vider_ligne();              /* consommer le reste après '=' */
        printf("la syntaxe de l'expression est correcte\n");
        printf("sa valeur est %ld\n", valeur);
    }
    return 0;
}
