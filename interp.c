/*
  Author: Zachary Talarick
  Date: 5/20/2020

  Interpreter for:
  Simple Machine Language
    push <num> - push <num> to stack
    pop - pop top number from stack
    add - pop top 2 numbers off the stack, add them, push the sum on the stack
    ifeq <line number> - pop the top of stack if its eq 0 jump to <line number>
    jump <line number> - jump to <line number>
    print - print the stack
    dup  - pushes a copy of whats on top of the stack
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_FILESIZE 500
#define MAX_LINESIZE 15

void print_stack();
extern int errno;
char **program;

struct node{
  int data;
  struct node *next;
};

struct stack {
  struct node *top;
};

static struct stack *stack;

/*
  TODO List
   fix seg fault when parsing empty lines
   fix parsing when numbers are more than the max allowed.
   fix parsing push/ifeq/jump cmds when no number is given
        -fix warning by handling empty strings before strtol avoiding the need for end == num
   do something about change between long and int when using strtol
   add comments?
*/

/*
  push num onto the global stack variable
*/
void push(int num){
  struct node *new = (struct node*)malloc(sizeof(struct node));
  new->next = NULL;
  new->data = num;

  if(!stack->top){ // the stack is empty
    stack->top = new;
  }else{
    new->next = stack->top;
    stack->top = new;
  }
}
/*
  pop the top of the stack from global stack
  returns the popped number
*/
int pop(){
  if(!stack->top){
    printf("Cannot pop from an empty stack\n");
    exit(EXIT_FAILURE);
  }else if(!stack->top->next){ //stack is size 1
    int ret = stack->top->data;
    free(stack->top);
    stack->top = NULL;
    return ret;
  }else{
    int ret = stack->top->data;
    free(stack->top);
    stack->top = stack->top->next;
    return ret;
  }
}

/*
  function to execute program
  split each line with " " delimeter
  compare the first sting with the available commands
  attempt to convert the second string to an integer if needed
  execute the proper command
  return 0 on success
  return > 0 on failure
  1 - invalid command
  2 - second string is not an int
  3 - more than two tokens
  4 - bad jump
*/
int execute(char** program){
  int lineNum = 1;
  char *token;
  int num;
  char *end; //for error testing with strtol
  /*
  this is the array of [command][number]
   2 - max 2 tokens per line if proper syntax | 10 # digits of max int size
  */
  char cmd[2][10];

  int i = 0;

  while(token = strtok(program[lineNum], " ")){ //split each line by delimeter " "
    do{
      strcpy(cmd[i], token);
      token = strtok(NULL, " ");
      i++;
    } while(token != NULL);
    if(i > 2){ //if the cmd array has more than 2 entries
      printf("Line %d: Syntax Error, cannot have more than two tokens.\n", lineNum);
      return 3;
    }
    //compare first string cmd[0] with available commands
    if (0 == strcmp("push", cmd[0])) {
      num = strtol(cmd[1], &end, 10);
      if (end == num || *end != '\0' || errno == ERANGE){
        printf("Line %d: Token %s is not an int.\n", lineNum, cmd[1]);
        return 2;
      }
      push(num);
    }else if (0 == strcmp("pop", cmd[0])) {
      pop();
    }else if (0 == strcmp("add", cmd[0])) {
      int a = pop();
      int b = pop();
      push(a + b);
    }else if (0 == strcmp("ifeq", cmd[0])) {
      num = strtol(cmd[1], &end, 10);
      if (end == num || *end != '\0' || errno == ERANGE){
        printf("Line %d: Token %s is not an int.\n", lineNum, cmd[1]);
        return 2;
      }
      if(num < 1 || num > MAX_FILESIZE){
        printf("Line %d: Cannot jump to line %d\n", lineNum, num);
        return 4;
      }
      int top = pop();
      if(top == 0){
        lineNum = num - 1; //must be -1 because it is incemented at the end of the loop and is the proper number in the next iteration
      }
    }else if (0 == strcmp("jump", cmd[0])) {
      num = strtol(cmd[1], &end, 10);
      if (end == num || *end != '\0' || errno == ERANGE){
        printf("Line %d: Token %s is not an int.\n", lineNum, cmd[1]);
        return 2;
      }
      if(num < 1 || num > MAX_FILESIZE){
        printf("Line %d: Cannot jump to line %d\n", lineNum, num);
        return 4;
      }
      lineNum = num - 1; //same as ifeq
    }else if (0 == strcmp("print", cmd[0])) {
      print_stack();
    }else if (0 == strcmp("dup", cmd[0])) {
      push(stack->top->data);
    }else{
      printf("Error: %s is not a valid command\n", cmd[0]);
      return 1;
    }
    i=0; //needs to be set to 0 for proper use of cmd arr
    lineNum++;
  }
  return 0;
}


/*
  Function to parse the file fp and put each line into the program array
*/
void parse(FILE *fp, char **program){
  char line[MAX_LINESIZE];
  int lineNum = 1; //starts at 1 not 0
  while(fgets(line, MAX_LINESIZE, fp)){
    strcpy(program[lineNum], strtok(line, "\n")); //strtok gets rid of the \n character received when using fgets
    lineNum++;
  }
}

void print_stack(){ //for debugging
  if(!stack->top){ //if the stack is empty
    printf("The stack is empty\n");
    return;
  }
  struct node *current = stack->top;
  printf("The stack:");
  while(current){
    printf(" %d,", current->data);
    current = current->next;
  }
  printf("\n");

}

void print_program(char **arr, int size){ //for debugging
	for(int i = 0; i < size; i++){
      printf("%s", arr[i]);
	}
}


int main(int argc, char **argv){
  if(argc != 2){
    printf("Expected Usage: ./interp <filename>\n");
    exit(EXIT_FAILURE);
  }
  FILE *fp = fopen(argv[1], "r");
  if (!fp){
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  program = malloc(MAX_FILESIZE * sizeof(char*));
  for(int i = 0; i < MAX_FILESIZE; i++){
    program[i] = malloc (MAX_LINESIZE * sizeof(char));
  }

  stack = (struct stack*)malloc(sizeof(struct stack)); //intialize stack
  stack->top = NULL;

  parse(fp, program);
  int error = execute(program); //catch return value

  //Free the stack & program
  if(stack->top){
    struct node *current = stack->top;
    while(current){
      free(current);
      current = current->next;
    }
  }
  free(stack);
  for(int i = 0; i < MAX_FILESIZE; i++){
    free(program[i]);
  }
  free(program);
  fclose(fp);
  if(error != 0){
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
