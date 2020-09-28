#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NEQ, NUMBER, HEXNUMBER, REGISTER, AND, OR, POINTER, NEG, VALUE

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
	{"\\b[a-zA-Z_0-9]+", VALUE, 0},
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
uint32_t cal_val(char * tokens, bool * success);
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
			case VALUE:{
				bool flag;
				num = cal_val(tokens[l].str, &flag);
				if(!flag)num = -1;
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
