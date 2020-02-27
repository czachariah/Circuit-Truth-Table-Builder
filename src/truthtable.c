/*
 Chris Zachariah
 Rutgers University
 Truth Table Builder
 Language : C
 Input : Special circuit langauge , Output : Truth table 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct gateNode                                          //node to store gates, inputs and outputs
{
  int name;                                              //all the gates will be given a name so I don't have to do any string comparisons
  int* inputVals;
  int* outputVals;
  struct gateNode *next;
};
struct node                                              //node to store the variable names for inputs, outputs and temps
{
  char name[20];
  struct node *next;
};

int* evalute(int* mainArr , struct gateNode *root);

int main (int argc , char** argv) {
  if (argc != 2) { printf("Error with input.\n"); return 0; }

  // Round 1 : get the number of inputs, outputs and temps and make a linked list of names of the inputs, outputs and temps
  FILE *reader;
  reader = fopen(argv[1],"r");
  if (reader == NULL) { printf("Error with reader.\n"); return 0; }

  char* string = (char*)malloc(20*sizeof(char));         // allocate space for token to look at individual strings
  memset(string, '0', 20);
  int inputs = 0;                                        // store the number of each variable to make a master array later in the code
  int outputs = 0;
  int temps = 0;
  struct node *inputRoot = NULL;                         // root for the linked list of input names
  struct node *inputLLptr = NULL;                        // pointer to use to go through the input LL
  struct node *outputRoot = NULL;                        // root for the linked list of output names
  struct node *outputLLptr = NULL;                       // pointer to use to go through the output LL
  struct node *tempRoot = NULL;                          // root for the linked list of temporary var names
  struct node *tempLLptr = NULL;                         // pointer to go through the temp LL

  int checkScan = fscanf(reader, " %s" , string);        // if this returns 1 , then we have read a string successfully ; use in the while loop
  int checkString = 0;                                   // will read through the rest of the strings depending on the directive

  while(checkScan > 0) {
//___________________________________________________________________________________________________________________________________________________
    if ((checkString = strcmp(string,"INPUT")) == 0) {
      fscanf(reader, " %d" , &inputs);                  // now we have the total number of input variables
      int i = 0;
      while (i < inputs) {                               // we must now put these inputs into the input linked list
        fscanf(reader, "%*[: ] %16s", string);
        if (inputRoot != NULL){                                       // the root is not NULL, and we need to put the new node in the front
          inputLLptr = (struct node*)malloc(sizeof(struct node));
          strcpy(inputLLptr->name, string);
          inputLLptr->next = inputRoot;
          inputRoot = inputLLptr;
          inputLLptr = NULL;
        } else {
          inputRoot = (struct node*)malloc(sizeof(struct node));      // this input will be the first node of the input LL
          strcpy(inputRoot->name, string);
          inputRoot->next = NULL;
        }
        i++;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"OUTPUT")) == 0) {
      fscanf(reader, " %d", &outputs);                    // now we have the total number of outputs
      int i = 0;
      while(i < outputs) {
        fscanf(reader, "%*[: ] %16s", string);
        if (outputRoot != NULL){                                       // the root is not NULL, and we need to put the new node in the front
          outputLLptr = (struct node*)malloc(sizeof(struct node));
          strcpy(outputLLptr->name, string);
          outputLLptr->next = outputRoot;
          outputRoot = outputLLptr;
          outputLLptr = NULL;
        } else {
          outputRoot = (struct node*)malloc(sizeof(struct node));      // this output will be the first node of the output LL
          strcpy(outputRoot->name, string);
          outputRoot->next = NULL;
        }
        i++;
      }
//___________________________________________________________________________________________________________________________________________________
  } else if ( ((checkString = strcmp(string,"PASS")) == 0) || ((checkString = strcmp(string,"NOT")) == 0) ) {           // will have one input and one output
      fscanf(reader, "%*[: ] %16s", string);
      bool found = 0;
      inputLLptr = inputRoot;
      while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
        if ((strcmp(string,inputLLptr->name)) == 0){
          found = 1;
        }
        inputLLptr = inputLLptr->next;
      }
      if (found == 0) {                                                // this variable is not an input, it might be a temp vars
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {
        if ( ((strcmp(string,"0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
          if (tempRoot != NULL) {
            tempLLptr = (struct node*)malloc(sizeof(struct node));
            strcpy(tempLLptr->name,string);
            tempLLptr->next = tempRoot;
            tempRoot = tempLLptr;
            tempLLptr = NULL;
          } else {
            tempRoot = (struct node*)malloc(sizeof(struct node));
            strcpy(tempRoot->name,string);
            tempRoot->next = NULL;
          }
          found = 1;
          temps++;
        }
      }
      //now we are done with figuring out the input, now move onto the output
      fscanf(reader, "%*[: ] %16s", string);
      found = 0;
      outputLLptr = outputRoot;
      while(outputLLptr != NULL) {
        if ((strcmp(string,outputLLptr->name)) == 0) {                      // go through the output LL and see if it is one of those variables
          found = 1;
        }
        outputLLptr = outputLLptr->next;
      }
      if (found == 0) {                                                     // means that it may be a temp var
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {                                                     // means that it is a new temp var
        if (tempRoot != NULL) {
          tempLLptr = (struct node*)malloc(sizeof(struct node));
          strcpy(tempLLptr->name,string);
          tempLLptr->next = tempRoot;
          tempRoot = tempLLptr;
          tempLLptr = NULL;
        } else {
          tempRoot = (struct node*)malloc(sizeof(struct node));
          strcpy(tempRoot->name,string);
          tempRoot->next = NULL;
        }
        found = 1;
        temps++;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ( ((checkString = strcmp(string,"AND")) == 0) || ((checkString = strcmp(string,"OR")) == 0) || ((checkString = strcmp(string,"NAND")) == 0) || ((checkString = strcmp(string,"NOR")) == 0) || ((checkString = strcmp(string,"XOR")) == 0) ) { // will take two inputs and one output
      fscanf(reader, "%*[: ] %16s", string);
      bool found = 0;
      inputLLptr = inputRoot;
      while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
        if ((strcmp(string,inputLLptr->name)) == 0){
          found = 1;
        }
        inputLLptr = inputLLptr->next;
      }
      if (found == 0) {                                                // this variable is not an input, it might be a temp vars
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {
        if ( ((strcmp(string,"0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
          if (tempRoot != NULL) {
            tempLLptr = (struct node*)malloc(sizeof(struct node));
            strcpy(tempLLptr->name,string);
            tempLLptr->next = tempRoot;
            tempRoot = tempLLptr;
            tempLLptr = NULL;
          } else {
            tempRoot = (struct node*)malloc(sizeof(struct node));
            strcpy(tempRoot->name,string);
            tempRoot->next = NULL;
          }
          found = 1;
          temps++;
        }
      }
      fscanf(reader, "%*[: ] %16s", string);
      found = 0;
      inputLLptr = inputRoot;
      while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
        if ((strcmp(string,inputLLptr->name)) == 0){
          found = 1;
        }
        inputLLptr = inputLLptr->next;
      }
      if (found == 0) {                                                // this variable is not an input, it might be a temp vars
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {
        if ( ((strcmp(string,"0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
          if (tempRoot != NULL) {
            tempLLptr = (struct node*)malloc(sizeof(struct node));
            strcpy(tempLLptr->name,string);
            tempLLptr->next = tempRoot;
            tempRoot = tempLLptr;
            tempLLptr = NULL;
          } else {
            tempRoot = (struct node*)malloc(sizeof(struct node));
            strcpy(tempRoot->name,string);
            tempRoot->next = NULL;
          }
          found = 1;
          temps++;
        }
      }
      //now we are done with figuring out the input, now move onto the output
      fscanf(reader, "%*[: ] %16s", string);
      found = 0;
      outputLLptr = outputRoot;
      while(outputLLptr != NULL) {
        if ((strcmp(string,outputLLptr->name)) == 0) {                      // go through the output LL and see if it is one of those variables
          found = 1;
        }
        outputLLptr = outputLLptr->next;
      }
      if (found == 0) {                                                     // means that it may be a temp var
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {                                                     // means that it is a new temp var
        if (tempRoot != NULL) {
          tempLLptr = (struct node*)malloc(sizeof(struct node));
          strcpy(tempLLptr->name,string);
          tempLLptr->next = tempRoot;
          tempRoot = tempLLptr;
          tempLLptr = NULL;
        } else {
          tempRoot = (struct node*)malloc(sizeof(struct node));
          strcpy(tempRoot->name,string);
          tempRoot->next = NULL;
        }
        found = 1;
        temps++;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"DECODER")) == 0) {         // parameters: n : 1 - n : 0 - (2^n -1)
      int numInputs;
      fscanf(reader, " %d", &numInputs);                                 // the first parameter will tell you the number inputs and 2^n outputs
      int i;
      bool found;
      for (i = 0 ; i < numInputs ; i++) {                           // get the first n inputs and figure out if they are past inputs or temps or 1/0
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        inputLLptr = inputRoot;
        while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
          if ((strcmp(string,inputLLptr->name)) == 0){
            found = 1;
          }
          inputLLptr = inputLLptr->next;
        }
        if (found == 0) {                                                // this variable is not an input, it might be a temp vars
          tempLLptr = tempRoot;
          while(tempLLptr != NULL){
            if ((strcmp(string,tempLLptr->name)) == 0) {
              found = 1;
            }
            tempLLptr = tempLLptr->next;
          }
        }
        if (found == 0) {
          if ( ((strcmp(string,"0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
            if (tempRoot != NULL) {
              tempLLptr = (struct node*)malloc(sizeof(struct node));
              strcpy(tempLLptr->name,string);
              tempLLptr->next = tempRoot;
              tempRoot = tempLLptr;
              tempLLptr = NULL;
            } else {
              tempRoot = (struct node*)malloc(sizeof(struct node));
              strcpy(tempRoot->name,string);
              tempRoot->next = NULL;
            }
            found = 1;
            temps++;
          }
        }
      }
      // now we need to go through all the outputs
      int temp = numInputs;
      int totalOutputs = 1;
      while (temp > 0) { totalOutputs *= 2; temp--; }                         // will give us 2^n outputs
      for (i = 0 ; i < totalOutputs ; i++) {
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        outputLLptr = outputRoot;
        while(outputLLptr != NULL) {
          if ((strcmp(string,outputLLptr->name)) == 0) {                      // go through the output LL and see if it is one of those variables
            found = 1;
          }
          outputLLptr = outputLLptr->next;
        }
        if (found == 0) {                                                     // means that it may be a temp var
          tempLLptr = tempRoot;
          while(tempLLptr != NULL){
            if ((strcmp(string,tempLLptr->name)) == 0) {
              found = 1;
            }
            tempLLptr = tempLLptr->next;
          }
        }
        if (found == 0) {                                                     // means that it is a new temp var
          if (tempRoot != NULL) {
            tempLLptr = (struct node*)malloc(sizeof(struct node));
            strcpy(tempLLptr->name,string);
            tempLLptr->next = tempRoot;
            tempRoot = tempLLptr;
            tempLLptr = NULL;
          } else {
            tempRoot = (struct node*)malloc(sizeof(struct node));
            strcpy(tempRoot->name,string);
            tempRoot->next = NULL;
          }
          found = 1;
          temps++;
        }
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"MULTIPLEXER")) == 0) {         // parameters: n : 0 - (2^n - 1) : 1 - n : out
      int numSelectors;
      fscanf(reader, " %d" , &numSelectors);                                // will give us the number of selectors (n)
      int temp = numSelectors;
      int totalInputs = 1;
      while (temp > 0) { totalInputs *= 2; temp--; }                        // will give us the total number of inputs (2^n)
      bool found;
      int i;
      for (i = 0 ; i < totalInputs ; i++) {
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        inputLLptr = inputRoot;
        while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
          if ((strcmp(string,inputLLptr->name)) == 0){
            found = 1;
          }
          inputLLptr = inputLLptr->next;
        }
        if (found == 0) {                                                // this variable is not an input, it might be a temp vars
          tempLLptr = tempRoot;
          while(tempLLptr != NULL){
            if ((strcmp(string,tempLLptr->name)) == 0) {
              found = 1;
            }
            tempLLptr = tempLLptr->next;
          }
        }
        if (found == 0) {
          if ( ((strcmp(string, "0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
            if (tempRoot != NULL) {
              tempLLptr = (struct node*)malloc(sizeof(struct node));
              strcpy(tempLLptr->name,string);
              tempLLptr->next = tempRoot;
              tempRoot = tempLLptr;
              tempLLptr = NULL;
            } else {
              tempRoot = (struct node*)malloc(sizeof(struct node));
              strcpy(tempRoot->name,string);
              tempRoot->next = NULL;
            }
            temps++;
            found = 1;
          }
        }
      }
      // now we have gone through the 2^n inputs, now we need to go through the n selectors (could be inputs/temps/0 or 1)
      for (i = 0 ; i < numSelectors ; i++) {
        fscanf(reader, "%*[: ] %16s", string);
        found = 0;
        inputLLptr = inputRoot;
        while (inputLLptr != NULL) {                                     // check through the input LL and try to find it
          if ((strcmp(string,inputLLptr->name)) == 0){
            found = 1;
          }
          inputLLptr = inputLLptr->next;
        }
        if (found == 0) {                                                // this variable is not an input, it might be a temp vars
          tempLLptr = tempRoot;
          while(tempLLptr != NULL){
            if ((strcmp(string,tempLLptr->name)) == 0) {
              found = 1;
            }
            tempLLptr = tempLLptr->next;
          }
        }
        if (found == 0) {
          if ( ((strcmp(string,"0")) != 0) && ((strcmp(string,"1")) != 0) ) { // means that the string is a new temp var
            if (tempRoot != NULL) {
              tempLLptr = (struct node*)malloc(sizeof(struct node));
              strcpy(tempLLptr->name,string);
              tempLLptr->next = tempRoot;
              tempRoot = tempLLptr;
              tempLLptr = NULL;
            } else {
              tempRoot = (struct node*)malloc(sizeof(struct node));
              strcpy(tempRoot->name,string);
              tempRoot->next = NULL;
            }
          }
          found = 1;
          temps++;
        }
      }
      // now we are done with the inputs and selectors, so now we only have the one output to deal with
      fscanf(reader, "%*[: ] %16s", string);
      found = 0;
      outputLLptr = outputRoot;
      while(outputLLptr != NULL) {
        if ((strcmp(string,outputLLptr->name)) == 0) {                      // go through the output LL and see if it is one of those variables
          found = 1;
        }
        outputLLptr = outputLLptr->next;
      }
      if (found == 0) {                                                     // means that it may be a temp var
        tempLLptr = tempRoot;
        while(tempLLptr != NULL){
          if ((strcmp(string,tempLLptr->name)) == 0) {
            found = 1;
          }
          tempLLptr = tempLLptr->next;
        }
      }
      if (found == 0) {                                                     // means that it is a new temp var
        if (tempRoot != NULL) {
          tempLLptr = (struct node*)malloc(sizeof(struct node));
          strcpy(tempLLptr->name,string);
          tempLLptr->next = tempRoot;
          tempRoot = tempLLptr;
          tempLLptr = NULL;
        } else {
          tempRoot = (struct node*)malloc(sizeof(struct node));
          strcpy(tempRoot->name,string);
          tempRoot->next = NULL;
        }
        found = 1;
        temps++;
      }
//___________________________________________________________________________________________________________________________________________________
    }
    checkScan = fscanf(reader, " %s" , string);
  } // ends the while loop for Round 1

  // make 2 arrays : first array => strings, with 0,1,inputs,outputs,temps and second array => ints with all 0 (index 1 is 1)
  int totalSize = (2 + inputs + outputs + temps);
  char **allVars = malloc(totalSize*sizeof(*allVars));                        // make a double string array to store all the variable names
  int tempInputs = inputs;
  int tempOutputs = outputs;
  int tempTemps = temps;
  int i;
  for (i = 0 ; i < totalSize ; i++) {
    allVars[i] = (char*)malloc(20*sizeof(char));
  }
  for (i = 0 ; i < totalSize ; i++){
    memset(allVars[i], '0', 20);
  }
  for (i = (totalSize - 1) ; i > 1 ; i--) {                            //  here we are going to insert all the names backwards from the LLs
    if (tempTemps > 0) {
      tempLLptr = tempRoot;
      tempRoot = tempRoot->next;
      strcpy(allVars[i],tempLLptr->name);
      free(tempLLptr);
      tempLLptr = NULL;
      tempTemps--;
    } else if (tempOutputs > 0) {
      outputLLptr = outputRoot;
      outputRoot = outputRoot->next;
      strcpy(allVars[i],outputLLptr->name);
      free(outputLLptr);
      outputLLptr = NULL;
      tempOutputs--;
    } else if (tempInputs > 0) {
      inputLLptr = inputRoot;
      inputRoot = inputRoot->next;
      strcpy(allVars[i],inputLLptr->name);
      free(inputLLptr);
      inputLLptr = NULL;
      tempInputs--;
    }
  }
  // free and NULL all pointers you are not going to use anymore
  free(inputRoot);
  inputRoot = NULL;
  free(outputRoot);
  outputRoot = NULL;
  free(tempRoot);
  tempRoot = NULL;
  fclose(reader);
  // makse sure to set the right values for the 0 and 1
  allVars[0] = "0";
  allVars[1] = "1";
  //make an int array that will coorrespond to the allVars array
  int *allVals = (int*)malloc(totalSize*sizeof(int));
  for (i = 0 ; i < totalSize ; i++) { allVals[i] = 2; }
  allVals[0] = 0;
  allVals[1] = 1;

  // Round 2 : go through the file and make gateNode LL and put in the correct parameters for each gate's inputs and outputs
  /*  KEY => GATE NAMES
        1 => PASS
        2 => NOT
        3 => AND
        4 => OR
        5 => NAND
        6 => NOR
        7 => XOR
        8 => DECODER
        9 => MULTIPLEXER
  */
  FILE *reader2;
  reader2 = fopen(argv[1],"r");
  if (reader2 == NULL) { printf("Error with reader.\n"); return 0; }
  struct gateNode *root = NULL;                                      // this is the root to the linked list of gate node instructions
  struct gateNode *ptr = NULL;                                       // will be the main pointer
  struct gateNode *newNode = NULL;                                   // will be used to add new nodes to the LL

  checkScan = fscanf(reader2, " %s" , string);               // if this returns 1 , then we have read a string successfully ; use in the while loop
  checkString = 0;                                           // will read through the rest of the strings depending on the directive
  while (checkScan > 0) {
//___________________________________________________________________________________________________________________________________________________
    if ( ((checkString = strcmp(string,"INPUT")) == 0) || ((checkString = strcmp(string,"OUTPUT")) == 0) ) { // can
      int numToSkip;
      fscanf(reader2, " %d" , &numToSkip);                                // now we have the total number of input variables
      int i = 0;
      while (i < numToSkip) {
        fscanf(reader2, "%*[: ] %16s", string);
        i++;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ( ((checkString = strcmp(string,"PASS")) == 0) || ((checkString = strcmp(string,"NOT")) == 0) ) { // 1 input, 1 output
      int gateName;
      int *inputIndexs = (int*)malloc(1*sizeof(int));
      int *outputIndexs = (int*)malloc(1*sizeof(int));
      if (strcmp(string,"PASS") == 0) {
        gateName = 1;
      } else {
        gateName = 2;
      }
      fscanf(reader2, "%*[: ] %16s", string);                              // get the input and find out which index it is located at
      for(i = 0 ; i < totalSize ; i++) {
        if (strcmp(allVars[i],string) == 0) {
          inputIndexs[0] = i;
        }
      }
      fscanf(reader2 ,"%*[: ] %16s", string);                              // get the output and find out which index it is located at
      for(i = 0 ; i < totalSize ; i++) {
        if (strcmp(allVars[i],string) == 0) {
          outputIndexs[0] = i;
        }
      }
      if (root != NULL) {                                                  // make a new gateNode with the gate name, input and output
        ptr = malloc(sizeof(struct gateNode));
        ptr = root;
        newNode = malloc(sizeof(struct gateNode));
        newNode->name = gateName;
        newNode->inputVals = inputIndexs;
        newNode->outputVals = outputIndexs;
        newNode->next = NULL;
        while(ptr->next != NULL) {
          ptr = ptr->next;
        }
        ptr->next = newNode;
        ptr = NULL;
        newNode = NULL;
      } else {
        root = (struct gateNode*)malloc(sizeof(struct gateNode));
        root->name = gateName;
        root->inputVals = inputIndexs;
        root->outputVals = outputIndexs;
        root->next = NULL;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ( ((checkString = strcmp(string,"AND")) == 0) || ((checkString = strcmp(string,"OR")) == 0) || ((checkString = strcmp(string,"NAND")) == 0) || ((checkString = strcmp(string,"NOR")) == 0) || ((checkString = strcmp(string,"XOR")) == 0) ) {
      // will take 2 inputs and 1 output
      int gateName;
      int *inputIndexs = (int*)malloc(2*sizeof(int));
      int *outputIndexs = (int*)malloc(1*sizeof(int));
      if (strcmp(string,"AND") == 0) {
        gateName = 3;
      } else if (strcmp(string,"OR") == 0) {
        gateName = 4;
      } else if (strcmp(string,"NAND") == 0) {
        gateName = 5;
      } else if (strcmp(string,"NOR") == 0) {
        gateName = 6;
      } else {
        gateName = 7;
      }
      fscanf(reader2, "%*[: ] %16s", string);                              // get the first input and find out which index it is located at
      for(i = 0 ; i < totalSize ; i++) {
        if (strcmp(allVars[i],string) == 0) {
          inputIndexs[0] = i;
        }
      }
      fscanf(reader2, "%*[: ] %16s", string);                              // get the second input and find out which index it is located at
      for(i = 0 ; i < totalSize ; i++) {
        if (strcmp(allVars[i],string) == 0) {
          inputIndexs[1] = i;
        }
      }
      fscanf(reader2 ,"%*[: ] %16s", string);                              // get the output and find out which index it is located at
      for(i = 0 ; i < totalSize ; i++) {
        if (strcmp(allVars[i],string) == 0) {
          outputIndexs[0] = i;
        }
      }
      if (root != NULL) {                                                  // make a new gateNode with the gate name, input and output
        ptr = malloc(sizeof(struct gateNode));
        ptr = root;
        newNode = malloc(sizeof(struct gateNode));
        newNode->name = gateName;
        newNode->inputVals = inputIndexs;
        newNode->outputVals = outputIndexs;
        newNode->next = NULL;
        while(ptr->next != NULL) {
          ptr = ptr->next;
        }
        ptr->next = newNode;
        ptr = NULL;
        newNode = NULL;
      } else {
        root = (struct gateNode*)malloc(sizeof(struct gateNode));
        root->name = gateName;
        root->inputVals = inputIndexs;
        root->outputVals = outputIndexs;
        root->next = NULL;
      }
//___________________________________________________________________________________________________________________________________________________
    } else if ((checkString = strcmp(string,"DECODER")) == 0 ) {
      int gateName = 8;
      int numInputs;
      fscanf(reader2," %d",&numInputs);                                   // first number will tell us how many inputs and outputs
      int *inputIndexs = (int*)malloc((numInputs+1)*sizeof(int));        // need to make extra space in the front to keep the number of inputs
      int temp = numInputs;
      int totalOutputs = 1;
      while (temp > 0) { totalOutputs *= 2; temp--; }                         // will give us 2^n outputs
      int *outputIndexs = (int*)malloc((totalOutputs+1)*sizeof(int));         // need extra space in front to know how many outputs there are
      for (i = 1 ; i <= numInputs ; i++) {                                    // insert the right index in the input array
        fscanf(reader2 ,"%*[: ] %16s", string);
        int j;
        for( j = 0 ; j < totalSize ; j++) {
          if (strcmp(allVars[j],string) == 0) {
            inputIndexs[i] = j;
          }
        }
      }
      inputIndexs[0] = numInputs;
      for (i = 1 ; i <= totalOutputs ; i++) {                                   // insert the right index in the output array
        fscanf(reader2 ,"%*[: ] %16s", string);
        int j;
        for( j = 0 ; j < totalSize ; j++) {
          if (strcmp(allVars[j],string) == 0) {
            outputIndexs[i] = j;
          }
        }
      }
      outputIndexs[0] = totalOutputs;
      if (root != NULL) {                                                  // make a new gateNode with the gate name, input and output
        ptr = malloc(sizeof(struct gateNode));
        ptr = root;
        newNode = malloc(sizeof(struct gateNode));
        newNode->name = gateName;
        newNode->inputVals = inputIndexs;
        newNode->outputVals = outputIndexs;
        newNode->next = NULL;
        while(ptr->next != NULL) {
          ptr = ptr->next;
        }
        ptr->next = newNode;
        ptr = NULL;
        newNode = NULL;
      } else {
        root = (struct gateNode*)malloc(sizeof(struct gateNode));
        root->name = gateName;
        root->inputVals = inputIndexs;
        root->outputVals = outputIndexs;
        root->next = NULL;
      }
//___________________________________________________________________________________________________________________________________________________
      } else if ((checkString = strcmp(string,"MULTIPLEXER")) == 0 ) {
        int gateName = 9;
        int numSelectors;
        fscanf(reader2," %d",&numSelectors);                           // first number will tell us how many selectors
        int temp = numSelectors;
        int totalInputs = 1;
        while (temp > 0) { totalInputs *= 2; temp--; }                 // will give us 2^n inputs
        int sizeForArray = (1+totalInputs+numSelectors);
        int *inputIndexs = (int*)malloc(sizeForArray*sizeof(int));     // make space for num of selectors + inputs + selectors
        int *outputIndexs = (int*)malloc(1*sizeof(int));               // will be one output
        for (i = 1 ; i < sizeForArray ; i++) {                        // insert the right index in the input array
          fscanf(reader2 ,"%*[: ] %16s", string);
          int j;
          for( j = 0 ; j < totalSize ; j++) {
            if (strcmp(allVars[j],string) == 0) {
              inputIndexs[i] = j;
            }
          }
        }
        inputIndexs[0] = numSelectors;
        fscanf(reader2 ,"%*[: ] %16s", string);                         // will be the one output , put the right index in
        int j;
        for( j = 0 ; j < totalSize ; j++) {
          if (strcmp(allVars[j],string) == 0) {
            outputIndexs[0] = j;
          }
        }
        if (root != NULL) {                                                  // make a new gateNode with the gate name, input and output
          ptr = malloc(sizeof(struct gateNode));
          ptr = root;
          newNode = malloc(sizeof(struct gateNode));
          newNode->name = gateName;
          newNode->inputVals = inputIndexs;
          newNode->outputVals = outputIndexs;
          newNode->next = NULL;
          while(ptr->next != NULL) {
            ptr = ptr->next;
          }
          ptr->next = newNode;
          ptr = NULL;
          newNode = NULL;
        } else {
          root = (struct gateNode*)malloc(sizeof(struct gateNode));
          root->name = gateName;
          root->inputVals = inputIndexs;
          root->outputVals = outputIndexs;
          root->next = NULL;
        }
//___________________________________________________________________________________________________________________________________________________
      }
    checkScan = fscanf(reader2, " %s" , string);
  } // ends the while loop for Round 2
  fclose(reader2);

  // Round 3: now we have a loaded array for all values and a gateNode LL , so we can now start to make out truthtable and get outputs
  int rowSize = (inputs+outputs);
  int temp = inputs;
  int rows = 1;
  while (temp > 0) { rows *= 2; temp--; } // will give us the number of row (2^n)
  int **table = malloc(rows * sizeof(*table));
  int j , k;
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
  // use the evalute method to get the correct out put and insert it into the array accordingly
  for (i = 0; i <  rows ; i++) {
      for (j = 0; j < rowSize ; j++) {
        if ((j >= 0) && (j < inputs)){ // put in the inputs
          allVals[2+j] = table[i][j];
        }
      }
      allVals = evalute(allVals,root);
      for (k = 0 ; k < outputs ; k++){
        table[i][inputs+k] = allVals[2+inputs+k];
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
  // have to free everything
  for (i = 0 ; i < rows ; i++) {
    free(table[i]);
  }
  free(table);
  free(string);
  return 0;
} // ends the main()

//use the LL as a set of instructions to change the outputs
/*  KEY => GATE NAMES
      1 => PASS
      2 => NOT
      3 => AND
      4 => OR
      5 => NAND
      6 => NOR
      7 => XOR
      8 => DECODER
      9 => MULTIPLEXER
*/
int* evalute(int* mainArr , struct gateNode *root) {
  struct gateNode *behindPtr = NULL;
  struct gateNode *ptr = root;
  bool startOver = false;
  bool found = false;
  struct gateNode *behindFind = NULL;
  struct gateNode *find = NULL;

  while(ptr != NULL) {
//--------------------------------------------------------------------------------------------------------------------------------------------------
    if (ptr->name == 1) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
          mainArr[ptr->outputVals[0]] = mainArr[ptr->inputVals[0]];
      }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 2) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
        int param = mainArr[ptr->inputVals[0]];
        if (param == 1) {
          param = 0;
        } else {
          param = 1;
        }
        mainArr[ptr->outputVals[0]] = param;
      }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 3) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else if (mainArr[ptr->inputVals[1]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[1]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[1]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
        if ((mainArr[ptr->inputVals[0]] == 1) && (mainArr[ptr->inputVals[1]] == 1)){
          mainArr[ptr->outputVals[0]] = 1;
        } else {
          mainArr[ptr->outputVals[0]] = 0;
        }
      }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 4) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else if (mainArr[ptr->inputVals[1]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[1]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[1]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
        if ((mainArr[ptr->inputVals[0]] == 0) && (mainArr[ptr->inputVals[1]] == 0)){
          mainArr[ptr->outputVals[0]] = 0;
        } else {
          mainArr[ptr->outputVals[0]] = 1;
        }
      }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 5) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else if (mainArr[ptr->inputVals[1]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[1]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[1]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
      if ((mainArr[ptr->inputVals[0]] == 1) && (mainArr[ptr->inputVals[1]] == 1)){
        mainArr[ptr->outputVals[0]] = 0;
      } else {
        mainArr[ptr->outputVals[0]] = 1;
      }
    }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 6) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else if (mainArr[ptr->inputVals[1]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[1]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[1]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
      if ((mainArr[ptr->inputVals[0]] == 0) && (mainArr[ptr->inputVals[1]] == 0)){
        mainArr[ptr->outputVals[0]] = 1;
      } else {
        mainArr[ptr->outputVals[0]] = 0;
      }
    }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 7) {
      if (mainArr[ptr->inputVals[0]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[0]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[0]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else if (mainArr[ptr->inputVals[1]] == 2) {
        startOver = true;
        found = false;
        behindFind = ptr;
        find = ptr->next;
        while((find != NULL) && (found == false)) {
          if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
            if (find->outputVals[0] == ptr->inputVals[1]) {
              found = true;
              behindFind->next = find->next;
              if (behindPtr == NULL) {
                find->next = root->next;
                root->next = find;
                //swap names
                int temp = root->name;
                root->name = find->name;
                find->name = temp;
                //swap Inputs
                int* temp2 = root->inputVals;
                root->inputVals = find->inputVals;
                find->inputVals = temp2;
                //swap Outputs
                int *temp3 = root->outputVals;
                root->outputVals = find->outputVals;
                find->outputVals = temp3;
              } else {
                find->next = ptr;
                behindPtr->next = find;
              }
            }
          } else {
            int numbOutputs = find->outputVals[0];
            int i;
            for (i = 1 ; i <= numbOutputs ; i++) {
              if (find->outputVals[i] == ptr->inputVals[1]) {
                found = true;
                behindFind->next = find->next;
                find->next = ptr;
                if (behindPtr == NULL) {
                  root = find;
                } else {
                  behindPtr->next = find;
                }
              }
            }
          }
          if (found == false) {
            behindFind = find;
            find = find->next;
          } else {
            find = NULL;
            behindFind = NULL;
          }
        } // ends the while loop
      } else {
      if (mainArr[ptr->inputVals[0]] != mainArr[ptr->inputVals[1]]){
        mainArr[ptr->outputVals[0]] = 1;
      } else {
        mainArr[ptr->outputVals[0]] = 0;
      }
    }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 8) {
      int numbInputs = ptr->inputVals[0];
      int numbOutputs = ptr->outputVals[0];
      int j;
      for (j = 1 ; j <= numbInputs ; j++) {
        if (mainArr[ptr->inputVals[j]] == 2) {
          startOver = true;
          found = false;
          behindFind = ptr;
          find = ptr->next;
          while((find != NULL) && (found == false)) {
            if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
              if (find->outputVals[0] == ptr->inputVals[j]) {
                found = true;
                behindFind->next = find->next;
                if (behindPtr == NULL) {
                  find->next = root->next;
                  root->next = find;
                  //swap names
                  int temp = root->name;
                  root->name = find->name;
                  find->name = temp;
                  //swap Inputs
                  int* temp2 = root->inputVals;
                  root->inputVals = find->inputVals;
                  find->inputVals = temp2;
                  //swap Outputs
                  int *temp3 = root->outputVals;
                  root->outputVals = find->outputVals;
                  find->outputVals = temp3;
                } else {
                  find->next = ptr;
                  behindPtr->next = find;
                }
              }
            } else {
              int numbOutputs = find->outputVals[0];
              int i;
              for (i = 1 ; i <= numbOutputs ; i++) {
                if (find->outputVals[i] == ptr->inputVals[j]) {
                  found = true;
                  behindFind->next = find->next;
                  find->next = ptr;
                  if (behindPtr == NULL) {
                    root = find;
                  } else {
                    behindPtr->next = find;
                  }
                }
              }
            }
            if (found == false) {
              behindFind = find;
              find = find->next;
            } else {
              find = NULL;
              behindFind = NULL;
            }
          } // ends the while loop
          break;
        }
      }
      if (startOver == false) {
      int i , binary , product , temp;
      int total = 0;
      for (i = 0; i < numbInputs ; i++) {
        temp = numbInputs - 1 - i;
        binary = 1;
        while (temp > 0) { binary *= 2; temp--; }             // have to get the right binary number from left to right
        product = mainArr[ptr->inputVals[i+1]] * binary;
        total += product;                                     // total everything up so we can figure out which binary to go to
      }
      total++;                                                // the first index is used for a value so have to adjust the total for it
      for (i = 1 ; i <= numbOutputs ; i++) {
        if (i == total) {
          mainArr[ptr->outputVals[i]] = 1;
        } else {
          mainArr[ptr->outputVals[i]] = 0;
        }
      }
    }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    } else if (ptr->name == 9) {
      int i , binary , product , temp;
      int total = 0;
      int numbSelectors = ptr->inputVals[0];                  // number of selectors (n)
      temp = numbSelectors;
      int totalInputs = 1;
      while (temp > 0) { totalInputs *= 2; temp--; }          // number of inputs (2^n)
      int j;
      for (j = 0 ; j <= (numbSelectors+totalInputs) ; j++) {
        if (mainArr[ptr->inputVals[j]] == 2) {
          startOver = true;
          found = false;
          behindFind = ptr;
          find = ptr->next;
          while((find != NULL) && (found == false)) {
            if ((find->name == 1) || (find->name == 2) || (find->name == 3) || (find->name == 4) || (find->name == 5) || (find->name == 6) || (find->name == 7) || (find->name == 9)) {
              if (find->outputVals[0] == ptr->inputVals[j]) {
                found = true;
                behindFind->next = find->next;
                if (behindPtr == NULL) {
                  find->next = root->next;
                  root->next = find;
                  //swap names
                  int temp = root->name;
                  root->name = find->name;
                  find->name = temp;
                  //swap Inputs
                  int* temp2 = root->inputVals;
                  root->inputVals = find->inputVals;
                  find->inputVals = temp2;
                  //swap Outputs
                  int *temp3 = root->outputVals;
                  root->outputVals = find->outputVals;
                  find->outputVals = temp3;
                } else {
                  find->next = ptr;
                  behindPtr->next = find;
                }
              }
            } else {
              int numbOutputs = find->outputVals[0];
              int i;
              for (i = 1 ; i <= numbOutputs ; i++) {
                if (find->outputVals[i] == ptr->inputVals[j]) {
                  found = true;
                  behindFind->next = find->next;
                  find->next = ptr;
                  if (behindPtr == NULL) {
                    root = find;
                  } else {
                    behindPtr->next = find;
                  }
                }
              }
            }
            if (found == false) {
              behindFind = find;
              find = find->next;
            } else {
              find = NULL;
              behindFind = NULL;
            }
          } // ends the while loop
          break;
        }
      }
      if (startOver == false) {
      for (i = 0 ; i < numbSelectors ; i++){
        temp = numbSelectors - 1 - i;
        binary = 1;
        while (temp > 0) { binary *= 2; temp--; }
        product = mainArr[ptr->inputVals[1 + totalInputs + i]] * binary;
        total += product;
      }
      total++;                                                // have to account for the first number
      for (i = 1; i <= total;i++) {
        if (i == total) {
          mainArr[ptr->outputVals[0]] = mainArr[ptr->inputVals[i]];
        }
      }
    }
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------
    if (startOver == true) {
      behindPtr = NULL;
      ptr = root;
      startOver = false;
    } else {
      behindPtr = ptr;
      ptr = ptr->next;
    }
  } // ends the while loop
  return mainArr;
}
