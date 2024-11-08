#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_SLICE 2
#define Max_Memory 1024
#define Block_Time 8

// 进程、作业状态：就绪 等待 阻塞 完成
enum status{
    status_wait = 'W',
    status_run = 'R',
    status_block = 'B',
    status_finish = 'F'
};

// 进程控制块结构体
typedef struct PCB{
	// 进程名
    char process_name[5];
	// 到达时间
    int arrive_time;
    // 仍需时间
    int need_time;
    // 进程状态：运行、就绪、阻塞、完成
	// 但完成状态不显示在进程队列中，完成则调出内存
    char status;
    // 进程阻塞时长
	int block_time;
	// 占用内存起址
	int begin;
    // 占用内存大小
    int size;
 	// 占用作业
	char task_name[5];
	// 后向指针
	PCB *next;
}PCB;

// 内存块结构体
typedef struct Node{
	// 分区起址
	int begin;
	// 分区大小
	int size;
	// 分区状态：Busy(1)或Free(0)
	int status;
	// 占用进程
	char process_name[5];
	// 后向指针
	Node *next;
}Node;

// 作业结构体
typedef struct Task{
	// 作业名
	char task_name[5];
    // 到达时间
	int arrive_time;
	// 所需时间
    int need_time;
	// 所需内存
    int size;
	// 作业状态：等待、完成
	char status;
	// 后向指针
	Task *next;
}Task;

// 进程调度总时间
int Total_time = 0;
// 记录当前执行进程的位置
PCB *runIndex = NULL;
// 记录当前进程队列个数
int sum = 0;
// 记录已经生成的进程数
int number = 0;

// 函数声明
// 初始化内存分区
void initNode(Node *node);
// 首次适应算法分配内存
bool first_fit(Node *node_list,PCB *new_process);
// 随机产生一批作业(>=10个)
void create_task(Task *task_list);
// 按到达时间插入新作业
void insert_task(Task *task_list,Task *new_task);
// 将已完成作业调整到作业队列尾部
void finish_task(Task *task_list,PCB *finish_process);
// 先来先服务选择作业
void change_task(Node *node_list,Task *task_list,PCB *process_list);
// 创建新进程
bool create_process(PCB *process_list,Task *move_task,Node *node_list);
// 按起始地址插入新进程
void insert_process(PCB *process_list,PCB *new_process);
// 随机阻塞进程
void block_process(PCB *process_list);
// 唤醒进程
void notify_process(PCB *process_list);
// 时间片轮转调度算法
void round_robin(Node *node_list,Task *tasl_list,PCB *process_list);
// 展示：内存分区表、内存分配情况
void show_node(Node *node_list);
// 展示：后备作业队列、作业完成情况
void show_task(Task *task_list);
// 展示：进程运行情况、作业完成情况
void show_process(PCB *process_list);

// 主函数
int main(){
	srand(time(NULL));
	// 创建内存分区链表
	Node *node_list = (Node*)malloc(sizeof(Node));
	if(node_list == NULL){
		printf("动态内存分配失败！");
	}else{
		node_list->next = NULL;
	}
	initNode(node_list);

	// 创建后备作业队列
	Task *task_list = (Task*)malloc(sizeof(Task));
	if(task_list == NULL){
		printf("动态内存分配失败！");
	}else{
		task_list->next = NULL;
	}
	create_task(task_list);

	// 创建空进程队列
	PCB *process_list = (PCB*)malloc(sizeof(PCB));
	if(process_list == NULL){
		printf("动态内存分配失败！");
	}else{
		process_list->next = NULL;
	}

	printf("初始状态：\n");
	show_node(node_list);
	show_task(task_list);
	show_process(process_list);

	// 调用时间片轮转调度算法
	round_robin(node_list,task_list,process_list);
}

// 初始化内存分区
void initNode(Node *node){
	Node *new_node = (Node*)malloc(sizeof(Node));
	if(new_node == NULL){
		printf("动态内存分配失败！");
	}
	// 分区起址
	new_node->begin = 0;
	// 分区大小
	new_node->size = Max_Memory;
	// 分区状态：0代表空闲，1代表占用
	new_node->status = 0;
	// 进程名
	sprintf(new_node->process_name,"%s","无");
	// 后向指针
	new_node->next = NULL;
	// 将该初始化分区放入分区链表
	node->next = new_node;
}

// 首次适应算法分配内存
bool first_fit(Node *node_list,PCB *new_process){
	Node *move = node_list->next;
	// 空闲内存分区链不为空
	while(move != NULL){
		// 空闲的空间
		if(move->status == 0){
			// 剩余空间大于作业需要的内存空间，可分配
			if(move->size > new_process->size){
				// 分配后剩余的空间
				Node *p = (Node*)malloc(sizeof(Node));
				p->begin = move->begin + new_process->size;
				p->size = move->size - new_process->size;
				p->status = 0;
				sprintf(p->process_name,"%s","无");
				// 分配给进程的空间
				move->size = new_process->size;
				move->status = 1;
				sprintf(move->process_name,"%s",new_process->process_name);
				// 改变节点的连接
				p->next = move->next;
				move->next = p;
				break;
			}else if(move->size == new_process->size){
				// 空闲空间和作业需要的内存空间大小相等时，可分配
				move->status = 1;
				sprintf(move->process_name,"%s",new_process->process_name);
				break;
			}
		}
		// 已到空闲内存分区链末尾
		if(move->next == NULL){
			printf("内存分配失败，没有足够大的内存分配给该进程！\n");
			return false;
		}
		move = move->next;
	}
	new_process->begin = move->begin;
	show_node(node_list);
	return true;
}

// 回收内存并将其中作业调整到后备作业队列
void recycle(Node *node_list,PCB *finish_process){
	Node *move = node_list->next;
	while(true){
		// 当进程队列的第一个进程占用内存需释放时
		if(strcmp(move->process_name,finish_process->process_name) == 0){
			move->status = 0;
			sprintf(move->process_name,"%s","无");
			break;
		}else if(move->status == 0 && strcmp(move->next->process_name,finish_process->process_name) == 0){
			// 当move指向需释放空间的前驱结点，需释放空间的上一块空间空闲时
			// 合并需释放空间上一块空间和需释放空间
			move->size = move->size + move->next->size;
			Node *q = move->next;
			move->next = move->next->next;
			// 释放内存
			free(q);
			break;
		}else if(strcmp(move->next->process_name,finish_process->process_name) == 0){
			// 需释放空间的上一块空间忙碌时
			// move指向当前释放的内存空间
			move = move->next;
			move->status = 0;
			sprintf(move->process_name,"%s","无");
			break;
		}else if(move->next == NULL){
			// 已走到链表末尾，此时表明进程名都不匹配
			printf("此进程不存在！\n");
			break;
		}
		move = move->next;
	}
	// 需释放空间的下一个空间空闲时
	if(move->next != NULL && move->next->status == 0){
		move->size = move->size + move->next->size;
		Node *q = move->next;
		move->next = move->next->next;
		free(q);
	}
}

// 随机产生一批作业(>=10)
void create_task(Task *task_list){
	printf("\n请输入产生的作业数(>=10)：");
	int number;
	scanf("%d",&number);
	printf("\n");
	int i = 1;
	// 循环创建后备作业队列
	while(i <= number){
		// 创建新作业
		Task *new_task = (Task*)malloc(sizeof(Task));
		if(new_task == NULL){
			printf("动态内存分配失败！");
		}
		// 作业名
		sprintf(new_task->task_name,"%s%d","T",i);
	    // 到达时间(0~8)
	    if(task_list->next == NULL){
	    	new_task->arrive_time = 0;
		}else{
			new_task->arrive_time = rand()%19;
		}
		// 服务时间(1~10)
		new_task->need_time = rand()%12+1;
		// 所需内存(31~200)
	    new_task->size = rand()%(Max_Memory/5)+31;
		// 作业状态
		new_task->status = status_wait;
		// 后向指针
		new_task->next = NULL;
		// 创建下一作业
		++i;
		// 将新作业插入后备作业队列
		insert_task(task_list,new_task);
	}
}

// 按到达时间插入新作业
void insert_task(Task *task_list,Task *new_task){
	int arrive_time = new_task->arrive_time;
	Task *move = task_list;
	while(move->next != NULL){
		if(move->next->arrive_time > arrive_time){
			new_task->next = move->next;
			move->next = new_task;
			return;
		}
		move = move->next;
	}
	move->next = new_task;
}

// 将已完成作业调整到作业队列尾部
void finish_task(Task *task_list,PCB *finish_process){
	Task *finish_task = (Task*)malloc(sizeof(Task));
	if(finish_task == NULL){
		printf("动态内存分配失败！");
	}
	sprintf(finish_task->task_name,"%s",finish_process->task_name);
	finish_task->arrive_time = finish_process->arrive_time;
	finish_task->need_time = finish_process->need_time;
	finish_task->size = finish_process->size;
	finish_task->status = finish_process->status;
	finish_task->next = NULL;
	Task *move = task_list;
	while(move->next != NULL){
		move = move->next;
	}
	move->next = finish_task;
	show_task(task_list);
}

// 先来先服务选择作业
void change_task(Node *node_list,Task *task_list,PCB *process_list){
	/*
	由于进程最多5个并发，即等待、阻塞、运行的进程数最多为5个
	所以从作业队列中选择作业不能使进程超过5个，而且要考虑作业是否到达
	*/
	// 选择作业
	Task *move = task_list->next;
	while(sum < 5 && move != NULL && move->status != status_finish && move->arrive_time <= Total_time){
		// 先保存要调入内存的作业
		Task *move_task = move;
		// 为该作业创建进程
		if(create_process(process_list,move_task,node_list)){
			// 分配内存成功
			sum++;
			// 剔除作业
			Task *p = task_list;
			while(p->next != move){
				p = p->next;
			}
			p->next = move->next;
			move = p->next;
		}else{
			move = move->next;
		}
	}
}

// 创建新进程
bool create_process(PCB *process_list,Task *move_task,Node *node_list){
	PCB *new_process = (PCB*)malloc(sizeof(PCB));
	if(new_process == NULL){
		printf("动态内存分配失败！");
	}
	// 进程名
	sprintf(new_process->process_name,"%s%d","P",++number);
	// 到达时间
	new_process->arrive_time = move_task->arrive_time;
    // 仍需时间
    new_process->need_time = move_task->need_time;
    // 进程状态
    new_process->status = move_task->status;
    // 阻塞时长
	new_process->block_time = 0;
	// 占用内存起址
	new_process->begin = 0;
    // 占用内存大小
    new_process->size = move_task->size;
 	// 占用作业
	sprintf(new_process->task_name,"%s",move_task->task_name);
	// 后向指针
	new_process->next = NULL;
	// 为新进程分配内存
	if(!first_fit(node_list,new_process)){
		free(new_process);
		return false;
	}
	// 将新进程放入进程队列
	insert_process(process_list,new_process);
	return true;
}

// 按起始地址插入新进程
void insert_process(PCB *process_list,PCB *new_process){
	int begin = new_process->begin;
	PCB *move = process_list;
	while(move->next != NULL){
		if(move->next->begin > begin){
			new_process->next = move->next;
			move->next = new_process;
			return;
		}
		move = move->next;
	}
	move->next = new_process;
	show_process(process_list);
}

// 随机阻塞进程
void block_process(PCB *process_list){
	// 随机生成阻塞进程名
	int block_id = rand()%number+1;
	char block_pro[5];
	sprintf(block_pro,"%s%d","P",block_id);
	PCB *move = process_list->next;
	while(move != NULL){
		if(strcmp(move->process_name,block_pro) == 0){
			// 更新进程状态
			move->status = status_block;
			// 将阻塞时长置0
			move->block_time = 0;
			printf("阻塞进程：%s\n",move->process_name);
			return;
		}
		move = move->next;
	}
}

// 唤醒进程
void notify_process(PCB *process_list){
	PCB *move = process_list->next;
	while(move != NULL){
		if(move->block_time >= Block_Time){
			// 更新进程状态
			move->status = status_wait;
			// 将阻塞时长置0
			move->block_time = 0;
			printf("唤醒进程：%s\n",move->process_name);
		}
		move = move->next;
	}
}

// 时间片轮转调度算法
void round_robin(Node *node_list,Task *task_list,PCB *process_list){
    /*
    用一个全局变量记录当前进程数，当前进程数小于5时，从作业队列中调入已到达作业并为作业创建进程分配内存
	用一个指针记录当前执行的进程，每一次从该进程开始找进程为wait并执行一个时间片
	当一个进程完成时，释放该进程占用的内存并将该已完成的作业调到后备作业队列的末尾
	整个调度过程直到后备作业队列的第一个作业为完成状态以及进程队列为空则结束
	在进程调度的过程中，随机阻塞进程：一定的时间后唤醒进程
     */
    // 记录当前执行进程的位置，初始指向进程队列的第一个进程
	runIndex = process_list->next;
    while (sum != 0 || task_list->next->status != status_finish) {
    	// 保存旧进程数
		int old_sum = sum;
    	// 当前进程数小于5并且仍然有作业未完成，从后备队列选择作业
		if(sum < 5 && task_list->next != NULL && task_list->next->status != status_finish){
			change_task(node_list,task_list,process_list);
		}
		// 随机阻塞进程
		block_process(process_list);
		// 唤醒进程
		notify_process(process_list);
		// 刚好runIndex指向NULL且当前进程数增加
		if(runIndex == NULL && old_sum < sum){
			// 找出之前进程的最后一个
			int move_sum = 0;
			PCB *move = process_list;
			while(move_sum < old_sum){
				move_sum++;
				move = move->next;
			}
			// runIndex指向新加入的进程
			runIndex = move->next;
		}else if(runIndex == NULL){
			// 当runIndex指向NULL且当前进程数未增加
			// runIndex重新指向进程队列的第一个进程
			runIndex = process_list->next;
		}
		// 经过以上处理runIndex仍然为NULL，则当前无进程无作业到达
		if(runIndex == NULL){
			printf("当前无进程且无作业到达！\n");
			printf("等待作业到达。。。。。。\n");
			printf("等待时长：%d\n",task_list->next->arrive_time - Total_time);
			Total_time += task_list->next->arrive_time - Total_time;
			printf("进程调度总时间：%d\n",Total_time);
			continue;
		}
		// 循环遍历进程队列，找到进程状态为wait的进程并执行
		PCB *memoryIndex = runIndex;
		while(runIndex->status != status_wait){
			runIndex = runIndex->next;
			// runIndex走到进程队列末尾
			if(runIndex == NULL){
				runIndex = process_list->next;
			}
			// runIndex循环一遍后仍找不到状态为wait的进程
			if(runIndex == memoryIndex){
				show_process(process_list);
				// 表示所有进程都阻塞
				// 找出距离阻塞进程被唤醒最近的进程，等待该进程被唤醒
				int notify_time = Block_Time - process_list->next->block_time;
				PCB *move = process_list->next;
				while(move != NULL){
					// 如果有距离阻塞进程被唤醒更近的进程
					if(Block_Time - move->block_time < notify_time){
						// 更新唤醒时长
						notify_time = Block_Time - move->block_time;
					}
					move = move->next;
				}
				printf("当前进程全部阻塞，等待进程被唤醒！\n");
				printf("等待时长：%d\n",notify_time);
				// 此时已找出距离阻塞进程被唤醒最近时长
				move = process_list->next;
				while(move != NULL){
					// 等待进程被唤醒
					// 更新所有进程的阻塞时长
					move->block_time += notify_time;
					move = move->next;
				}
				// 更新进程调度总时间
				Total_time += notify_time;
				show_process(process_list);
				// 唤醒进程
				notify_process(process_list);
				show_process(process_list);
				// 重新进行进程调度
				printf("进程调度总时间：%d\n",Total_time);
				continue;
			}
		}
		// 执行当前进程
		runIndex->status = status_run;
		show_process(process_list);
		// 判断该进程是否能完成
        if (runIndex->need_time-TIME_SLICE > 0) {
        	// 进程执行一个时间片后无法完成
            // 更新进程调度总时间
        	Total_time += TIME_SLICE;
			// 更新进程仍需时间
        	runIndex->need_time -= TIME_SLICE;
        	// 更改进程状态
			runIndex->status = status_wait;
        	// 指向下一进程
			runIndex = runIndex->next;
			// 更新阻塞时长
			PCB *p = process_list->next;
			while(p != NULL){
				if(p->status == status_block){
					p->block_time += TIME_SLICE;
				}
				p = p->next;
			}
        }else{
        	// 进程可以完成
        	// 更新阻塞时长
			PCB *p = process_list->next;
			while(p != NULL){
				if(p->status == status_block){
					p->block_time += runIndex->need_time;
				}
				p = p->next;
			}
			// 更新进程调度总时间
        	Total_time += runIndex->need_time;
			// 更新进程仍需时间
			runIndex->need_time = 0;
			runIndex->status = status_finish;
			show_process(process_list);
        	// 移除该进程同时释放内存并将其中作业调整到后备作业队列
        	PCB *finish_process = runIndex;
        	PCB *move = process_list;
        	while(move->next != runIndex){
        		move = move->next;
			}
			// 移除进程
			move->next = runIndex->next;
			// 指向下一进程
			runIndex = move->next;
			// 将其中作业调整到后备作业队列并释放内存
			finish_task(task_list,finish_process);
			recycle(node_list,finish_process);
			free(finish_process);
			sum--;
		}
		show_node(node_list);
		show_task(task_list);
		show_process(process_list);
		printf("进程调度总时间：%d\n",Total_time);
    }
}

// 展示：内存分区表、内存分配情况
void show_node(Node *node_list){
	Node *move = node_list->next;
	printf("************************************************************************************\n");
	printf("                 内存分区表                 \n");
	printf("－－－－－－－－－－－－－－－－－－－－－\n");
	printf("|起始地址   分区大小   占用进程   分区状态|\n");
	while(move != NULL){
		if(move->status == 0){
			printf("|   %d\t      %d\t   %s\t    Free  |\n",move->begin,move->size,move->process_name,move->status);
		}else{
			printf("|   %d\t      %d\t   %s\t    Busy  |\n",move->begin,move->size,move->process_name,move->status);
		}
		move = move->next;
	}
	printf("－－－－－－－－－－－－－－－－－－－－－\n");
}

// 展示：后备作业队列、作业完成情况
void show_task(Task *task_list){
	Task *move = task_list->next;
	printf("                    后备作业队列                    \n");
	printf("～～～～～～～～～～～～～～～～～～～～～～～～～～\n");
	printf("|作业名   到达时间   所需时间   所需内存   作业状态|\n");
	while(move != NULL){
		printf("|  %s\t     %d\t        %d\t  %d\t      %c    |\n",move->task_name,move->arrive_time,move->need_time,move->size,move->status);
		move = move->next;
	}
	printf("～～～～～～～～～～～～～～～～～～～～～～～～～～\n");
}

// 展示：进程运行情况、作业完成情况
void show_process(PCB *process_list){
	PCB *move = process_list->next;
	printf("                                  当前各进程PCB信息                                  \n");
	printf("﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀\n");
	printf("|进程名   到达时间   仍需时间   进程状态   阻塞时长   内存起址   内存大小   占用作业|\n");
	while(move != NULL){
		printf("|  %s\t     %d\t        %d\t   %c\t      %d\t         %d\t    %d\t       %s   |\n",
		move->process_name,move->arrive_time,move->need_time,move->status,move->block_time,move->begin,move->size,move->task_name);
		move = move->next;
	}
	printf("﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀﹀\n");
}
