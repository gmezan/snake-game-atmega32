#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

const int time1=100; //in ms
void turn_matrix_off(void)
{
	PORTA&=~(0xFF);
	PORTC&=~(0xFF);
}

void set0(int LEDS[8][8])
{
	int i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			LEDS[i][j]=0;
		}
	}
}
void led_array(int LED[8][8])
{	int temp=0;
	int i,j;
	turn_matrix_off();
	for(j=0;j<8;j++)
	{	//LED[j][i]
		for(i=0;i<8;i++)
		{
			temp|=((LED[j][i])<<i);
		}
		PORTC=temp;
		PORTA=(0x00|(1<<j));
		_delay_us(300);
		turn_matrix_off();
		//_delay_us(10);
		temp=0;
	}
}
void restart_snake(int POS[64][2],int LED[8][8], int* ri, int* rj, int* length, int* reset, int* i,int* j, int* fi,int* fj, int* bef)
{
	set0(LED);
	POS[0][0]=0;
	POS[0][1]=2;
	POS[1][0]=0;
	POS[1][1]=1;
	POS[2][0]=0;
	POS[2][1]=0;
	LED[0][0]=1;LED[0][1]=1;LED[0][2]=1;
	*ri=rand() %8;
	*rj=rand() %8;
	*length=3;
	*reset=1;
	*i=2;
	*j=0;
	*fi=1;
	*fj=0;
	*bef=0;
}

void set_up(void)
{
	MCUCSR = (1<<JTD); // enable portc
	MCUCSR = (1<<JTD);
	DDRA|=0xFF; // PORTA as output
	DDRC|=0xFF; // PORTC as output
	DDRB&=~(0b00011111); // first 5 bits as input
	// enabling pull up resistors for inputs:
	PORTB|= 0b00011111;
	//turning off the leds:
	turn_matrix_off();
}

void snake(void)
{
	uint32_t n;
	int length=3; //initial length
	int LED[8][8];
	int POS[64][2]={{0,2},{0,1},{0,0}}; // POS[0][n]=j y POS[1][n]=i
	set0(LED);
	LED[0][0]=1;LED[0][1]=1;LED[0][2]=1;
	int POS1[64][2];//arreglo auxiliar
	int m;
	int reset=1;
	while (1)
	{
		turn_matrix_off();
		int fi=1,fj=0,i=2,j=0;//fi y fj son las señales banderas para i y j
		int bef=0; // 1=j, 0=i
		//int cont=0;
		int ri,rj;
		n=0;
		int ri0=rand() %8;
		int rj0=rand() %8;
		ri=rand() %8;
		rj=rand() %8;
		int cond=0,cond2=1;//condicion
		while(1)
		{
			ri0=rand() %8;
			rj0=rand() %8;
			if((ri==i)&&(rj==j))
			{
				while(1)
				{
					cond=1;
					for(m=0;m<length;m++)
					{
						cond*=(!((POS[m][0]==rj0) && (POS[m][1]==ri0)));
					}
					if(cond==1)
					{
						ri=ri0;
						rj=rj0;
						length++;
						break;
					}
					else
					{
						ri0=rand() %8;
						rj0=rand() %8;
					}
				}
			}
			
			if(!(PINB & (1<<0))&&(bef!=1)) {fi=0;fj=1; bef=1;}
			if(!(PINB & (1<<1))&&(bef!=1)) {fi=0;fj=-1;bef=1;}
			if(!(PINB & (1<<2))&&(bef!=0)) {fi=1;fj=0; bef=0;}
			if(!(PINB & (1<<3))&&(bef!=0)) {fi=-1;fj=0;bef=0;}
			
			if(n>50)//ciclos de espera para ver un movimiento
			{
				//(fj==1 )?((j<7)?(j++):(j=0)):(j=j);
				//(fj==-1)?((j>0)?(j--):(j=7)):(j=j);
				//(fi==1 )?((i<7)?(i++):(i=0)):(j=j);
				//(fi==-1)?((i>0)?(i--):(i=7)):(j=j);
				cond2=1;
				(fj==1 )?((j<7)?(j++):(cond2=0)):(j=j);
				(fj==-1)?((j>0)?(j--):(cond2=0)):(j=j);
				(fi==1 )?((i<7)?(i++):(cond2=0)):(j=j);
				(fi==-1)?((i>0)?(i--):(cond2=0)):(j=j);
				n=0;
				// move:
				for(m=0;m<length;m++)
				{
					POS1[m][0]=POS[m][0];
					POS1[m][1]=POS[m][1];
				}
				for(m=0;m<(length-1);m++)
				{
					POS[m+1][0]=POS1[m][0];
					POS[m+1][1]=POS1[m][1];
				}
				POS[0][0]=j;
				POS[0][1]=i;
				set0(LED);
				for(m=0;m<(length);m++)
				{
					LED[POS[m][0]][POS[m][1]]=1;
				}
				LED[rj][ri]=1;	//food
			}
			n++;
			cond=1;
			for(m=0;m<(length-1);m++)
			{
				cond*=(!((POS[m+1][0]==j) && (POS[m+1][1]==i)));
			}
			((cond==0)||(cond2==0))?(restart_snake(POS,LED,&ri,&rj,&length,&reset,&i,&j,&fi,&fj,&bef)):(cond=cond);
			led_array(LED);
			PORTC=(1<<ri);
			PORTA=(1<<rj);
			_delay_us(300);
			turn_matrix_off();
			cond2=1;
			//_delay_us(7000);
		}
	}
}

void rotate_shape(uint8_t array[2])
{
	uint8_t var1,var2;
	var1=(((array[0])>>1)+(array[1]&(1<<1)))&0b11;
	var2=((array[0]&(1<<0))+((array[1])<<1))&0b11;
	array[0]=var1;
	array[1]=var2;
}

void set0x2( uint8_t array[8])
{
	int n;
	for(n=0;n<8;n++)
	{
		array[n]=0;
	}
}

void led_arrayx2(uint8_t led[8])
{
	int n;
	for(n=0;n<8;n++)
	{
		PORTC=led[n];
		PORTA=(1<<(7-n));
		_delay_ms(1);
		turn_matrix_off();
	}
}

void tetris()
{
	uint8_t t_square[2]={0b11,0b11};
	uint8_t t_line[2]={0b00,0b11};
	uint8_t t_2line[2]={0b10,0b11};
	uint8_t shapes[3][2]={{t_square[0],t_square[1]},{t_line[0],t_line[1]},{t_2line[0],t_2line[1]}};
	uint8_t element=rand()%2;
	uint8_t end=1,n=0,i=3,ciclos=0,fi=0,m=0;
	uint8_t cond=1, rotar=0;
	 
	uint8_t LED[8]={0,0,0,0,0,0,0,0};
	uint8_t LED1[8]={0,0,0,0,0,0,0,0};
	uint8_t POS[8]={0,0,0,0,0,0,0,0};	
		
	while(1)
	{
		//start:
		element=rand()%3;
		n=0;i=3;
		while((end)&&(n!=8))
		{
			if(n==0)
			{
				POS[n]=(shapes[element][1])<<i;
			}
			else
			{
				POS[n-1]=(shapes[element][0])<<i;
				POS[n]=(shapes[element][1])<<i;
			}
			if(!(PINB & (1<<2))) fi=1;
			if(!(PINB & (1<<3))) fi=2;
			if(!(PINB&(1<<4))) rotar=1;
			
			ciclos++;
			if(!(ciclos%30))
			{	
				if(ciclos>60)
				{ 
					set0x2(POS);
					n++; ciclos=0;
				}
				
				if(rotar&&(!(ciclos%30)))
				{
					rotate_shape(shapes[element]);
					rotar=0;
				}
				
				if(fi==1)
					(i<6)?(i++):(i=6);
				else if(fi==2)
					(i>0)?(i--):(i=0);
				
				fi=0;		
			}
			
			cond=1;
			for(m=0;m<8;m++)
			{
				LED1[m]=POS[m];
			}
			led_arrayx2(LED1);
			
		}
		
	}
	
}

int main(void)
{
	set_up();
	snake();
	//tetris(); not finsihed :(
}