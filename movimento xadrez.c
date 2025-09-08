// Desafio: Movimentando as Peças do Xadrez (C)
// Autor: você 😉
// Build (Linux/macOS):  gcc -O2 -std=c11 src/chess_moves.c -o chess
// Build (Windows MinGW): gcc -O2 -std=c11 src\chess_moves.c -o chess.exe

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define BOARD_N 8

typedef enum { WHITE=0, BLACK=1, NO_COLOR=2 } Color;
typedef enum {
    EMPTY=0, PAWN='P', KNIGHT='N', BISHOP='B',
    ROOK='R', QUEEN='Q', KING='K'
} PieceKind;

typedef struct { PieceKind kind; Color color; } Piece;

static Piece board[BOARD_N][BOARD_N];
static Color turn = WHITE;

/* ---------- Util ---------- */
static bool in_bounds(int r, int c){ return r>=0 && r<BOARD_N && c>=0 && c<BOARD_N; }

static char piece_symbol(Piece p){
    if(p.kind==EMPTY) return '.';
    char ch = (char)p.kind;
    return (p.color==WHITE)? ch : (char)tolower(ch);
}

static void clear_board(void){
    for(int r=0;r<BOARD_N;++r)
        for(int c=0;c<BOARD_N;++c)
            board[r][c]=(Piece){EMPTY,NO_COLOR};
}

static void setup_initial(void){
    clear_board();
    // Peões
    for(int c=0;c<BOARD_N;++c){
        board[6][c]=(Piece){PAWN,WHITE};
        board[1][c]=(Piece){PAWN,BLACK};
    }
    // Torres
    board[7][0]=(Piece){ROOK,WHITE};  board[7][7]=(Piece){ROOK,WHITE};
    board[0][0]=(Piece){ROOK,BLACK};  board[0][7]=(Piece){ROOK,BLACK};
    // Cavalos
    board[7][1]=(Piece){KNIGHT,WHITE}; board[7][6]=(Piece){KNIGHT,WHITE};
    board[0][1]=(Piece){KNIGHT,BLACK}; board[0][6]=(Piece){KNIGHT,BLACK};
    // Bispos
    board[7][2]=(Piece){BISHOP,WHITE}; board[7][5]=(Piece){BISHOP,WHITE};
    board[0][2]=(Piece){BISHOP,BLACK}; board[0][5]=(Piece){BISHOP,BLACK};
    // Damas
    board[7][3]=(Piece){QUEEN,WHITE};  board[0][3]=(Piece){QUEEN,BLACK};
    // Reis
    board[7][4]=(Piece){KING,WHITE};   board[0][4]=(Piece){KING,BLACK};
}

static void print_board(void){
    printf("\n   a  b  c  d  e  f  g  h\n");
    printf("  ------------------------\n");
    for(int r=0;r<BOARD_N;++r){
        printf("%d |", 8-r);
        for(int c=0;c<BOARD_N;++c){
            printf(" %c", piece_symbol(board[r][c]));
        }
        printf(" |\n");
    }
    printf("  ------------------------\n");
    printf("   a  b  c  d  e  f  g  h\n");
    printf("Peças brancas = maiúsculas, pretas = minúsculas. '.' = vazio.\n");
}

/* algebraic square to (row,col). e.g., "e2" -> r=6,c=4 */
static bool parse_square(const char *s, int *out_r, int *out_c){
    if(!s || strlen(s)<2) return false;
    char f=tolower((unsigned char)s[0]);
    char r=s[1];
    if(f<'a'||f>'h'||r<'1'||r>'8') return false;
    int col = f - 'a';
    int row = 8 - (r - '0'); // rank 8 -> row 0
    *out_r=row; *out_c=col;
    return in_bounds(row,col);
}

/* path clear for sliders (bishop/rook/queen). excludes src, includes dst test later */
static bool path_clear_line(int r0,int c0,int r1,int c1){
    int dr = (r1>r0) - (r1<r0);  // -1,0,1
    int dc = (c1>c0) - (c1< c0);
    int r=r0+dr, c=c0+dc;
    while(r!=r1 || c!=c1){
        if(board[r][c].kind!=EMPTY) return false;
        r+=dr; c+=dc;
    }
    return true;
}

/* ---------- Movement rules (no check rules) ---------- */
static bool can_move_pawn(int r0,int c0,int r1,int c1, Color side){
    int dir = (side==WHITE)? -1 : 1; // white moves up (towards row 0)
    int start_row = (side==WHITE)? 6 : 1;

    int dr = r1 - r0;
    int dc = c1 - c0;

    Piece dst = board[r1][c1];

    // forward 1
    if(dc==0 && dr==dir && dst.kind==EMPTY) return true;

    // forward 2 from start (path must be clear)
    if(dc==0 && dr==2*dir && r0==start_row){
        int midr = r0 + dir;
        if(board[midr][c0].kind==EMPTY && dst.kind==EMPTY) return true;
    }

    // capture diagonal
    if((dc==1 || dc==-1) && dr==dir && dst.kind!=EMPTY && dst.color!=side) return true;

    // (sem en passant)
    return false;
}

static bool can_move_knight(int r0,int c0,int r1,int c1, Color side){
    int dr = r1 - r0, dc = c1 - c0;
    int adr = dr<0?-dr:dr, adc = dc<0?-dc:dc;
    if(!((adr==2 && adc==1) || (adr==1 && adc==2))) return false;
    Piece dst = board[r1][c1];
    return (dst.kind==EMPTY || dst.color!=side);
}

static bool can_move_bishop(int r0,int c0,int r1,int c1, Color side){
    int dr=r1-r0, dc=c1-c0;
    if((dr<0?-dr:dr)!=(dc<0?-dc:dc)) return false; // precisa ser diagonal
    if(!path_clear_line(r0,c0,r1,c1)) return false;
    Piece dst = board[r1][c1];
    return (dst.kind==EMPTY || dst.color!=side);
}

static bool can_move_rook(int r0,int c0,int r1,int c1, Color side){
    if(!(r0==r1 || c0==c1)) return false;
    if(!path_clear_line(r0,c0,r1,c1)) return false;
    Piece dst = board[r1][c1];
    return (dst.kind==EMPTY || dst.color!=side);
}

static bool can_move_queen(int r0,int c0,int r1,int c1, Color side){
    int dr=r1-r0, dc=c1-c0;
    bool straight = (r0==r1 || c0==c1);
    bool diag = ((dr<0?-dr:dr)==(dc<0?-dc:dc));
    if(!(straight || diag)) return false;
    if(!path_clear_line(r0,c0,r1,c1)) return false;
    Piece dst = board[r1][c1];
    return (dst.kind==EMPTY || dst.color!=side);
}

static bool can_move_king(int r0,int c0,int r1,int c1, Color side){
    int dr = r1 - r0, dc = c1 - c0;
    int adr = dr<0?-dr:dr, adc = dc<0?-dc:dc;
    if(adr>1 || adc>1) return false;
    Piece dst = board[r1][c1];
    // (não validamos estar em xeque; sem roque)
    return (dst.kind==EMPTY || dst.color!=side);
}

/* validate move of piece at src to dst (no check rules) */
static bool is_legal_move(int r0,int c0,int r1,int c1){
    if(!in_bounds(r0,c0)||!in_bounds(r1,c1)) return false;
    if(r0==r1 && c0==c1) return false;
    Piece src = board[r0][c0];
    if(src.kind==EMPTY || src.color!=turn) return false;

    Piece dst = board[r1][c1];
    if(dst.kind!=EMPTY && dst.color==turn) return false; // não pode capturar a própria

    switch(src.kind){
        case PAWN:   return can_move_pawn(r0,c0,r1,c1,src.color);
        case KNIGHT: return can_move_knight(r0,c0,r1,c1,src.color);
        case BISHOP: return can_move_bishop(r0,c0,r1,c1,src.color);
        case ROOK:   return can_move_rook(r0,c0,r1,c1,src.color);
        case QUEEN:  return can_move_queen(r0,c0,r1,c1,src.color);
        case KING:   return can_move_king(r0,c0,r1,c1,src.color);
        default:     return false;
    }
}

static void maybe_promote_pawn(int r,int c){
    Piece *p = &board[r][c];
    if(p->kind!=PAWN) return;
    if(p->color==WHITE && r==0){ p->kind=QUEEN; printf("Promoção: peão branco virou Dama!\n"); }
    if(p->color==BLACK && r==7){ p->kind=QUEEN; printf("Promoção: peão preto virou Dama!\n"); }
}

static void apply_move(int r0,int c0,int r1,int c1){
    Piece src = board[r0][c0];
    Piece dst = board[r1][c1];
    if(dst.kind!=EMPTY && dst.color!=src.color){
        printf("Captura: %c em %c%c removido.\n",
               piece_symbol(dst), 'a'+c1, '0'+(8-r1));
    }
    b
