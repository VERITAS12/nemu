// #include "nemu.h"

// /* We use the POSIX regex functions to process regular expressions.
//  * Type 'man regex' for more information about POSIX regex functions.
//  */
// #include <sys/types.h>
// #include <regex.h>
// #include <stdlib.h>

// enum {
// 	NOTYPE = 256, EQ

// 	/* TODO: Add more token types */
//         , NUM, NEQ, OR, AND, REG, REF, NEG
// };

// static struct rule {
// 	char *regex;
// 	int token_type;
// } rules[] = {

// 	/* TODO: Add more rules.
// 	 * Pay attention to the precedence level of different rules.
// 	 */

// 	{" +",	NOTYPE},				// spaces
// 	{"\\+", '+'},					// plus
// 	{"==", EQ},						// equal
// 	{"0x[0-9a-fA-F]{1,8}", NUM},			// hex
// 	{"[0-9]{1,10}", NUM},					// dec
// 	{"\\$[a-z]{1,31}", REG},				// register names 
// 	{"-", '-'},
// 	{"\\*", '*'},
// 	{"/", '/'},
// 	{"%", '%'},
// 	{"!=", NEQ},
// 	{"&&", AND},
// 	{"\\|\\|", OR},
// 	{"!", '!'},
// 	{"\\(", '('},
// 	{"\\)", ')'} 
// };

// #define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

// static regex_t re[NR_REGEX];

// /* Rules are used for many times.
//  * Therefore we compile them only once before any usage.
//  */
// void init_regex() {
// 	int i;
// 	char error_msg[128];
// 	int ret;

// 	for(i = 0; i < NR_REGEX; i ++) {
// 		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
// 		if(ret != 0) {
// 			regerror(ret, &re[i], error_msg, 128);
// 			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
// 		}
// 	}
// }

// typedef struct token {
// 	int type;
// 	char str[32];
// } Token;

// Token tokens[32];
// int nr_token;

// static bool make_token(char *e) {
// 	int position = 0;
// 	int i;
// 	regmatch_t pmatch;
	
// 	nr_token = 0;

// 	while(e[position] != '\0') {
// 		/* Try all rules one by one. */
// 		for(i = 0; i < NR_REGEX; i ++) {
// 			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
// 				char *substr_start = e + position;
// 				int substr_len = pmatch.rm_eo;

// 				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
// 				position += substr_len;

// 				/* TODO: Now a new token is recognized with rules[i]. Add codes
// 				 * to record the token in the array `tokens'. For certain types
// 				 * of tokens, some extra actions should be performed.
// 				 */

// 				switch(rules[i].token_type) {
//                                         case NOTYPE: break;
//                                         case NUM:
// 					//default: panic("please implement me");
//                                         case REG: sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
// 					default: tokens[nr_token].type = rules[i].token_type;
// 							 nr_token ++;
// 				}

// 				break;
// 			}
// 		}

// 		if(i == NR_REGEX) {
// 			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
// 			return false;
// 		}
// 	}

// 	return true; 
// }

// /*TODO: Expression evaluation*/

// static int op_prec(int t) {
// 	switch(t) {
// 		case '!': case NEG: case REF: return 0;
// 		case '*': case '/': case '%': return 1;
// 		case '+': case '-': return 2;
// 		case EQ: case NEQ: return 4;
// 		case AND: return 8;
// 		case OR: return 9;
// 		default: assert(0);
// 	}
// }

// static inline int op_prec_cmp(int t1, int t2) {
// 	return op_prec(t1) - op_prec(t2);
// }

// static int find_dominated_op(int s, int e, bool *success) {
// 	int i;
// 	int bracket_level = 0;
// 	int dominated_op = -1;
// 	for(i = s; i <= e; i ++) {
// 		switch(tokens[i].type) {
// 			case REG: case NUM: break;

// 			case '(': 
// 				bracket_level ++; 
// 				break;

// 			case ')': 
// 				bracket_level --; 
// 				if(bracket_level < 0) {
// 					*success = false;
// 					return 0;
// 				}
// 				break;

// 			default:
// 				if(bracket_level == 0) {
// 					if(dominated_op == -1 || 
// 							op_prec_cmp(tokens[dominated_op].type, tokens[i].type) < 0 ||
// 							(op_prec_cmp(tokens[dominated_op].type, tokens[i].type) == 0 && 
// 							 tokens[i].type != '!' && tokens[i].type != '~' &&
// 							 tokens[i].type != NEG && tokens[i].type != REF) ) {
// 						dominated_op = i;
// 					}
// 				}
// 				break;
// 		}
// 	}

// 	*success = (dominated_op != -1);
// 	return dominated_op;
// }

// uint32_t get_reg_val(const char*, bool *);

// static uint32_t eval(int s, int e, bool *success) {
// 	if(s > e) {
// 		// bad expression
// 		*success = false;
// 		return 0;
// 	}
// 	else if(s == e) {
// 		// single token
// 		uint32_t val;
// 		switch(tokens[s].type) {
// 			case REG: val = get_reg_val(tokens[s].str + 1, success);	// +1 to skip '$'
// 					  if(!*success) { return 0; }
// 					  break;

// 			case NUM: val = strtol(tokens[s].str, NULL, 0); break;

// 			default: assert(0);
// 		}

// 		*success = true;
// 		return val;
// 	}
// 	else if(tokens[s].type == '(' && tokens[e].type == ')') {
// 		return eval(s + 1, e - 1, success);
// 	}
// 	else {
// 		int dominated_op = find_dominated_op(s, e, success);
// 		if(!*success) { return 0; }

// 		int op_type = tokens[dominated_op].type;
// 		if(op_type == '!' || op_type == NEG || op_type == REF) {
// 			uint32_t val = eval(dominated_op + 1, e, success);
// 			if(!*success) { return 0; }

// 			switch(op_type) {
// 				case '!': return !val;
// 				case NEG: return -val;
// 				case REF: return swaddr_read(val, 4);
// 				default: assert(0);
// 			}
// 		}

// 		uint32_t val1 = eval(s, dominated_op - 1, success);
// 		if(!*success) { return 0; }
// 		uint32_t val2 = eval(dominated_op + 1, e, success);
// 		if(!*success) { return 0; }

// 		switch(op_type) {
// 			case '+': return val1 + val2;
// 			case '-': return val1 - val2;
// 			case '*': return val1 * val2;
// 			case '/': return val1 / val2;
// 			case '%': return val1 % val2;
// 			case EQ: return val1 == val2;
// 			case NEQ: return val1 != val2;
// 			case AND: return val1 && val2;
// 			case OR: return val1 || val2;
// 			default: assert(0);
// 		}
// 	}
// }

// /* TODO: Expression evaluation end */

// uint32_t expr(char *e, bool *success) {
// 	if(!make_token(e)) {
// 		*success = false;
// 		return 0;
// 	}

// 	/* TODO: Insert codes to evaluate the expression. */
//        	//panic("please implement me");
// 	//return 0;
//         /* Detect REF and NEG tokens */
// 	int i;
// 	int prev_type;
// 	for(i = 0; i < nr_token; i ++) {
// 		if(tokens[i].type == '-') {
// 			if(i == 0) {
// 				tokens[i].type = NEG;
// 				continue;
// 			}

// 			prev_type = tokens[i - 1].type;
// 			if( !(prev_type == ')' || prev_type == NUM || prev_type == REG) ) {
// 				tokens[i].type = NEG;
// 			}
// 		}

// 		else if(tokens[i].type == '*') {
// 			if(i == 0) {
// 				tokens[i].type = REF;
// 				continue;
// 			}

// 			prev_type = tokens[i - 1].type;
// 			if( !(prev_type == ')' || prev_type == NUM || prev_type == REG) ) {
// 				tokens[i].type = REF;
// 			}
// 		}
// 	}

// 	return eval(0, nr_token - 1, success);
// }

#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NEQ, NUMBER, HEXNUMBER, REGISTER, AND, OR, POINTER, NEG

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE, 0},				// spaces
	{"	+", NOTYPE, 0},
	{"\\b[0-9]+\\b", NUMBER, 0},
	{"\\b0[xX][0-9a-fA-F]+\\b", HEXNUMBER, 0},
	{"\\$[a-zA-Z]+", REGISTER, 0},
	{"\\+", '+', 4},					// plus
	{"==", EQ, 3},						// equal
	{"!=", NEQ, 3},
	{"!", '!', 6},
	{"-", '-', 4},
	{"\\*", '*', 5},
	{"/", '/', 5},
	{"&&", AND, 2},
	{"\\|\\|", OR, 1},
	{"\\(", '(', 7},
	{"\\)", ')',7},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {//Travrse all the rules.
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);//
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	uint32_t val;
	int priority;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	printf("make token with %s\n", e);
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {//shuo ming dang qian wei zhi gang hao pi pei dao
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;//ben duan pi pei zi fu de chang du
				char *temp = e + position + 1;//xia yi ge zi fu kai shi de wei zhi
				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case REGISTER:// record
						tokens[nr_token].type = rules[i].token_type;
						tokens[nr_token].priority = rules[i].priority;
						strncpy(tokens[nr_token].str, temp, substr_len - 1);
						tokens[nr_token].str[substr_len-1] = '\0';
						nr_token++;		
						break;
					default:
						//printf("here\n");
						tokens[nr_token].type = rules[i].token_type;
						tokens[nr_token].priority = rules[i].priority;
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						//printf("%s\n",tokens[nr_token].str);
						tokens[nr_token].str[substr_len] = '\0';
						nr_token++;
				}
				position += substr_len;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	//printf("%s\n", tokens[0].str);
	for(i = 0; i < nr_token; i++){
		if(i == 0 || (tokens[i - 1].type != NUMBER && tokens[i - 1].type != HEXNUMBER && tokens[i - 1].type != ')' && tokens[i - 1].type != REGISTER)){
			if(tokens[i].type == '-'){printf("neg\n");tokens[i].type = NEG;tokens[i].priority = 6;}
			else if(tokens[i].type == '*'){tokens[i].type = POINTER;tokens[i].priority = 6;}
		}
	}
	//printf("get %d\n", nr_token);
	//for(i = 0; i < nr_token;i++)printf("%s", tokens[i].str);printf("\n");
	return true; 
}

bool check_parentheses(int l, int r){
	if(tokens[l].type == '('&&tokens[r].type == ')'){
		int i = l + 1,cnt_l = 0, cnt_r = 0;
		for(;i < r; i++){
			if(tokens[i].type == '(')cnt_l++;
			if(tokens[i].type  == ')')cnt_r++;
			if(cnt_r > cnt_l)return false;
		}
		if(cnt_l == cnt_r)return true;
	}
	return false;
}

int dominant_operator(int l, int r){
	int i, j, op = l, min = 100;
	int min_t = 100;
	for(i = l;i <= r;i++)if(min_t>tokens[i].priority&&tokens[i].priority!=0)min_t = tokens[i].priority;
	printf("min: %d\n", min_t);
	if(min_t == 6)for(i = l;i <= r;i++)if(min_t == tokens[i].priority)return i;
	for(i = l;i <= r;i++){
		if(tokens[i].type == NUMBER || tokens[i].type == HEXNUMBER || tokens[i].type == REGISTER)continue;
		int cnt = 0;
		bool flag = true;
		for(j = i + 1;j <= r;j++)
			if(tokens[j].type == '(')cnt++;
			else if(tokens[j].type == ')'&&cnt == 0){flag = false;break;}
			else if(tokens[j].type == ')')cnt--;
		if(!flag)continue;
		if(tokens[i].priority <= min)min = tokens[i].priority,op = i;
	}
	return op;
}
uint32_t eval(int l, int r){
	if(r < l){
		printf("Something terriable has happened(r < l), please check!\n");
	}else if(l == r){
		uint32_t num = 0;
		switch(tokens[l].type){
			case NUMBER: sscanf(tokens[l].str, "%d", &num);break;
			case HEXNUMBER: sscanf(tokens[l].str, "%x", &num);break;
			case REGISTER:{
				int i;
				bool flag = false;
				for(i = R_EAX; i <= R_EDI; i++)
					if(strcmp(tokens[l].str, regsl[i]) == 0)num = reg_l(i), flag = true;
				for(i = R_AX; i <= R_DI; i++)
					if(strcmp(tokens[l].str, regsw[i]) == 0)num = reg_w(i), flag = true;
				for(i = R_AL; i <= R_BH; i++)
					if(strcmp(tokens[l].str, regsb[i]) == 0)num = reg_b(i), flag = true;
				if(!flag)printf("The REGISTER doesn't exist\n");
				break;
			}
			default: printf("eval defalut output!!!\n");
	}
		return num;
	}else if(check_parentheses(l, r)){
		return eval(l + 1, r - 1);
	}else{
		
		int op = dominant_operator(l, r);
		if(tokens[op].type == POINTER||tokens[op].type == NEG || tokens[op].type == '!'){
			uint32_t val = eval(op + 1, r);
			switch(tokens[op].type){
				case POINTER: return swaddr_read(val, 4);
				case NEG: return -val;
				case '!': return !val;
				default : printf("point neg default\n");
			}
		}
		int val1 = eval(l, op -1), val2 = eval(op + 1, r);
		switch(tokens[op].type){
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			case AND: return val1 && val2;
			case OR: return val1 || val2;
			case EQ: return val1 == val2;
			case NEQ: return val1 != val2;
			default: assert(0);
		}
	}
	return -1;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	*success = true;
	return eval(0, nr_token - 1);
}
