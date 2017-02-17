#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define GAMELENGTH 8

/////////////////////////////////////////////////////////////////////
// some structures
////////////////////////////////////////////////////////////////////

static struct move myMove;
static int win = 0;
static int winp = 0;
static int PLAYER, MASTER;

enum colour {
  violet, blue, green, pink, orange, yellow, none, red, white, wrong
};

struct move {
  int moves[4];
};

struct ret {
  int rets[4];
};

struct chain{
  struct move mov;
  struct chain *next;
};


///////////////////////////////////////////////////////////////////
// general helpers
///////////////////////////////////////////////////////////////////

enum colour match(char a){
  enum colour res;
  switch (a){
  case 'v': res = violet;
    break;
  case 'b': res = blue;
    break;
  case 'g': res = green;
    break;
  case 'p': res = pink;
    break;
  case 'o': res = orange;
    break;
  case 'y': res = yellow;
    break;
  case 'w': res = white;
    break;
  case 'r': res = red;
    break;
  case '_': res = wrong;
    break;
  default: res = none;
  }
  if (PLAYER && (res == violet || res == blue || res == green || res == pink || res == orange || res == yellow || res == none)){
    printf ("choose from: r, w, _\n");
  }
  else if (MASTER && (res == red || res == white || res == wrong || res == none)){
    printf("choose from: v, b, g, p, o, y\n");
  }    
  return res;
}

void printmv(struct move a){
  for (int i = 0; i < 4; i++){
    int r = a.moves[i];
    if (r == 0) printf("v ");
    else if (r == 1) printf("b ");
    else if (r == 2) printf("g ");
    else if (r == 3) printf("p ");
    else if (r == 4) printf("o ");
    else if (r == 5) printf("y ");
    else if (r == 6) printf(" ");
    else if (r == 7) printf("r ");
    else if (r == 8) printf("w ");
    else if (r == 9) printf("_ ");
    else printf("error, read wrong character");
  }
  printf("\n");
}

struct move getMove(){
  size_t l,m, buffersize = 30;
  char buf[buffersize];
  if (fgets(buf, buffersize, stdin) != NULL) {
    char frst, scnd, thrd, frth;
    l = sscanf(buf, "%c %c %c %c", &frst, &scnd, &thrd, &frth );
    if (l != 4){
      if (l == 3 && frst == 'a' && scnd == 'n' && thrd == 's') printmv(myMove);
      //printf("enter 4 of: v, b, g, p, v, y\n");
      return getMove();
    }
    struct move res;
    res.moves[0] = match(frst);
    res.moves[1] = match(scnd);
    res.moves[2] = match(thrd);
    res.moves[3] = match(frth);
    return res;
  }
  printf("wrong format \n");
  return getMove();
}



///////////////////////////////////////////////////////////////////
// helpers for MASTER
///////////////////////////////////////////////////////////////////

void nrCorrect(int a[], int b[], int *cor, int *pos){
  int counter = 0;
  for (int i = 0; i < 4; i++){
    if (a[i] == b[i]){
      (*cor)++;
      counter++;
      a[i] = -1;
      b[i] = -2;
    }
  }
  if (counter == 4) win = 1;
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      if (a[i] == b[j] && i != j){
	(*pos)++;
	b[j] = -2;
	a[i] = -1;
      }
    }
  }
}

struct ret answer(struct move a){
  struct ret res;
  int cor = 0, pos = 0;
  int mv[4], rt[4];
  for (int i = 0; i < 4; i++){
    mv[i] = myMove.moves[i];
    rt[i] = a.moves[i];
  }
  nrCorrect(mv, rt, &cor, &pos);
  for (int i = 0; i < 4; i++){
    if (i < cor) res.rets[i] = red;
    else if (i < cor + pos) res.rets[i] = white;
    else res.rets[i] = none;
  }
  return res;
}

struct move chooseMove(){
  struct move res;
  srand(time(NULL));
  int fact = rand();
  for (int i = 0; i < 4; i++){
    srand(time(NULL) + fact*i);
    res.moves[i] = rand() % 6;
  }
  return res;
} 

void printrt(struct ret a, int i){
  printf("%d :: ", i+1);
  for (int i = 0; i < 4; i++){
    int r = a.rets[i];
    if (r == red) printf("r ");
    else if (r == white) printf("w ");
    else if (r == none) printf("_ ");
  }
  printf("\n");
}



///////////////////////////////////////////////////////////////////
// helpers for PLAYER
///////////////////////////////////////////////////////////////////


void printChain(struct chain* head){
  while(head->next){
    printmv(head->mov);
    head = head->next;
  }
  printmv(head->mov);
}

struct chain *generateChain(){
  struct chain *init = (struct chain*) malloc(sizeof(struct chain));
  struct move a;
  a.moves[0] = 0;
  a.moves[1] = 0;
  a.moves[2] = 0;
  a.moves[3] = 0;
  init->mov = a;
  struct chain *last = init;
  for (int i = 0; i < 6; i++){
    for (int j = 0; j < 6; j++){
      for (int k = 0; k < 6; k++){
	for (int l= 0; l < 6; l++){
	  if (i == j && j == k && k == l && l == 0) continue;
	  struct chain *cur = (struct chain*) malloc(sizeof(struct chain));
	  struct move a;
	  a.moves[0] = i;
	  a.moves[1] = j;
	  a.moves[2] = k;
	  a.moves[3] = l;
	  cur->mov = a;
	  last->next = cur;
	  last = cur;
	}
      }
    }
  }
  return init;	  
}

int compatible(struct move a, struct move b, struct move ans){
  int res = 0;
  int cor = 0, pos = 0, wro = 0;
  int dif = 0, same = 0, samesame = 0;
  for (int i = 0; i < 4; i++){
    if (ans.moves[i] == red) cor++;
    else if (ans.moves[i] == white) pos++;
    else if (ans.moves[i] == wrong) wro++;
  }
  nrCorrect(a.moves, b.moves, &samesame, &same);
  dif = 4 - samesame - same;
  if (cor == samesame && pos == same && wro == dif) res = 1;
  return res;
}

int nrCompatible(struct move a, struct move b, struct chain *head){
  int res = 0;
  while(head->next != NULL){
    res += compatible(a, head->mov, b);
    head = head->next;
  }
  return res;
}

int maxCompatible(struct move a, struct chain head){
  int max = 0;
  for (int i = 7; i < 10; i++){
    for (int j = i; j < 10; j++){
      for (int k = j; k < 10; k++){
	for (int l = k; l < 10; l++){
	  if (i == 7 && j == 7 && k == 7 && l == 8) break; 
	  struct move b;
	  b.moves[0] = i;
	  b.moves[1] = j;
	  b.moves[2] = k;
	  b.moves[3] = l;
	  int res = nrCompatible(a, b, &head);
	  if (res > max) max = res;
	}
      }
    }
  }
  return max;	  
}

struct move makeGuess(struct chain *head){
  struct move candidate;
  struct chain *current = head;
  int max = 1286;
  while(current != NULL){
    int res = maxCompatible(current->mov, *head);
    if(res < max){
      candidate = current->mov;
      max = res;
    }
    if (current->next) current = current->next;
    else break;
  }
  return candidate;
}

void updatePossible(struct move a, struct move b, struct chain *head){
  struct chain *newh = (struct chain *) malloc(sizeof(struct chain));
  struct chain *res = newh;
  newh->next = head;
  while(newh->next != NULL){   
    if (!compatible(a, newh->next->mov, b)){
      struct chain *deletee = newh->next;
      if (newh->next->next){
	newh->next = newh->next->next;
	free(deletee);
      } else newh->next = NULL; 
    }
    else if (newh->next) newh = newh->next;
    else break;
  }
  *head = *res->next;
  if (!head->next) winp = 1;
}

struct move first(){
  struct move guess;
  srand(time(NULL));
  int fact = rand();
  int one = abs((rand() + fact*7) % 6);
  int two = (one + abs(rand() + fact*13) % 5) % 5;
  int pos = rand() % 4;
  int pos2 = (pos + (fact % 2 + 1)) % 4;
  for (int i = 0; i < 4; i++){
    if (i == pos || i == pos2) guess.moves[i] = one;
    else guess.moves[i] = two;
  }
  return guess;
} 


///////////////////////////////////////////////////////////////////
// gameplay
///////////////////////////////////////////////////////////////////

int playGame(){
  myMove = chooseMove();
  for (int i = 0; i < GAMELENGTH; i++){
    struct move try = getMove();
    struct ret ans = answer(try);
    if (win){
      printf("win!\n");
      break;
    }
    printrt(ans, i);
  }
  if (!win){
    printf("lose!\n");
    printmv(myMove);
  }
  return 0;
}

int getPlayed(){
  win = 0;
  struct chain *chain = generateChain();
  struct move frstguess = first();
  printmv(frstguess);
  struct move frstans = getMove();
  updatePossible(frstguess, frstans, chain);
  if (winp) {
    printf("i win!\n");
    return 0;
  }
  for (int i = 0; i < GAMELENGTH; i++){
    struct move guess = makeGuess(chain);
    printmv(guess);
    struct move ans = getMove();
    updatePossible(guess, ans, chain);
    if (winp){
      printf("i win!\n");
      break;
    }
  }
    
}

int main(){
  printf("player [p] or master [m]?\n");
  char a = getchar();
  if (a == 'p'){
    PLAYER = 1;
    MASTER = 0;
    return getPlayed();
  } else if (a == 'm'){
    MASTER = 1;
    PLAYER = 0;
    return playGame();
  } else {
    printf("choose between p and m ");  
  }
    
}

  
