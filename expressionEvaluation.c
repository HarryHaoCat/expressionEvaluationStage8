/*************************************************************************
> File Name: expressionEvaluation.c
> Author: HaoJie
> Mail: 954487858@qq.com
> Created Time: 2018/08/14  17h22m3s
************************************************************************/
#include <stdio.h>
#include "sequentialStack.h"
#include <ctype.h>
State IsLegal(char* infixExpression, int index, int* space, char preChar, int* braceNum); //Check if the expression is legal.
void printError(char* infixExpression, int index);    									  //Mark the location of error.
/*
The function converts infix expression into postfix expression.
Firstly, check if the infix expression is legal, if it's illegal, print error information and return FAILED.
If the infix expresssion is legal, convert it into postfix expression and retuen SUCCEEDED.
*/
State infixToPostfix(char* infixExpression, char postfixExpression[]) {
	for (int i = 0; i < 100; i++)
		postfixExpression[i] = '\0';
	char preChar = 's';
	SequentialStackChar* s = (SequentialStackChar*)malloc(sizeof(SequentialStackChar));
	ElemType e;
	if (InitStack(s) == FAILED) {
		printf("<Error! Initialize stack fails>\n");
		return FAILED;
	}
	int numIndex = 0, index = 0, braceNum = 0;
	int space = 0, leftBrace = 0, rightBrace = 0;
	while (infixExpression[index] != '\0') {
		//Check if the expression is legal.
		if (IsLegal(infixExpression, index, &space, preChar, &braceNum) == FAILED)
			return FAILED;
		char ch = infixExpression[index];
		switch(ch){
			//If the current character is space or '\t', the index pluses one and continue. 
			case ' ' :
			case '\t':
						index++;
						continue;
			//If the current character is brace '(', push it into the stack. 
			case '(': 
						leftBrace++;
					    if (Push(s, ch) == FAILED) {
						    printf("<Error! The stack is full>\n");
						    DestroyStack(s);
						    return FAILED;
					    }	
					    break;
			/*If the current character is brace ')', pop operators from the stack to the postfix expression
			until meeting brace '(', and pop the brace '(' out of the stack at the end.
			*/		    
			case ')':	
						rightBrace++;
						if(GetTop(s, &e) == FAILED){
							printf("<Error! The stack is empty>\n");
							DestroyStack(s);
							return FAILED;
						} 	
						while(e != '('){
						  	if (Pop(s, &postfixExpression[numIndex++]) == FAILED){
								printf("<Error! The stack is empty>\n");
								DestroyStack(s);
								return FAILED;
							} 
					 		if(GetTop(s, &e) == FAILED){
					 			printf("<Error! The stack is empty>\n");
								DestroyStack(s);
						        return FAILED;
					        } 
						}
					  	if (Pop(s, &e) == FAILED){
							printf("<Error! The stack is empty>\n");
							DestroyStack(s);
							return FAILED;     
						}
					    break;
			/*If the current character is operator '+' or '-',
				check if it's a unary operator, if it is, push '@' or '$' into the stack respectively,
				else when the stack isn't empty and top char of the stack isn't '(', pop the top char 
				of the stack into the postfix expression, finally, push the current operator into the stack.
			*/		    
			case '+': 
		    case '-':
						if((preChar == '(' || preChar == 's') && (isdigit(infixExpression[index + 1]) || infixExpression[index + 1] == '('))
							ch = ch == '+' ? '@' : '$'; 	    
			    	    else{
			    	    	while(StackEmpty(s) == FAILED &&(GetTop(s, &e) == SUCCEEDED) && e != '(') {  
								if (Pop(s, &postfixExpression[numIndex++]) == FAILED){
									printf("<Error! The stack is empty>\n");
									DestroyStack(s);
									return FAILED;
								} 
							} 
						} 
						if(Push(s, ch) == FAILED) {
						    printf("<Error! The stack is full>\n");
							DestroyStack(s);
							return FAILED;
						}     
						break;
			/*If the current character is '*' or '/', 
				when the stack is not empty and the top character of the stack is '*', '/', '@' or '$',
				pop the top character into the postfix expression, finally, push the current operator into the stack.
			*/			
			case '*':
			case '/':
			            while(StackEmpty(s) == FAILED && (GetTop(s, &e) == SUCCEEDED) && (e == '/' || e == '*' || e == '@' || e == '$')) {  
					  		if(Pop(s, &postfixExpression[numIndex++]) == FAILED){
					  			printf("<Error! The stack is empty>\n");
								DestroyStack(s);
								return FAILED;
					  		} 	
					   	} 
					   	if(Push(s, ch) == FAILED) { 
						    printf("<Error! The stack is full>\n");
							DestroyStack(s);
							return FAILED;
						}
					  	break;
			//If the current character is digit, put the number in the postfix expression. 
			default :
			            while (isdigit(ch))	{
					 		postfixExpression[numIndex++] = ch; 
					 		index++;
					 		ch = infixExpression[index];
					 	}      		    
		    	      	index--;
		    	      	postfixExpression[numIndex++]='#';
		}
		preChar = infixExpression[index];	
		index++;	
	}
	//Move the rest characters from the char stack to the postfix expression.
	while (StackEmpty(s) == FAILED) {
		if (Pop(s, &postfixExpression[numIndex++]) == FAILED) {
			printf("<Error! The stack is empty>\n");
			DestroyStack(s);
			return FAILED;
		}
	}
	//DestroyStack the char stack.
	DestroyStack(s);
	//Check if braces are matched.
 	if (leftBrace != rightBrace) {
 		leftBrace < rightBrace ? printf("<Error! No matched \'(\' before \')\'!!!>\n") 
 		                       : printf("<Error! No matched \')\' after \'(\'>\n");
		return FAILED;
	}
	//Check if the input is empty.
	if (space == index) {
		printf("<Error! The input is empty>\n");
		return FAILED;
	}
	else return SUCCEEDED;
}
/*
Compute the value of postfix expression.
Traverse the postfix expression, if it's an operand, push it into a double stack.
If it's a binary operator, pop two operands from the double stack, and calculate them, then push it back to the double stack.
If it's a unary operator, pop one operand from the double stack, and calculate it, then push it back to the double stack.
when traversing is over, pop the result from the stack to the value.
*/
State computeValueFromPostfix(char* postfixExpression, double *value) {
	int index = 0;
	SequentialStackDouble* ds = (SequentialStackDouble*)malloc(sizeof(SequentialStackDouble));
	if (InitStackDouble(ds) == FAILED) {
		printf("<Error! Initialize stack fails>\n");
		return FAILED;
	}
	double num1 = 0.0, num2 = 0.0;
	int temp;
	double tempValue;
	while (postfixExpression[index] != '\0') {
		switch(postfixExpression[index]){
			//If the current character is a '#', then skip it.
			case '#':
						index++;
						continue;
			/*If the current character is operator '+', '-', '*', '/', get two operands from the stack to calculate, 
			then put it back to the stack.
			*/
			case '+':
			case '-':
			case '*':
			case '/':
						if (PopDouble(ds, &num1) == FAILED) {
							printf("<Error! The stack is empty>\n");
							DestroyStackDouble(ds);
							return FAILED;
						}
						if (PopDouble(ds, &num2) == FAILED) {
							printError(postfixExpression, index);
							printf("<Error! Operator \'%c\' lacks opreand>\n", postfixExpression[index]);
							DestroyStackDouble(ds);
							return FAILED;
						}
						switch (postfixExpression[index]) {
						case ('+'): if (PushDouble(ds, num2 + num1) == FAILED) { printf("<Error! The stack is full>\n"); return FAILED; }break;
						case ('-'): if (PushDouble(ds, num2 - num1) == FAILED) { printf("<Error! The stack is full>\n"); return FAILED; }break;
						case ('*'): if (PushDouble(ds, num2 * num1) == FAILED) { printf("<Error! The stack is full>\n"); return FAILED; }break;
						case ('/'): if (num1 != 0) {
										if (PushDouble(ds, (double)num2 / num1) == FAILED) {
											printf("<Error! The stack is full>\n");
											return FAILED;
										}
										break;
									}
									else {
										printError(postfixExpression, index);
										printf("<Error! Some operand is divided by zero in the expression>\n");
										return FAILED;
									}
						default: break;
						}break;
			//If the current character is '@' or '$', get one operand from the stack to calculate, then put the result back to stack.
			case '@':
			case '$':
						if (PopDouble(ds, &num1) == FAILED) {
							printf("<Error! The stack is empty>\n");
							DestroyStackDouble(ds);
							return FAILED;
						}
						switch (postfixExpression[index]) {
						case ('@'): if (PushDouble(ds, num1) == FAILED) { printf("<Error! The stack is full>\n"); return FAILED; }break;
						case ('$'): if (PushDouble(ds, 0 - num1) == FAILED) { printf("<Error! The stack is full>\n"); return FAILED; }break;
						default:break;
						}break;
			//If the current character is digit, push the number into the double stack.
			default:
						tempValue = (double)(postfixExpression[index] - '0');
						temp = index + 1;
						while (postfixExpression[temp] != '#') {
							tempValue = 10.0 *tempValue + (postfixExpression[temp] - '0');
							temp++;
							index++;
						}
						if (PushDouble(ds, tempValue) == FAILED) {
							printf("<Error! The stack is full>\n");
							DestroyStackDouble(ds);
							return FAILED;
						}	
		}
		index++;
	}	
	//Clear the postfix expression
	for (int i = 0; i < 100; i++)
		postfixExpression[i] = '\0';
	*value = 0.0;
	//Get the result from the stack, then give it to value. 
	if (PopDouble(ds, value) == FAILED) {
		printf("<Error! The stack is empty>\n");
		DestroyStackDouble(ds);
		return FAILED;
	}
	DestroyStackDouble(ds);
	return SUCCEEDED;
}

//Check if the expression is legal. 
State IsLegal(char* infixExpression, int index, int* space, char preChar, int* braceNum) {
	/*
	Check whether the current character is illegal,
	if the current character isn't '+', '-', '*', '/', ' ', '\t', '(', ')' or digit '0'-'9',
	then print proper error information and return FAILED to indicate illegal characters in the expression.
	*/
	char ch = infixExpression[index];
	if (!(isdigit(ch) || ch == '+' || ch == ' '|| ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '\t')) {
		printError(infixExpression, index);
		if ((ch > 0 && ch < 9)|| (ch >= 10 && ch< 32) || ch == 127){
			if (ch == '\b') 
				printf("<Error! Illegal escape character \'\\%o\' after \'%c\' in the expression>\n", ch, infixExpression[index - 2]);
			else printf("<Error! Illegal escape character \'\\%o\' in the expression>\n", ch);
		}
		else printf("<Error! Illegal character \'%c\' in the expression>\n", ch);
		return FAILED;
	}
	else{
		switch(ch){
			/*
			Handle space and '\t'.
			If the last character is space or '\t' with existing operator before it, return FAILED and error message.
			*/
			case ' ' :
			case '\t':
						(*space)++;
						if(infixExpression[index + 1] == '\0' && (preChar == '+' || preChar == '-' || preChar == '*' || preChar == '/')){
							printError(infixExpression, index - 1);
							printf("<Error! No operand after operator \'%c\'>\n", preChar);
							return FAILED;
						}
						break;
			/*Handle illegal unary operator '+' and '-'.
			(1)A space follows a unary operator '+' or '-'.
			(2)An operaor '+', '-', '*', '/' follows an operator '+' or '-'.
			(3)There is only one operator '+' or '-' in the expression without operand.
			*/
			case '+':
			case '-':
						if ((preChar == 's' || preChar== '(') && (infixExpression[index + 1] == ' ' || infixExpression[index + 1] == '\t')) {
							printError(infixExpression, index + 1);
							printf("<Error! A space follows a unary \'%c\'>\n", ch);
							return FAILED;
						}
						else if (preChar == '+' || preChar == '-' || preChar == '*' || preChar == '/') {
							printError(infixExpression, index);
							printf("<Error! Operator \'%c\'  follows another operator \'%c\' in the expression>\n", ch, preChar);
							return FAILED;
						}
						else if(infixExpression[index + 1] == '\0'){
							printError(infixExpression, index);
							printf("<Error! No operand after operator \'%c\'>\n", ch);
							return FAILED;
						}
						break;
			/*
			Handle illegal '*' and '/'
			(1)No opreands before operator '*' or '/'.
			(2)The current operator '*','/','+' or '-' follows another operator '*' or '/'.
			(3)No opreand after operator '*' or '/'.
			*/
			case '*':
			case '/':
						if (preChar == 's' || (preChar != ')' && !isdigit(preChar) && preChar != '+' && preChar != '-' && preChar != '*' && preChar != '/')) {
							printError(infixExpression, index);
							printf("<Error! No operand before operator \'%c\'>\n", ch);
							return FAILED;
						}
						else if (preChar == '+' || preChar == '-' || preChar == '*' || preChar == '/') {
							printError(infixExpression, index);
							printf("<Error! Operator \'%c\'  follows another operator \'%c\' in the expression>\n", ch, preChar);
							return FAILED;
						}
						else if (infixExpression[index + 1] == '\0') {
							printError(infixExpression, index);
							printf("<Error! No operand after operator \'%c\'>\n", ch);
							return FAILED;
						}
						break;
			/*
			Handle illegal brace ')'.
			(1)There is an operator before brace ')' without operand.
			(2)No expression in the braces.
			(3)No matched brace before the current brace.
			*/
			case ')':
						(*braceNum)--;
						if(preChar == '+' || preChar == '-' || preChar == '*' || preChar == '/'){
							printError(infixExpression, index - 1);	
							printf("<Error! No operand after operator \'%c\'>\n", preChar);
							return FAILED;
						}
						else if(preChar == '('){
							printError(infixExpression, index - 1);	
							printf("<Error! No expression in the braces>\n");
							return FAILED;				}
						else if (preChar == 's' || (*braceNum) < 0) {
							printError(infixExpression, index);
							printf("<Error! No matched \'(\' before \')\'>\n");
							return FAILED;
						}
						break;
			/*
			Handle illegal brace '('.
			(1)There is a brace ')' before a '(' without operator between them.
			(2)There is a digit before a '(' without operator between them.
			*/
			case '(':
						(*braceNum)++;
						if (preChar == ')') {
							printError(infixExpression, index);
							printf("<Error! No operator between \')\' and \'(\'>\n");
							return FAILED;
						}
						else if (isdigit(preChar)) {
							printError(infixExpression, index);
							printf("<Error! No operator between operand and \'(\'>\n");
							return FAILED;
						} 
						break;
			/*
			If the current character is a digit,
				(1)the effective character before it is also a digit and no operator between them,
				(2)a brace ')' before a digit, and there is no operator between them,   
			else it's a legal digit, put it into the array postfixExpression.
			*/
			default:
						if ((infixExpression[index - 1] == ' ' || infixExpression[index - 1] == '\t')&& isdigit(preChar)) {
							printError(infixExpression, index - 1);
							printf("<Error! A space between two operands>\n");
							return FAILED;
						}
						else if(preChar == ')'){
							printError(infixExpression, index);
							printf("<Error! No operator between \')\' and operand>\n");
							return FAILED;			
						}
						break;
		}
	}
	return SUCCEEDED;
}
/*Mark the index of error.
  The arguments of the function are the address of the infix expression and the location of error. 
  Print a sign '^' below the error.
*/
void printError(char* infixExpression, int index) {
	printf("\"");
	printf("%s", infixExpression);
	printf("\"\n");
	int i = 0;
	printf(" ");
	if (infixExpression[index] == '\b')
		index = index - 2;
	while (i < index) {
		if (infixExpression[i] == '\t'){
			printf("\t");		}
		else printf(" ");
		i++;
	}
	printf("^\n");
}
