/*Ghazal Ronagh */
/*This program tries to find the transactions that might be fradulent by cheking
if they passed the daily limit and also checking the time, if 2 transaction has
occured in the same time but different locations, one of them are likely to be
fradulent. This program arranged stage by stage and I uploaded test0 and test1 
as well to be able to test this program with them */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
 /* stage numbers */ 
#define STAGE_NUM_ONE 1			                 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
/* stage header format string */
#define STAGE_HEADER "=========================Stage %d=========================\n"	
#define MAXCARDS 100  /* maximum number of cards */
#define CARD_ID_LEN 8        
#define TRANSC_ID_LEN 12
#define DATE_TIME_LEN 19
#define DATE_LEN 10

/****************************************************/
/* This struct represent a credit card records */
typedef struct{
	 char card_id[CARD_ID_LEN +1];    /*it is alphanumeric*/
	 int daily_lim;                   /*positive integer*/
	 int transc_lim;                  /*positive integer*/
	 int used_daily_transc_amount;
	 char last_transc_datetime[DATE_TIME_LEN+1];
}cardrec_t;

/*This struct represent each transaction informations */
typedef struct{
	char transc_id[TRANSC_ID_LEN +1];
	char card_id[CARD_ID_LEN +1];
	char datetime[DATE_TIME_LEN+1];
	int transc_amount;
}transc_t;

/*This struct represent transaction nodes */
typedef struct transc_node{
	transc_t data;
	struct transc_node *next;
}transc_node_t;

/*This struct represent transactions list */
typedef struct {
	transc_node_t *head;
	transc_node_t *foot;
}transc_list_t;

/**************************************************************/
/* function prototypes */
void read_one_card(cardrec_t *card);
void print_stage_header(int stage_num);
/*stages */
void stage_one(cardrec_t *card);
void stage_two(cardrec_t cards[],int *card_num);
transc_list_t * stage_three(transc_list_t *transc_list);
void stage_four(transc_list_t *transc_list, cardrec_t cards[], int num_cards);
/*functions */
double avg_daily_lim(cardrec_t cards[], int num_cards);
char* largest_transc_lim(cardrec_t cards[], int num_cards);
int read_one_transc(transc_t *transc);
void insert_transc_at_foot(transc_list_t *transc_list,transc_t transc);
void check_transc_limits(transc_t transc, cardrec_t cards[], int num_cards);
int compare_card_ids (const void* key, const void* elem);

/****************************************************************/
int
main(int argc, char *argv[]) {
	 /* to hold all card record inputs*/
	 cardrec_t cards[MAXCARDS];	
	 transc_list_t *transc_list = NULL;
	/* to hold the number of card record inputs */
	 int num_cards = 0;	
	/* stage 1:reading the first credit card record from the input data */
	stage_one(&cards[0]); 
	num_cards ++;
	/*stage2: read all credit card records and store those in an array */
	stage_two(cards, &num_cards);
	/*stage 3: read all transactions and store them in linked list */
	transc_list = stage_three(transc_list);
    /*stage 4: checking the limits,searching for card ID by binary search */
	stage_four(transc_list,cards,num_cards);
	return 0;
}

/***************************************************************/
/*read one line (one card record) into card  */
void 
read_one_card(cardrec_t card[]){  
    scanf("%s %d %d", card->card_id, &card->daily_lim, &card->transc_lim);
}
/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* stage 1: reading the first credit card record */
void 
stage_one(cardrec_t *card) {
	print_stage_header(STAGE_NUM_ONE);
	/*read the credit card record */
	read_one_card(card);
    
	/* print the required fields */	
	printf("Card ID: %s\n", card->card_id);
    printf("Daily limit: %d\nTransaction limit: %d\n\n", card->daily_lim, 
      card->transc_lim);   
}

/****************************************************************/
/*stage 2: read all credit card records and store those in "cards" array */ 
void 
stage_two(cardrec_t cards[],int *num_cards){
 	int i=*num_cards;
    
	print_stage_header(STAGE_NUM_TWO);
    
 	/*read the card records and stop when it reach the separating line*/
	read_one_card(&cards[i]);  
	while (strcmp(cards[i].card_id, "%%%%%%%%%%")){	
		i++;
		read_one_card(&cards[i]);  
	}
	*num_cards=i;
    
	/*print the required information */
	printf("Number of credit cards: %d\n", *num_cards); 
	printf("Average daily limit: %.2f\n", 
	 avg_daily_lim(cards,*num_cards));
	printf("Card with the largest transaction limit: %s\n",
	 largest_transc_lim(cards, *num_cards));
	printf("\n");	
}

/*************/
/*this function calcultes the average of daily limits */
double avg_daily_lim(cardrec_t cards[], int num_cards){

	int i;
	double sum_daily_limit=0;

	for (i = 0; i < num_cards ; i++) {
		sum_daily_limit += cards[i].daily_lim;
    }
    return sum_daily_limit / num_cards;
}

/* This function finds the largest transaction limit and in case of tie it 
returns the one that its card ID is smaller */
char *largest_transc_lim(cardrec_t cards[], int num_cards){
    int i,largest_index=0;
    for (i=0 ; i < num_cards; ++i) {
        if (cards[i].transc_lim > cards[largest_index].transc_lim){
            largest_index = i;
        }
    }
    return cards[largest_index].card_id; 
}

/**************************************************/
/*stage 3: read all transactions and store them in linked data structure */
transc_list_t *
stage_three(transc_list_t *transc_list){
	transc_t transc;
	transc_node_t *transc_node;
	print_stage_header(STAGE_NUM_THREE);
	
	transc_list=(transc_list_t *)malloc(sizeof(transc_list));
    /* make empty list */
	transc_list->head=NULL;
	transc_list->foot=NULL;
    
	while(read_one_transc(&transc)!=EOF) {
		insert_transc_at_foot(transc_list,transc);
	}
	for(transc_node=transc_list->head;transc_node!=NULL;
	 transc_node=transc_node->next) {
		printf("%s\n",transc_node->data.transc_id);
	}
	printf("\n");
	return transc_list;
}

/*****************/
/*read one transaction record into transc */
int
read_one_transc(transc_t *transc){
	return scanf("%s %s %s %d ", transc->transc_id, transc->card_id,
		transc->datetime, &transc->transc_amount);
}
/* insert new item (new transaction) from foot */
void
insert_transc_at_foot(transc_list_t *transc_list,transc_t transc){
	transc_node_t *new_node;

	new_node = (transc_node_t*)malloc(sizeof(*new_node));
	new_node -> data  = transc;
	new_node -> next  = NULL;
    /*check if the list is empty or not */
	if( transc_list->head == NULL ){
		transc_list->head=transc_list->foot=new_node;
	}
	else{
		transc_list->foot->next=new_node;
		transc_list->foot=new_node;
    }

}

/***********************************************************/
/*stage 4: checking for fradulent transcations by checking the limits and
then searching for credit card ID by binary search */
void
stage_four(transc_list_t *transc_list, cardrec_t cards[], int num_cards){
	
	transc_node_t *transc_node;
	int i;
	print_stage_header(STAGE_NUM_FOUR);
	
	for(i=0;i<num_cards;i++) {
		cards[i].used_daily_transc_amount=0;
		strcpy(cards[i].last_transc_datetime,"0000:00:00");
	}
	
	for(transc_node=transc_list->head;transc_node!=NULL;
     transc_node=transc_node->next) {
		check_transc_limits(transc_node->data,cards,num_cards);								
	}
}

/***********/
/*This function checks the transc limit by comparing it to daily limit and 
transaction limit and print the result of checking limits */
void check_transc_limits(transc_t transc, cardrec_t cards[], int num_cards){
	printf("%s             ",transc.transc_id);
	cardrec_t *card=bsearch(transc.card_id,cards,num_cards,sizeof(cardrec_t),
    compare_card_ids);
    
	/* checks if Previous transaction of this card is today */
	if(!strncmp(card->last_transc_datetime,transc.datetime,DATE_LEN)) {  
		card->used_daily_transc_amount+=transc.transc_amount;
	}
    /* check for the reverse situation that is
    when Previous transaction of this card is not today */
	else {																 
		strcpy(card->last_transc_datetime,transc.datetime);
		card->used_daily_transc_amount=transc.transc_amount;
	}
	
	if((transc.transc_amount>(card->transc_lim))&&
		((card->used_daily_transc_amount)>(card->daily_lim))) {
		printf("OVER_BOTH_LIMITS");
	}
	else if(transc.transc_amount>(card->transc_lim)) {
		printf("OVER_TRANS_LIMIT");
	}
	else if((card->used_daily_transc_amount)>(card->daily_lim)) {
		printf("OVER_DAILY_LIMIT");
	}
	else{
		printf("IN_BOTH_LIMITS");
	}
	printf("\n");
}

/* checks if card ID is the one we are looking for.
if it is not, then it compare it to show if it is smaller or larger */
int compare_card_ids (const void* key, const void* elem) {
	return strcmp((char *)key,((cardrec_t *)elem)->card_id);
}

/*****************************************************************/
/* The time complexity of going through transaction list once is O(n) where
n = number of transactionsand O(log m) is the time complexity of binarysearch
where m = number of cards so in total the time complexity of stage 4
is O(nlongm) */

