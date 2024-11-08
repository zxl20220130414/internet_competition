#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_SLICE 2
#define Max_Memory 1024
#define Block_Time 8

// ���̡���ҵ״̬������ �ȴ� ���� ���
enum status{
    status_wait = 'W',
    status_run = 'R',
    status_block = 'B',
    status_finish = 'F'
};

// ���̿��ƿ�ṹ��
typedef struct PCB{
	// ������
    char process_name[5];
	// ����ʱ��
    int arrive_time;
    // ����ʱ��
    int need_time;
    // ����״̬�����С����������������
	// �����״̬����ʾ�ڽ��̶����У����������ڴ�
    char status;
    // ��������ʱ��
	int block_time;
	// ռ���ڴ���ַ
	int begin;
    // ռ���ڴ��С
    int size;
 	// ռ����ҵ
	char task_name[5];
	// ����ָ��
	PCB *next;
}PCB;

// �ڴ��ṹ��
typedef struct Node{
	// ������ַ
	int begin;
	// ������С
	int size;
	// ����״̬��Busy(1)��Free(0)
	int status;
	// ռ�ý���
	char process_name[5];
	// ����ָ��
	Node *next;
}Node;

// ��ҵ�ṹ��
typedef struct Task{
	// ��ҵ��
	char task_name[5];
    // ����ʱ��
	int arrive_time;
	// ����ʱ��
    int need_time;
	// �����ڴ�
    int size;
	// ��ҵ״̬���ȴ������
	char status;
	// ����ָ��
	Task *next;
}Task;

// ���̵�����ʱ��
int Total_time = 0;
// ��¼��ǰִ�н��̵�λ��
PCB *runIndex = NULL;
// ��¼��ǰ���̶��и���
int sum = 0;
// ��¼�Ѿ����ɵĽ�����
int number = 0;

// ��������
// ��ʼ���ڴ����
void initNode(Node *node);
// �״���Ӧ�㷨�����ڴ�
bool first_fit(Node *node_list,PCB *new_process);
// �������һ����ҵ(>=10��)
void create_task(Task *task_list);
// ������ʱ���������ҵ
void insert_task(Task *task_list,Task *new_task);
// ���������ҵ��������ҵ����β��
void finish_task(Task *task_list,PCB *finish_process);
// �����ȷ���ѡ����ҵ
void change_task(Node *node_list,Task *task_list,PCB *process_list);
// �����½���
bool create_process(PCB *process_list,Task *move_task,Node *node_list);
// ����ʼ��ַ�����½���
void insert_process(PCB *process_list,PCB *new_process);
// �����������
void block_process(PCB *process_list);
// ���ѽ���
void notify_process(PCB *process_list);
// ʱ��Ƭ��ת�����㷨
void round_robin(Node *node_list,Task *tasl_list,PCB *process_list);
// չʾ���ڴ�������ڴ�������
void show_node(Node *node_list);
// չʾ������ҵ���С���ҵ������
void show_task(Task *task_list);
// չʾ�����������������ҵ������
void show_process(PCB *process_list);

// ������
int main(){
	srand(time(NULL));
	// �����ڴ��������
	Node *node_list = (Node*)malloc(sizeof(Node));
	if(node_list == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
	}else{
		node_list->next = NULL;
	}
	initNode(node_list);

	// ��������ҵ����
	Task *task_list = (Task*)malloc(sizeof(Task));
	if(task_list == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
	}else{
		task_list->next = NULL;
	}
	create_task(task_list);

	// �����ս��̶���
	PCB *process_list = (PCB*)malloc(sizeof(PCB));
	if(process_list == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
	}else{
		process_list->next = NULL;
	}

	printf("��ʼ״̬��\n");
	show_node(node_list);
	show_task(task_list);
	show_process(process_list);

	// ����ʱ��Ƭ��ת�����㷨
	round_robin(node_list,task_list,process_list);
}

// ��ʼ���ڴ����
void initNode(Node *node){
	Node *new_node = (Node*)malloc(sizeof(Node));
	if(new_node == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
	}
	// ������ַ
	new_node->begin = 0;
	// ������С
	new_node->size = Max_Memory;
	// ����״̬��0������У�1����ռ��
	new_node->status = 0;
	// ������
	sprintf(new_node->process_name,"%s","��");
	// ����ָ��
	new_node->next = NULL;
	// ���ó�ʼ�����������������
	node->next = new_node;
}

// �״���Ӧ�㷨�����ڴ�
bool first_fit(Node *node_list,PCB *new_process){
	Node *move = node_list->next;
	// �����ڴ��������Ϊ��
	while(move != NULL){
		// ���еĿռ�
		if(move->status == 0){
			// ʣ��ռ������ҵ��Ҫ���ڴ�ռ䣬�ɷ���
			if(move->size > new_process->size){
				// �����ʣ��Ŀռ�
				Node *p = (Node*)malloc(sizeof(Node));
				p->begin = move->begin + new_process->size;
				p->size = move->size - new_process->size;
				p->status = 0;
				sprintf(p->process_name,"%s","��");
				// ��������̵Ŀռ�
				move->size = new_process->size;
				move->status = 1;
				sprintf(move->process_name,"%s",new_process->process_name);
				// �ı�ڵ������
				p->next = move->next;
				move->next = p;
				break;
			}else if(move->size == new_process->size){
				// ���пռ����ҵ��Ҫ���ڴ�ռ��С���ʱ���ɷ���
				move->status = 1;
				sprintf(move->process_name,"%s",new_process->process_name);
				break;
			}
		}
		// �ѵ������ڴ������ĩβ
		if(move->next == NULL){
			printf("�ڴ����ʧ�ܣ�û���㹻����ڴ������ý��̣�\n");
			return false;
		}
		move = move->next;
	}
	new_process->begin = move->begin;
	show_node(node_list);
	return true;
}

// �����ڴ沢��������ҵ����������ҵ����
void recycle(Node *node_list,PCB *finish_process){
	Node *move = node_list->next;
	while(true){
		// �����̶��еĵ�һ������ռ���ڴ����ͷ�ʱ
		if(strcmp(move->process_name,finish_process->process_name) == 0){
			move->status = 0;
			sprintf(move->process_name,"%s","��");
			break;
		}else if(move->status == 0 && strcmp(move->next->process_name,finish_process->process_name) == 0){
			// ��moveָ�����ͷſռ��ǰ����㣬���ͷſռ����һ��ռ����ʱ
			// �ϲ����ͷſռ���һ��ռ�����ͷſռ�
			move->size = move->size + move->next->size;
			Node *q = move->next;
			move->next = move->next->next;
			// �ͷ��ڴ�
			free(q);
			break;
		}else if(strcmp(move->next->process_name,finish_process->process_name) == 0){
			// ���ͷſռ����һ��ռ�æµʱ
			// moveָ��ǰ�ͷŵ��ڴ�ռ�
			move = move->next;
			move->status = 0;
			sprintf(move->process_name,"%s","��");
			break;
		}else if(move->next == NULL){
			// ���ߵ�����ĩβ����ʱ��������������ƥ��
			printf("�˽��̲����ڣ�\n");
			break;
		}
		move = move->next;
	}
	// ���ͷſռ����һ���ռ����ʱ
	if(move->next != NULL && move->next->status == 0){
		move->size = move->size + move->next->size;
		Node *q = move->next;
		move->next = move->next->next;
		free(q);
	}
}

// �������һ����ҵ(>=10)
void create_task(Task *task_list){
	printf("\n�������������ҵ��(>=10)��");
	int number;
	scanf("%d",&number);
	printf("\n");
	int i = 1;
	// ѭ����������ҵ����
	while(i <= number){
		// ��������ҵ
		Task *new_task = (Task*)malloc(sizeof(Task));
		if(new_task == NULL){
			printf("��̬�ڴ����ʧ�ܣ�");
		}
		// ��ҵ��
		sprintf(new_task->task_name,"%s%d","T",i);
	    // ����ʱ��(0~8)
	    if(task_list->next == NULL){
	    	new_task->arrive_time = 0;
		}else{
			new_task->arrive_time = rand()%19;
		}
		// ����ʱ��(1~10)
		new_task->need_time = rand()%12+1;
		// �����ڴ�(31~200)
	    new_task->size = rand()%(Max_Memory/5)+31;
		// ��ҵ״̬
		new_task->status = status_wait;
		// ����ָ��
		new_task->next = NULL;
		// ������һ��ҵ
		++i;
		// ������ҵ�������ҵ����
		insert_task(task_list,new_task);
	}
}

// ������ʱ���������ҵ
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

// ���������ҵ��������ҵ����β��
void finish_task(Task *task_list,PCB *finish_process){
	Task *finish_task = (Task*)malloc(sizeof(Task));
	if(finish_task == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
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

// �����ȷ���ѡ����ҵ
void change_task(Node *node_list,Task *task_list,PCB *process_list){
	/*
	���ڽ������5�����������ȴ������������еĽ��������Ϊ5��
	���Դ���ҵ������ѡ����ҵ����ʹ���̳���5��������Ҫ������ҵ�Ƿ񵽴�
	*/
	// ѡ����ҵ
	Task *move = task_list->next;
	while(sum < 5 && move != NULL && move->status != status_finish && move->arrive_time <= Total_time){
		// �ȱ���Ҫ�����ڴ����ҵ
		Task *move_task = move;
		// Ϊ����ҵ��������
		if(create_process(process_list,move_task,node_list)){
			// �����ڴ�ɹ�
			sum++;
			// �޳���ҵ
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

// �����½���
bool create_process(PCB *process_list,Task *move_task,Node *node_list){
	PCB *new_process = (PCB*)malloc(sizeof(PCB));
	if(new_process == NULL){
		printf("��̬�ڴ����ʧ�ܣ�");
	}
	// ������
	sprintf(new_process->process_name,"%s%d","P",++number);
	// ����ʱ��
	new_process->arrive_time = move_task->arrive_time;
    // ����ʱ��
    new_process->need_time = move_task->need_time;
    // ����״̬
    new_process->status = move_task->status;
    // ����ʱ��
	new_process->block_time = 0;
	// ռ���ڴ���ַ
	new_process->begin = 0;
    // ռ���ڴ��С
    new_process->size = move_task->size;
 	// ռ����ҵ
	sprintf(new_process->task_name,"%s",move_task->task_name);
	// ����ָ��
	new_process->next = NULL;
	// Ϊ�½��̷����ڴ�
	if(!first_fit(node_list,new_process)){
		free(new_process);
		return false;
	}
	// ���½��̷�����̶���
	insert_process(process_list,new_process);
	return true;
}

// ����ʼ��ַ�����½���
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

// �����������
void block_process(PCB *process_list){
	// �����������������
	int block_id = rand()%number+1;
	char block_pro[5];
	sprintf(block_pro,"%s%d","P",block_id);
	PCB *move = process_list->next;
	while(move != NULL){
		if(strcmp(move->process_name,block_pro) == 0){
			// ���½���״̬
			move->status = status_block;
			// ������ʱ����0
			move->block_time = 0;
			printf("�������̣�%s\n",move->process_name);
			return;
		}
		move = move->next;
	}
}

// ���ѽ���
void notify_process(PCB *process_list){
	PCB *move = process_list->next;
	while(move != NULL){
		if(move->block_time >= Block_Time){
			// ���½���״̬
			move->status = status_wait;
			// ������ʱ����0
			move->block_time = 0;
			printf("���ѽ��̣�%s\n",move->process_name);
		}
		move = move->next;
	}
}

// ʱ��Ƭ��ת�����㷨
void round_robin(Node *node_list,Task *task_list,PCB *process_list){
    /*
    ��һ��ȫ�ֱ�����¼��ǰ����������ǰ������С��5ʱ������ҵ�����е����ѵ�����ҵ��Ϊ��ҵ�������̷����ڴ�
	��һ��ָ���¼��ǰִ�еĽ��̣�ÿһ�δӸý��̿�ʼ�ҽ���Ϊwait��ִ��һ��ʱ��Ƭ
	��һ���������ʱ���ͷŸý���ռ�õ��ڴ沢��������ɵ���ҵ��������ҵ���е�ĩβ
	�������ȹ���ֱ������ҵ���еĵ�һ����ҵΪ���״̬�Լ����̶���Ϊ�������
	�ڽ��̵��ȵĹ����У�����������̣�һ����ʱ����ѽ���
     */
    // ��¼��ǰִ�н��̵�λ�ã���ʼָ����̶��еĵ�һ������
	runIndex = process_list->next;
    while (sum != 0 || task_list->next->status != status_finish) {
    	// ����ɽ�����
		int old_sum = sum;
    	// ��ǰ������С��5������Ȼ����ҵδ��ɣ��Ӻ󱸶���ѡ����ҵ
		if(sum < 5 && task_list->next != NULL && task_list->next->status != status_finish){
			change_task(node_list,task_list,process_list);
		}
		// �����������
		block_process(process_list);
		// ���ѽ���
		notify_process(process_list);
		// �պ�runIndexָ��NULL�ҵ�ǰ����������
		if(runIndex == NULL && old_sum < sum){
			// �ҳ�֮ǰ���̵����һ��
			int move_sum = 0;
			PCB *move = process_list;
			while(move_sum < old_sum){
				move_sum++;
				move = move->next;
			}
			// runIndexָ���¼���Ľ���
			runIndex = move->next;
		}else if(runIndex == NULL){
			// ��runIndexָ��NULL�ҵ�ǰ������δ����
			// runIndex����ָ����̶��еĵ�һ������
			runIndex = process_list->next;
		}
		// �������ϴ���runIndex��ȻΪNULL����ǰ�޽�������ҵ����
		if(runIndex == NULL){
			printf("��ǰ�޽���������ҵ���\n");
			printf("�ȴ���ҵ�������������\n");
			printf("�ȴ�ʱ����%d\n",task_list->next->arrive_time - Total_time);
			Total_time += task_list->next->arrive_time - Total_time;
			printf("���̵�����ʱ�䣺%d\n",Total_time);
			continue;
		}
		// ѭ���������̶��У��ҵ�����״̬Ϊwait�Ľ��̲�ִ��
		PCB *memoryIndex = runIndex;
		while(runIndex->status != status_wait){
			runIndex = runIndex->next;
			// runIndex�ߵ����̶���ĩβ
			if(runIndex == NULL){
				runIndex = process_list->next;
			}
			// runIndexѭ��һ������Ҳ���״̬Ϊwait�Ľ���
			if(runIndex == memoryIndex){
				show_process(process_list);
				// ��ʾ���н��̶�����
				// �ҳ������������̱���������Ľ��̣��ȴ��ý��̱�����
				int notify_time = Block_Time - process_list->next->block_time;
				PCB *move = process_list->next;
				while(move != NULL){
					// ����о����������̱����Ѹ����Ľ���
					if(Block_Time - move->block_time < notify_time){
						// ���»���ʱ��
						notify_time = Block_Time - move->block_time;
					}
					move = move->next;
				}
				printf("��ǰ����ȫ���������ȴ����̱����ѣ�\n");
				printf("�ȴ�ʱ����%d\n",notify_time);
				// ��ʱ���ҳ������������̱��������ʱ��
				move = process_list->next;
				while(move != NULL){
					// �ȴ����̱�����
					// �������н��̵�����ʱ��
					move->block_time += notify_time;
					move = move->next;
				}
				// ���½��̵�����ʱ��
				Total_time += notify_time;
				show_process(process_list);
				// ���ѽ���
				notify_process(process_list);
				show_process(process_list);
				// ���½��н��̵���
				printf("���̵�����ʱ�䣺%d\n",Total_time);
				continue;
			}
		}
		// ִ�е�ǰ����
		runIndex->status = status_run;
		show_process(process_list);
		// �жϸý����Ƿ������
        if (runIndex->need_time-TIME_SLICE > 0) {
        	// ����ִ��һ��ʱ��Ƭ���޷����
            // ���½��̵�����ʱ��
        	Total_time += TIME_SLICE;
			// ���½�������ʱ��
        	runIndex->need_time -= TIME_SLICE;
        	// ���Ľ���״̬
			runIndex->status = status_wait;
        	// ָ����һ����
			runIndex = runIndex->next;
			// ��������ʱ��
			PCB *p = process_list->next;
			while(p != NULL){
				if(p->status == status_block){
					p->block_time += TIME_SLICE;
				}
				p = p->next;
			}
        }else{
        	// ���̿������
        	// ��������ʱ��
			PCB *p = process_list->next;
			while(p != NULL){
				if(p->status == status_block){
					p->block_time += runIndex->need_time;
				}
				p = p->next;
			}
			// ���½��̵�����ʱ��
        	Total_time += runIndex->need_time;
			// ���½�������ʱ��
			runIndex->need_time = 0;
			runIndex->status = status_finish;
			show_process(process_list);
        	// �Ƴ��ý���ͬʱ�ͷ��ڴ沢��������ҵ����������ҵ����
        	PCB *finish_process = runIndex;
        	PCB *move = process_list;
        	while(move->next != runIndex){
        		move = move->next;
			}
			// �Ƴ�����
			move->next = runIndex->next;
			// ָ����һ����
			runIndex = move->next;
			// ��������ҵ����������ҵ���в��ͷ��ڴ�
			finish_task(task_list,finish_process);
			recycle(node_list,finish_process);
			free(finish_process);
			sum--;
		}
		show_node(node_list);
		show_task(task_list);
		show_process(process_list);
		printf("���̵�����ʱ�䣺%d\n",Total_time);
    }
}

// չʾ���ڴ�������ڴ�������
void show_node(Node *node_list){
	Node *move = node_list->next;
	printf("************************************************************************************\n");
	printf("                 �ڴ������                 \n");
	printf("������������������������������������������\n");
	printf("|��ʼ��ַ   ������С   ռ�ý���   ����״̬|\n");
	while(move != NULL){
		if(move->status == 0){
			printf("|   %d\t      %d\t   %s\t    Free  |\n",move->begin,move->size,move->process_name,move->status);
		}else{
			printf("|   %d\t      %d\t   %s\t    Busy  |\n",move->begin,move->size,move->process_name,move->status);
		}
		move = move->next;
	}
	printf("������������������������������������������\n");
}

// չʾ������ҵ���С���ҵ������
void show_task(Task *task_list){
	Task *move = task_list->next;
	printf("                    ����ҵ����                    \n");
	printf("����������������������������������������������������\n");
	printf("|��ҵ��   ����ʱ��   ����ʱ��   �����ڴ�   ��ҵ״̬|\n");
	while(move != NULL){
		printf("|  %s\t     %d\t        %d\t  %d\t      %c    |\n",move->task_name,move->arrive_time,move->need_time,move->size,move->status);
		move = move->next;
	}
	printf("����������������������������������������������������\n");
}

// չʾ�����������������ҵ������
void show_process(PCB *process_list){
	PCB *move = process_list->next;
	printf("                                  ��ǰ������PCB��Ϣ                                  \n");
	printf("�������������������������������������������\n");
	printf("|������   ����ʱ��   ����ʱ��   ����״̬   ����ʱ��   �ڴ���ַ   �ڴ��С   ռ����ҵ|\n");
	while(move != NULL){
		printf("|  %s\t     %d\t        %d\t   %c\t      %d\t         %d\t    %d\t       %s   |\n",
		move->process_name,move->arrive_time,move->need_time,move->status,move->block_time,move->begin,move->size,move->task_name);
		move = move->next;
	}
	printf("�������������������������������������������\n");
}
