/*This is the final one*/
#include<stdio.h>
#include<stdlib.h>
#define N 3
#define M 100000
#define Q 40
typedef struct num
{
    int a[N][N];
    int h;
    int wrong;
    int f;
    struct num * down;
    struct num * up;
}node;

static node* open_list[M];
static node* close_list[M];
static node* stack[M];
static int pointer_open=0;
static int pointer_close=0;
static int pointer_stack=0;
static int time=0;
static FILE * fout=NULL;
static int num_node=0;
static int node_depth_flag=0;			//收集层数，更换排序方式
static int note_flag=0;
static int dst[N][N];
static int node_depth_num[Q];

int correct(node *);				//正确 确定数字错位数和估值函数
int compare(node *,node *);			//比较chess
void create_list(node **);			//建立表头
void print(node *,FILE *);				    //打印 棋盘数字和深度
node * move(node *);
void chess_copy(node *,node*);		//正确 复制数字位置 节点深度计算
void add_to_openlist(node *);
void del_last_openlist(void);
void add_to_closelist(node *);
void del_from_closelist(node*);
int judge_existence(node *);		//判断存在性 若存在返回1 若不存在返回0 若存在且比之前值小取代旧值并加入open表（移出close表） 若不存在则加入open表
void openlist_sort(void);			//降序排列数组元素
void output_openlist(void);
void output_closelist(void);
void solve_down_pointer(node *);
void open_list_resort(void);
void create_dst(void);

void create_dst(void)
{
	printf("Input your destiny please:\n");
	int i;
	int j;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			scanf("%d",&dst[i][j]);
}



int main()
{
    int i,j;
    fout=fopen("./log.txt","w+");
    FILE *fout_ans;
    fout_ans=fopen("./ans.txt","w+");
    create_dst();
	node *head=NULL;
	node *win_flag=NULL;
	create_list(&head);
	printf("Input your original 8 num:\n");
    for(i=0;i<N;i++)
		{
			for(j=0;j<N;j++)
        {
            scanf("%d",&(head->a[i][j]));
        }
		}
	head->h=0;
	correct(head);
	add_to_openlist(head);
	print(head,fout_ans);
	while(1)
	{
		if(pointer_open==0)
		{
			printf("false\n");
			break;
		}
		//计数
		//puts("check in main");
		//output_openlist();
		//fprintf(fout,"No %d\n\n",time);
		openlist_sort();
		//output_openlist();
		//print(open_list[pointer_open-1]);
		add_to_closelist(open_list[pointer_open-1]);
		//output_closelist();
		del_last_openlist();
		//output_openlist();
		win_flag=move(close_list[pointer_close-1]);
		if(win_flag!=NULL)
		{
			fprintf(fout,"This is the log\n");
			fprintf(fout, "*************************************************************************\n" );
			int depth;
			for(depth=0;depth<node_depth_flag;depth++)
			{
				fprintf(fout,"Depth is %d: num is %d\n",depth,node_depth_num[depth]);
			}
			fprintf(fout,"The node num is %d\n\n",num_node);
			fprintf(fout, "The node_depth_flag=%d\n\n",node_depth_flag );
			fprintf(fout, "The note_flag=%d\n\n",note_flag );
			fprintf(fout, "*************************************************************************\n" );
			fprintf(fout, "The open_list is as follow\n\n" );
			output_openlist();
			fprintf(fout, "*************************************************************************\n" );
			fprintf(fout, "The close_list is as follow\n\n");
			output_closelist();
			win_flag->down=NULL;
			node *p_win=head;
			solve_down_pointer(win_flag);
			for(i=1;p_win->down!=NULL;i++)
			{
				p_win=p_win->down;
				fprintf(fout_ans,"Step %d\n",i);
				print(p_win,fout_ans);
			}
			break;
		}
		time++;
	}
    return 0;
}

void solve_down_pointer(node *x)
{
	while(x->up!=NULL)
	{
		x->up->down=x;
		x=x->up;
	}
}

void output_openlist(void)
{
	int i;
	fputs("Contents of the open_list\n",fout);
	for(i=0;i<pointer_open;i++)
	{
		print(open_list[i],fout);
	}
	fputs("end\n\n",fout);
}

void output_closelist(void)
{
	int i;
	fputs("Contents of the close_list\n",fout);
	for(i=0;i<pointer_close;i++)
	{
		print(close_list[i],fout);
	}
	fputs("end\n\n",fout);
}

void add_to_openlist(node *x)
{
	open_list[pointer_open]=x;
	pointer_open++;
	num_node++;
	node_depth_num[x->h]++;
}

void del_last_openlist(void)
{
	pointer_open--;
}

void add_to_closelist(node *x)
{
	close_list[pointer_close]=x;
	pointer_close++;
}

void del_from_closelist(node *x)		//正确
{
	int i;
	for(i=pointer_close-1;i>=0;i--)
	{
		if(close_list[i]!=x)
		{
			stack[pointer_stack]=close_list[i];
			pointer_stack++;
		}
		else
			pointer_close--;
	}
	for(i=0;i<pointer_close;i++)
	{
		close_list[i]=stack[--pointer_stack];
	}
}


void print(node *m,FILE * n)					//打印 棋盘数字和深度
{
	fprintf(n,"f is %d Depth is %d wrong is %d op is %d cp is %d\n",m->f,m->h,m->wrong,pointer_open,pointer_close);
    int i,j;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            fprintf(n,"%d",m->a[i][j]);
            if(j==N-1)
            	fputc('\n',n);
            else
				fputc(' ',n);
        }
    }
}

void create_list(node **headp)		//建立表头
{
		*headp=(node *)malloc(sizeof(node));
		(*headp)->up=NULL;
		//(*headp)->down=NULL;
}

node * move(node *x)				// 移动数字
{
		//print(x);
		int o_i=1000,o_j=1000;
		int i=0,j=0,flag=100;
		for(i=0;i<N;i++)
		{
			for(j=0;j<N;j++)
			{
				if(x->a[i][j]==0)
				{
					o_i=i;
					o_j=j;
					goto OK;
				}
			}
		}
		OK:
		i=o_i+1,j=o_j+1;
		switch(i)
		{
			case 1:
				{
					if(j==1)
						flag=1;
					else if(j==2)
						flag=2;
					else if(j==3)
						flag=3;
				}
				break;
			case 2:
				{
					if(j==1)
						flag=4;
					else if(j==2)
						flag=5;
					else if(j==3)
						flag=6;
				}
				break;
			case 3:
				{
					if(j==1)
						flag=7;
					else if(j==2)
						flag=8;
					else if(j==3)
						flag=9;
				}
				break;
		}
		if(flag==100)
			puts("false");
		//printf("flag=%d oi=%d oj=%d\n",flag,o_i,o_j);
		node *up_move,*down_move,*left_move,*right_move;
		up_move=down_move=left_move=right_move=NULL;
		switch(flag)
		{
			case 1:																		//依照上下左右
				{
					down_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,down_move);
					chess_copy(x,right_move);
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(right_move);
				}
				break;
			case 2:
				{
					down_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,down_move);
					chess_copy(x,left_move);
					chess_copy(x,right_move);
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
					//correct(left_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(right_move);
				}
				break;
			case 3:
				{
					down_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					chess_copy(x,down_move);
					chess_copy(x,left_move);
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
					//correct(left_move);
				}
				break;
			case 4:
				{
					up_move=(node *)malloc(sizeof(node));
					down_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,down_move);
					chess_copy(x,right_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(left_move);
				}
				break;
			case 5:
				{
					up_move=(node *)malloc(sizeof(node));
					down_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,down_move);
					chess_copy(x,left_move);
					chess_copy(x,right_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					//correct(up_move);
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
					//correct(left_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(right_move);
				}
				break;
			case 6:
				{
					up_move=(node *)malloc(sizeof(node));
					down_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,down_move);
					chess_copy(x,left_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					//correct(up_move);
					down_move->a[o_i][o_j]=down_move->a[o_i+1][o_j];
					down_move->a[o_i+1][o_j]=0;
					//correct(down_move);
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
					//correct(left_move);
				}
				break;
			case 7:
				{
					up_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,right_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					//correct(up_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(right_move);
				}
				break;
			case 8:
				{
					up_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					right_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,left_move);
					chess_copy(x,right_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					//correct(up_move);
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
					//correct(left_move);
					right_move->a[o_i][o_j]=right_move->a[o_i][o_j+1];
					right_move->a[o_i][o_j+1]=0;
					//correct(right_move);
				}
				break;
			case 9:
				{
					up_move=(node *)malloc(sizeof(node));
					left_move=(node *)malloc(sizeof(node));
					chess_copy(x,up_move);
					chess_copy(x,left_move);
					up_move->a[o_i][o_j]=up_move->a[o_i-1][o_j];
					up_move->a[o_i-1][o_j]=0;
					left_move->a[o_i][o_j]=left_move->a[o_i][o_j-1];
					left_move->a[o_i][o_j-1]=0;
				}
				break;
		}
		//移动无问题
		node * c_flag=NULL;
		if(up_move!=NULL)
		{
			if(correct(up_move))
				c_flag=up_move;
			up_move->up=x;
			judge_existence(up_move);

		}
		if(left_move!=NULL)
		{
			if(correct(left_move))
				c_flag=left_move;
			left_move->up=x;
			judge_existence(left_move);
			//print(left_move);
		}
		if(down_move!=NULL)
		{
			if(correct(down_move))
				c_flag=down_move;
			down_move->up=x;
			judge_existence(down_move);
			//print(down_move);
		}
		if(right_move!=NULL)
		{
			if(correct(right_move))
				c_flag=right_move;
			right_move->up=x;
			judge_existence(right_move);
			//print(right_move);
		}
		if(c_flag==NULL)
			return NULL;
		else
			return c_flag;
}

void chess_copy(node *x,node *y)//正确 复制数字位置 节点深度计算
{
	int i,j;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		y->a[i][j]=x->a[i][j];
	y->h=x->h+1;
	if(y->h>node_depth_flag)
	{
		node_depth_flag=y->h;
		//if(node_depth_flag==13||node_depth_flag==21||node_depth_flag==24)
		//	open_list_resort();
		//note_flag=1;
	}

}

void open_list_resort(void)
{
	int i;
	for(i=0;i<pointer_open;i++)
		correct(open_list[i]);
	for(i=0;i<pointer_close;i++)
		correct(close_list[i]);
	openlist_sort();
}

int correct(node *x)		//正确 确定数字错位数和估值函数
{
	//int weigh;
	//if(node_depth_flag<=12)
		//weigh=1;
	//else if(node_depth_flag<=20)
		//weigh=2;
	//else if(node_depth_flag<=23)
		//weigh=3;
	//else
		//weigh=4;
	//int dst[N][N]={{4,5,7},{6,0,1},{3,2,8}};
	int i,j,m=0;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		{
			if(dst[i][j]==x->a[i][j])		//摆对的数目
				m++;
		}
	x->wrong=9-m;							//摆错的数目
	x->f=x->h+x->wrong;						//估值函数
	if(m==9)
		return 1;
	else
		return 0;
}

int compare(node *x,node *y)//比较chess
{
	int i,j;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			if(x->a[i][j]!=y->a[i][j])
				goto no;
	return 1;
	no:
	return 0;
}

int judge_existence(node *x)//判断存在性 若存在返回1 若不存在返回0 若存在且比之前值小取代旧值并加入open表（移出close表） 若不存在则加入open表
{
	int i,j;
	for(i=0;i<pointer_open;i++)									//在openlist
		if(compare(open_list[i],x))
		{
			if(open_list[i]->f>x->f)
				open_list[i]=x;
			//print(open_list[pointer_open-1]);
			goto END_FUNC;
		}
	for(j=0;j<pointer_close;j++)								//在closelist
		if(compare(close_list[j],x))
		{
			if(close_list[j]->f>x->f)
			{
				add_to_openlist(x);						//需要排序
				del_from_closelist(close_list[j]);
				//print(open_list[pointer_open-1]);
			}
			goto END_FUNC;
		}
	add_to_openlist(x);
	//print(open_list[pointer_open-1]);
	return 0;
	END_FUNC:
	return 1;
}

void openlist_sort(void)	//对openlist排序
{
	node * swap;
	int i,j;
	int num=pointer_open;
	for(i=0;i<num;i++)
	{
		if(i==1)
			break;
		for(j=0;j<num-1-i;j++)
		{
			if(open_list[j]->f<open_list[j+1]->f)
			{
				swap=open_list[j];
				open_list[j]=open_list[j+1];
				open_list[j+1]=swap;
			}
		}
	}
}
