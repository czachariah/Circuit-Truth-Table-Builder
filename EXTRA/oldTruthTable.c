#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// will be a node struct to use for the temporary varibales
struct otherVarNode
{
  char varName[20];
  int val;
  struct otherVarNode *next;
};

int* evalute(char* fileName , int* inputs , int* outputs , int numbInputs , int numbOutputs);
//char* round1(char* fileName);

int main(int argc, char **argv) {
  if (argc != 2) { printf("Error with input.\n"); return 0; }

  FILE *reader;
  reader = fopen(argv[1],"r");

  if (reader == NULL) { printf("Error with reader.\n"); return 0; }

  //from here we will start reading the file to get the number of inputs and outputs...
  char* string = (char *)malloc(20 * sizeof(char));
  memset(string , '0', 20);
  int inputs;
  fscanf(reader, " %s", string); // will take in the INPUT
  fscanf(reader , " %d", &inputs); // now we know how many inputs

  int checkStrings = strcmp(string, "OUTPUT");
  while (checkStrings != 0) {
    fscanf(reader, " %s", string);
    checkStrings = strcmp(string, "OUTPUT");
  }
  //here we got through the first line and now we are on the OUTPUT line
  int outputs;
  fscanf(reader , " %d", &outputs);
  // now we have both the total number of inputs and outputs and we can then start making the truth table
  int rowSize = (inputs+outputs);
  int temp = inputs;
  int rows = 1;
  while (temp > 0) { rows *= 2; temp--; } // will give us the number of row (2^n)
  fclose(reader);

  int **table = malloc(rows * sizeof(*table));
  //1048576 => 2^20
  int i , j , k;
  for (i = 0 ; i < rows ; i ++){ // allocate memory for the 2D array truth table
    table[i] = (int*)malloc((rowSize) * sizeof(int));
  }
  //put '0' inside the truth table as the original values
  for (i = 0; i <  rows ; i++) {
      for (j = 0; j < rowSize ; j++) {
      table[i][j] = 0;
      }
  }
  //correctly make the inputs of the truth table
  //int changeTo = 0;
  int powerTo = 0;
  int check = 1;
  int sub = 0;
  for (k = (inputs - 1) ; k >= 0 ; k --) {
    powerTo = inputs - (k+1);
    if (powerTo == 0) {
      check = 1;
    } else {
      check = 1;
      while (powerTo > 0) { check *= 2; powerTo--;}
    }
  //  changeTo = 1;
    sub = check;
    bool change = false;
    for (i = 0 ; i < rows ; i++){
      j = k;
      if (sub == check) {
        change = false;
        sub--;
      } else if (sub < check  && sub > 0 && change == false) {
        sub--;
      } else if (sub == 0 ) {
        table[i][j] = 1;
        change = true;
        sub ++;
      } else if (sub < check && sub > 0 && change == true){
        table[i][j] = 1;
        sub++;
      }
    }
  }
  // will go row by row and use the input to get the output from the evalute() method
  int inputValues[inputs];
  int outputValues[outputs];
  memset(inputValues , 0 , inputs);
  memset(outputValues , 0 , outputs);
  int *inputValPtr = inputValues;
  int *outputValPtr = outputValues;
  for (i = 0; i <  rows ; i++) {
      for (j = 0; j < rowSize ; j++) {
        if ((j >= 0) && (j < inputs)){ // put in the inputs
          inputValPtr[j] = table[i][j];
        } else {                       // put in the outputs
          outputValPtr[j - inputs] = table[i][j];
        }
      }
      //here we use the evalute function to get an array of output to put back into the table
      outputValPtr = evalute(argv[1], inputValues , outputValues , inputs , outputs);
      for (k = 0 ; k < outputs ; k++){
        table[i][inputs+k] = outputValPtr[k];
      }
  }
  //correctly print out the truth table according to the instructions
  for (i = 0; i <  rows ; i++) {
      for (j = 0; j < rowSize ; j++) {
        if (j == (inputs - 1)){
          printf("%d | ", table[i][j]);
        } else if (j == (rowSize - 1)) {
          printf("%d", table[i][j]);
        } else {
         printf("%d ", table[i][j]);
        }
      }
      printf("\n");
  }
  //free up the space in the table
  for (i = 0 ; i < rows ; i++) {
    free(table[i]);
  }
  free(table);
  free(string);
  return 0;
}
int* evalute(char* fileName , int* inputs , int* outputs, int numbInputs , int numbOutputs) {
  FILE *reader;
  reader = fopen(fileName,"r");
  if (reader == NULL) { printf("Error with reader.\n"); return 0; }

  char **outPuts = malloc(numbOutputs*sizeof(*outPuts)); // array of Strings of output variables
  char **inPuts = malloc(numbInputs*sizeof(*inPuts)); // array of Strings of input variables
  int i;
  // make sure to allocate memory for the columns
  for (i = 0 ; i < numbOutputs ; i++){
    outPuts[i] = (char*)malloc((20) * sizeof(char));
  }
  for (i = 0 ; i < numbInputs ; i++) {
    inPuts[i] = (char*)malloc((20) * sizeof(char));
  }
  //need to fill these new arrays with initial values
  for (i = 0 ; i < numbOutputs ; i++){
    memset(outPuts[i], '0', 20);
  }
  for (i = 0 ; i < numbInputs ; i++) {
    memset(inPuts[i], '0' , 20);
  }

  // we need to account for the chance that there are going to be some temp variables
  // so we are going to use a linked list since we don't know how many will show up
  struct otherVarNode *root = NULL; // don't need to do anything right now, only when we get a temp var

  //this will be our string token that we use to comapare and find values / put values in
  char* string = (char *)malloc(20 * sizeof(char));
  memset(string , '0', 20);

  //starting the loop to read for Directives
  int checkScan = fscanf(reader, " %s" , string);
  int checkString = 0;
  while (checkScan > 0) {
//___________________________________________________________________________________________________________________________________________________
    if ((checkString = strcmp(string,"INPUT")) == 0) {
      //the next string should be a number that tells us how many inputs there are , so we can copy that string to our array inPuts
      int count , i;
      fscanf(reader, " %d" , &count);
      i = 0;
      // put all the variables in the right place in the array, these will then coorrespond with the array with the input values
      while (i < count) {
        fscanf(reader, "%*[: ] %16s", string);
        strcpy(inPuts[i], string);
        i++;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"OUTPUT")) == 0) {
      //the next string should be a number that tells us the number of outputs
      int count, i;
      fscanf(reader," %d", &count);
      i = 0;
      // put the string into the array outPuts
      while (i < count) {
        fscanf(reader, "%*[: ] %16s", string);
        strcpy(outPuts[i], string);
        i++;
      }
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"PASS")) == 0) { // will take one input and put it into one output
      int parm;
      fscanf(reader, "%*[: ] %16s", string); // will be your one parameter for input
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts and see if the string matches one of them
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm = inputs[i]; // if it matches, then use the same index in the inputs array argument to get the value
        }
      }
      if (found == 0) {
        // means that the string is not one of the inputs , so now check the temp var by looking at the linked list
        // if a temp var shows up in the input side, then we can assume that we have already seen it and sotred it with a value
        // though it may not be exaclty what we are looking for in every case
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      // if we still havent found the value, then that means that the string is either a literal 0 or 1
      // so we just need to put 0 or 1 ints into parm
      if (found == 0){
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm = 1;
        }
      }
      // if the next line is true, then this means that the input is invalid and we cannot determine the output
      // EXTRA CREDIT => at this step, make a function that can look through the next couple lines of instructions and find values
      if (found == 0) { return outputs; }

      // now we are done getting the input so we must move onto the second argument which is the ouput
      fscanf(reader, "%*[: ] %16s", string);

      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = parm; // can use the same index to fill in the value for the new output
        }
      }
      if (found == false) { // means that the string is not one of the inputs , so it must be a temp var
        if (root != NULL) {
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = parm;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = parm;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new temp node and put it into the linked list
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = parm;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) { printf("Something went wrong at: PASS\n"); return outputs; } // should never be true ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"NOT")) == 0) { // will take one input and put the opposite value into one output
      int parm;
      fscanf(reader, "%*[: ] %16s", string); // will be your one parameter for input
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm = 1;
        }
      }
      // if the next line is true, then this means that the input is invalid and we cannot determine the output
      // EXTRA CREDIT => at this step, make a function that can look through the next couple lines of instructions and find values
      if (found == 0) {  return outputs; }

      // since this is NOT, we need to invert the input
      if (parm == 0){
        parm = 1;
      } else {
        parm = 0;
      }

      //we are done with the input, so now we move onto the output
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = parm;
        }
      }
      if (found == 0) { // means that the string is not one of the outputs , so now check the temp vars
        if (root != NULL) {
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = parm;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = parm;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = parm;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) { printf("Something went wrong at: NOT\n"); return outputs;} // should never be true ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"AND")) == 0) { // will take in two inputs and one output (output = 1 only if both inputs are 1)
      int parm1 , parm2 , out;
      fscanf(reader, "%*[: ] %16s", string); // will be your first parameter for input (parm1)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm1 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm1 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm1 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm1 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      fscanf(reader, "%*[: ] %16s", string); // will be your second parameter for input (parm2)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm2 = inputs[i];
        }
      }
      if (found == false) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm2 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == false){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm2 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm2 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      // now figure out what the output should be using the two parameters that we just got
      if ((parm1 == 1) && (parm2 == 1)) {
        out = 1;
      } else {
        out = 0;
      }
      // now we just need to put the outputs value in the correct output variable
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = out;
        }
      }
      if (found == 0) {                                           // means that the string is not one of the inputs , so now check the temp var
        if (root != NULL) {                                       // there are other temp vars and you need to find one or see if it is in there
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = out;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = out;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = out;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) {  printf("Something went wrong at: AND\n"); return outputs; } // this should never be true ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"OR")) == 0) { // will take in two inputs and one output (output = 0 only if both inputs are 0)
      int parm1, parm2 , out;
      fscanf(reader, "%*[: ] %16s", string); // will be your first parameter for input (parm1)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm1 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm1 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm1 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm1 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      fscanf(reader, "%*[: ] %16s", string); // will be your second parameter for input (parm2)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm2 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm2 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm2 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm2 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      // now figure out what the output should be using the parameters
      if ((parm1 == 0) && (parm2 == 0)) {
        out = 0;
      } else {
        out = 1;
      }
      //now find the output variable to put the output value in
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = out;
        }
      }
      if (found == 0) {                                            // means that the string is not one of the inputs , so now check the temp var
        if (root != NULL) {                                        // there are other temp vars and you need to find one or see if it is in there
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = out;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = out;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = out;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) { printf("Something went wrong at: OR\n"); return outputs;} // should never happen ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"NAND")) == 0) { // will take in two inputs and one output , then invert the output of a AND
      int parm1 , parm2 , out;
      fscanf(reader, "%*[: ] %16s", string); // will be your first parameter for input (parm1)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm1 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm1 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm1 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm1 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      fscanf(reader, "%*[: ] %16s", string); // will be your second parameter for input (parm2)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm2 = inputs[i];
        }
      }
      if (found == false) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm2 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == false){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm2 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm2 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      // now figure out what the output should be using the two parameters that we just got
      if ((parm1 == 1) && (parm2 == 1)) {
        out = 0;
      } else {
        out = 1;
      }
      // now we just need to put the outputs value in the correct output variable
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = out;
        }
      }
      if (found == 0) {                                         // means that the string is not one of the inputs , so now check the temp var
        if (root != NULL) {                                     // there are other temp vars and you need to find one or see if it is in there
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = out;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = out;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = out;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) {  printf("Something went wrong at: NAND\n"); return outputs; } // this should never be true ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"NOR")) == 0) { // will take in two inputs and one output, and then invert the output of an OR
      int parm1, parm2 , out;
      fscanf(reader, "%*[: ] %16s", string); // will be your first parameter for input (parm1)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm1 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm1 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm1 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm1 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      fscanf(reader, "%*[: ] %16s", string); // will be your second parameter for input (parm2)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm2 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm2 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm2 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm2 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      // now figure out what the output should be using the parameters
      if ((parm1 == 0) && (parm2 == 0)) {
        out = 1;
      } else {
        out = 0;
      }
      //now find the output variable to put the output value in
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = out;
        }
      }
      if (found == 0) {                                              // means that the string is not one of the inputs , so now check the temp var
        if (root != NULL) {                                          // there are other temp vars and you need to find one or see if it is in there
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = out;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = out;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = out;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) { printf("Something went wrong at: NOR\n"); return outputs;} // should never happen ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"XOR")) == 0) { // will take two inputs and one output ; ouutput = 1 if the two paramters not equal
      int parm1, parm2 , out;
      fscanf(reader, "%*[: ] %16s", string); // will be your first parameter for input (parm1)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      int i;
      bool found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm1 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm1 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm1 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm1 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      fscanf(reader, "%*[: ] %16s", string); // will be your second parameter for input (parm2)
      //figure out if it is one of the inputs , temporary varibales or 0 / 1
      found = 0;
      for (i = 0 ; i < numbInputs ; i++){ // check through inPuts
        if ((strcmp(string,inPuts[i])) == 0){
          found = 1;
          parm2 = inputs[i];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm2 = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm2 = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm2 = 1;
        }
      }
      //if the next line is true, then that means that there is incorrect instruction input
      if (found == 0) {  return outputs; }

      // now figure out what the output should be using the parameters
      if (parm1 != parm2) {
        out = 1;
      } else {
        out = 0;
      }
      //now find the output variable to put the output value in
      fscanf(reader, "%*[: ] %16s", string);
      //figure out if it is one of the outputs or temporary variables
      found = 0;
      for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
        if ((strcmp(string,outPuts[i])) == 0){
          found = 1;
          outputs[i] = out;
        }
      }
      if (found == 0) {                                       // means that the string is not one of the inputs , so now check the temp var
        if (root != NULL) {                                   // there are other temp vars and you need to find one or see if it is in there
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find != NULL) {
            if ((strcmp(string,find->varName)) == 0){
              found = 1;
              find->val = out;
            }
            find = find->next;
          }
          // free(find);
        } else { // root is null and this is the first temp var
          root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(root->varName, string);
          root->val = out;
          root->next = NULL;
          found = 1;
        }
        if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
          struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          find = root;
          while(find->next != NULL) {
            find = find->next;
          }
          struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
          strcpy(newNode->varName, string);
          newNode->val = out;
          newNode->next = NULL;
          find->next = newNode;
          found = 1;
          // free(newNode);
          // free(find);
        }
      }
      if (found == 0) { printf("Something went wrong at: XOR\n"); return outputs;} // should never happen ...
//____________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"DECODER")) == 0) {
  // first argument tells you number of inputs and then use it to find the number of outputs (2^n) ; inputs are binary (need to convert to decimal)
  // one output is 1 while the rest will be 0 ; use the binary from the input in order to find this specific output
    int inputsForDEC;
    fscanf(reader , " %d", &inputsForDEC); // now we know how many inputs
    //now figure out how many outputs there are
    int temp;
    temp = inputsForDEC;
    int totalOutputsDEC = 1;
    while (temp > 0) { totalOutputsDEC *= 2; temp--; } // will give us 2^n outputs

    //now work on the inputs and figure out the binary form and convert to decimal by totaling everything up
    int i , curBinary , parm , total;
    int totalsForInput = 0;
    bool found;
    for (i = 0 ; i < inputsForDEC ; i++) { // will run through all the inputs
      fscanf(reader, "%*[: ] %16s", string);
      // figure out of if this input string is an input variable, temp var or literal 0/1
      found = 0;
      int j;
      for (j = 0 ; j < numbInputs ; j++){ // check through inPuts
        if ((strcmp(string,inPuts[j])) == 0){
          found = 1;
          parm = inputs[j];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == false){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm = 1;
        }
      }
      //if the next line is true, there is a problem with the input instructions
      if (found == false) {  return outputs; }

      // now we have the value for parm and now we need the binary value to multipy and add all together to keep track
      temp = inputsForDEC - 1 - i;          // since we going to try to figure out the binary value from left to right instead of the opposite way
      curBinary = 1;
      while (temp > 0) { curBinary *= 2; temp--; } // will give us 2^(correct numb) so we can muliply with the input
      total = parm * curBinary;             // parm will be 0 or 1 and we muliply that with the binary number to get a total
      totalsForInput += total;              // totalsForInput will keep track of all the added totals so that we can iterate through the outputs
    } // ends for loop

    //now we have the totalsForInput so find it in the output and make that 1 and the rest make it all 0
    for (i = 0 ; i < totalOutputsDEC ; i++){
      if (i == totalsForInput ) {
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        int j;
        for (j = 0 ; j < numbOutputs ; j++){ // check through outPuts
          if ((strcmp(string,outPuts[j])) == 0){
            found = 1;
            outputs[j] = 1;
          }
        }
        if (found == 0) {                                         // means that the string is not one of the inputs , so now check the temp var
          if (root != NULL) {                                     // there are other temp vars and you need to find one or see if it is in there
            struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            find = root;
            while(find != NULL) {
              if ((strcmp(string,find->varName)) == 0){
                found = 1;
                find->val = 1;
              }
              find = find->next;
            }
            // free(find);
          } else { // root is null and this is the first temp var
            root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            strcpy(root->varName, string);
            root->val = 1;
            root->next = NULL;
            found = 1;
          }
          if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
            struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            find = root;
            while(find->next != NULL) {
              find = find->next;
            }
            struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            strcpy(newNode->varName, string);
            newNode->val = 1;
            newNode->next = NULL;
            find->next = newNode;
            found = 1;
            // free(newNode);
            // free(find);
          }
        }
        // if the next line is true, there was something wrong with the input instructions
        if (found == 0) {  return outputs; }
      } else {
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        int j;
        for (j = 0 ; j < numbOutputs ; j++){ // check through outPuts
          if ((strcmp(string,outPuts[j])) == 0){
            found = 1;
            outputs[j] = 0;
          }
        }
        if (found == 0) {                                       // means that the string is not one of the inputs , so now check the temp var
          if (root != NULL) {                                   // there are other temp vars and you need to find one or see if it is in there
            struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            find = root;
            while(find != NULL) {
              if ((strcmp(string,find->varName)) == 0){
                found = 1;
                find->val = 0;
              }
              find = find->next;
            }
            // free(find);
          } else { // root is null and this is the first temp var
            root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            strcpy(root->varName, string);
            root->val = 0;
            root->next = NULL;
            found = 1;
          }
          if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
            struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            find = root;
            while(find->next != NULL) {
              find = find->next;
            }
            struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
            strcpy(newNode->varName, string);
            newNode->val = 0;
            newNode->next = NULL;
            find->next = newNode;
            found = 1;
            // free(newNode);
            // free(find);
          }
        }
        if (found == 0) { return outputs; } // should never happen ...
      }
    } // ends for loop
//____________________________________________________________________________________________________________________________________________________
  } else if ((checkString = strcmp(string,"MULTIPLEXER")) == 0) {
    // first argument tells you number of selectors (which need to converted to binary and then decimal)
    // and then use it to find the number of inputs (2^n) ; these selectors are chosen depending on the result of the input conversion
    // there is just one output that will recieve the value of the specific selector that will be chosen
    int inputAmount;
    int totalsForInput = 0;                 // will used at the end to determine which selector's value is to be passed into the output
    fscanf(reader, " %d" , &inputAmount);   // will be number of selectors

    //now find the number of selectors and make an int array with the size you will get
    int temp = inputAmount;
    int totalInputsMUX = 1;
    while (temp > 0) { totalInputsMUX *= 2; temp--; }           // will give us 2^n selectors
    int *binary = (int*)malloc(totalInputsMUX*sizeof(int));     // use this array to store the values of the selectors

    int i;
    for (i = 0 ; i < totalInputsMUX ; i++) {                    // go through the next (2^n)-1 selectors and put the values into the binary array
      fscanf(reader, "%*[: ] %16s", string);
      bool found = 0;
      int j;
      for (j = 0 ; j < numbInputs ; j++){ // check through inPuts
        if ((strcmp(string,inPuts[j])) == 0){
          found = 1;
          binary[i] = inputs[j];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            binary[i] = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          binary[i] = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          binary[i] = 1;
        }
      }
      // if the next line is true, then there is a problem with the input instructions
      if (found == 0) {  return outputs; }
    } // end of the for loop

    // now we will got through the n inputs and try to change them from binary to decimal (same thing that we did for the DECODER)
    for (i = 0 ; i < inputAmount ; i++) {
      int parm;
      fscanf(reader, "%*[: ] %16s", string);
      bool found = 0;
      int j;
      for (j = 0 ; j < numbInputs ; j++){ // check through inPuts
        if ((strcmp(string,inPuts[j])) == 0){
          found = 1;
          parm = inputs[j];
        }
      }
      if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            parm = find->val;
          }
          find = find->next;
        }
        // free(find);
      }
      if (found == 0){ // means that the string is not one of the temporary variables (or inputs) so, it has to be 0 or 1
        if ((strcmp(string,"0")) == 0) {
          found = 1;
          parm = 0;
        }
        if ((strcmp(string,"1")) == 0) {
          found = 1;
          parm = 1;
        }
      }
      // if the next line is true, then there is a problem with the input instructions
      if (found == false) {  return outputs; }

      // now we have the value for parm and now we need the binary value to multipy and add all together to keep track
      //int temp = inputAmount;
      //int totalInputsMUX = 1;
      //while (temp > 0) { totalInputsMUX *= 2; temp--; } // will give us 2^n output
      temp = inputAmount - 1 - i;
      int tot = 1;
      int total;
      while (temp > 0) { tot *= 2; temp--; } // will give us 2^(correct numb) so we can muliply with the input
      total = parm * tot;                    // depending on whether parm is 1 or 0, this gets multiplied by tot
      totalsForInput += total;               // keep track of the totalsForInput in order to use it in the end to get the output value
    } // ends for loop

    // now we know exactly which selector value to use and just need to find the correct output varibale to put the value in
    fscanf(reader, "%*[: ] %16s", string); // the one output
    bool found = 0;
    for (i = 0 ; i < numbOutputs ; i++){ // check through outPuts
      if ((strcmp(string,outPuts[i])) == 0){
        found = 1;
        outputs[i] = binary[totalsForInput];
      }
    }
    if (found == 0) { // means that the string is not one of the inputs , so now check the temp var
      if (root != NULL) { // there are other temp vars and you need to find one or see if it is in there
        struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        find = root;
        while(find != NULL) {
          if ((strcmp(string,find->varName)) == 0){
            found = 1;
            find->val = binary[totalsForInput];
          }
          find = find->next;
        }
        // free(find);
      } else { // root is null and this is the first temp var
        root = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
        strcpy(root->varName, string);
        root->val = binary[totalsForInput];
        root->next = NULL;
        found = 1;
      }
    }
    if (found == 0) { // root is not empty, but the temp var was not in there, so make a new node
      struct otherVarNode *find = (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
      find = root;
      while(find->next != NULL) {
        find = find->next;
      }
      struct otherVarNode *newNode= (struct otherVarNode*)malloc(sizeof(struct otherVarNode));
      strcpy(newNode->varName, string);
      newNode->val = binary[totalsForInput];
      newNode->next = NULL;
      find->next = newNode;
      found = 1;
      // free(newNode);
      // free(find);
    }
      if (found == false) {  return outputs; } // this should never happen ...
  } // end of all the else - if
//____________________________________________________________________________________________________________________________________________________
    checkScan = fscanf(reader, " %s" , string);
  } // ends main while loop

// free and close everything before returning
  if (root != NULL){
    struct otherVarNode *cur = NULL;
    struct otherVarNode *next = root;
    do {
      cur = next;
      next = next->next;
      free(cur);
    } while (next != NULL);
  }
  for (i = 0 ; i < numbOutputs ; i++){
    free (outPuts[i]);
  }
  free(outPuts);
  for (i = 0 ; i < numbInputs ; i++) {
    free (inPuts[i]);
  }
  free(inPuts);
  free(string);
  fclose(reader);
  return outputs;
}
