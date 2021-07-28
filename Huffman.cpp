#define _CRT_SECURE_NO_WARNINGS
#define BufSize =100;
#define NUM_ASCII 256

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//������ �ڵ�: ���ڴ�, ���ڴ� ����
//�Է� ����: input.txt  (�ؽ�Ʈ ����)
//���ڵ��� ����: input.encoded  (���̳ʸ� ����)
//���ڵ��� ����: decodedinput.txt  (�ؽ�Ʈ ����)
struct node
{
	char v;
	int cnt;
	char code;
	node*next;
	node*left;
	node*right;
};

node*head = 0;
node *huffRoot = 0;

char codeBuf[100];
int  codeBufIdx = -1;
char *symCode[NUM_ASCII];
int numOfSym = 0;
char code[100];
int totalBitNum = 0;  //��ü ��Ʈ ����

void addToSLL(node* cur)
{
	if (head == 0)
	{
		head = cur;
		return;
	}

	node*temp = head;
	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = cur;
}


void createVertex(char _v)
{
	node*newOne = (node*)malloc(sizeof(node));
	newOne->v = _v;
	newOne->cnt = 1;
	newOne->left = newOne->right = newOne->next = 0;

	addToSLL(newOne);
}

node* findVertex(char v)
{
	node*temp = head;
	while (temp != 0)
	{
		if (temp->v == v || (temp->v) + 32 == v)
		{
			return temp;
		}
		temp = temp->next;
	}
	return 0;
}

void cntCharacterNum(char* _buf)
{

	char v;
	for (int i = 0; i < 99; i++)
	{
		if (_buf[i] == '\0')
		{
			return;
		}
		v = _buf[i];
		if (findVertex(v) == 0) //�ߺ��� ���� ���� ���
		{
			createVertex(v); //���ο� ��带 �������
		}
		else
		{
			node*find = findVertex(v); //�ߺ��� ���� ���� ���
			find->cnt += 1;  //�ߺ��� ����� cnt�� +1
		}
	}
}

node*findSmallest()
{

	int short_num = INT_MAX;
	node*short_node = head;

	if (head == 0)
	{
		return 0;
	}

	node*temp = head;
	while (temp != 0)
	{
		if (temp->cnt < short_num)
		{
			short_node = temp;
			short_num = temp->cnt;
		}
		temp = temp->next;
	}

	if (short_node == head)
	{
		head = head->next;
	}
	else
	{
		node*prev = head;
		//printf("prev: %c\n", prev->next->v);
		while (prev->next != short_node)
		{
			prev = prev->next;
		}
		prev->next = short_node->next;//prev = short
		//head->next = prev->next;
		//printf("%c , %c\n", prev->v, short_node->v);
	}

	short_node->next = 0;
	return short_node;
}

node* buildHuffmanTree()
{
	while (1)
	{
		node* first = findSmallest();   // �󵵼� ���� ���� ���� �̴´�.
		node* second = findSmallest();
		//printf("first: %d, second: %d\n", first->cnt, second->cnt);

		if (second == 0)   // �������� Ʈ���� ������.
		{
			return first;
		}

		node*newOne = (node*)malloc(sizeof(node));
		newOne->next = 0;
		newOne->left = first;
		newOne->right = second;
		newOne->cnt = first->cnt + second->cnt;

		addToSLL(newOne);   // Ʈ�� ��ü�� ���������� �ִ´�.
	}
}


void showAll()
{
	node*temp = head;
	while (temp != 0)
	{
		printf("%c : %d\n", temp->v, temp->cnt);
		temp = temp->next;
	}
}


void Traversal(node* cur, char c) {
	codeBufIdx++;
	codeBuf[codeBufIdx] = c;
	codeBuf[codeBufIdx + 1] = 0; //null
	if (cur->left == 0 && cur->right == 0) {
		printf("%c: %s\n", cur->v, codeBuf);
		/*for (int i = 0; i < cur->cnt; i++) {
			strcat(code, codeBuf);
		}*/
		//symCode�� �ɺ�((int)cur->v))�� �ش��ϴ� ������ �ڵ�(codeBuf) ����
		char *hufCode = (char*)malloc(strlen(codeBuf) + 1); //null������ +1
		strcpy(hufCode, codeBuf);
		symCode[(int)cur->v] = hufCode;
	}
	else
	{
		Traversal(cur->left,'0');
		Traversal(cur->right,'1');
	}
	codeBuf[codeBufIdx] = 0;
	codeBufIdx--;
	//printf("binarycode: %s\n",code);
	return;
}

void encoding() {

	FILE*f = 0;
	f = fopen("input.txt", "rt");
	//char* arr;
	char buf[1024];

	if (f == 0) {
		printf("������ �� �� �����ϴ�\n");
		return;
	}

	FILE *fout = fopen("input.encoded", "wb");//������� ����, ���ڿ� ���� ������ �ڵ�, �� ������ binary ���Ͽ� ����Ѵ�.
	if (fout != 0)    //"input.encoded"�� ���ڵ��� �ʿ��� ������ �Է��Ѵ�.
	{
		//input.txt�� �ٽ� ����.
		FILE *fin = fopen("input.txt", "rt");
		if (fin != 0)
		{
			//��ü ��Ʈ ������ ����� ��ġ�� ����صд�.
			int localTotalNumBit;
			localTotalNumBit = ftell(fout);
			if (fseek(fout, 4, SEEK_CUR) != 0)
			{
				printf("���� ������ �̵� ����\n");
				fclose(fin);
				fclose(fout);
				return;
			}
			char bitBuf[100]; //�����Ǵ� ��Ʈ��Ʈ���� ������ ����
			int bitBufIdx = 0;   //���� ��Ʈ��Ʈ���� �����ϴ� ��ġ
			int bitShiftCnt = 7;  //��Ʈ ����Ʈ ȸ��
			char flag = 0; //flag =0�̸� ����� ���� ������ �ǹ�

			memset(bitBuf, 0, 100);
			while (fgets(buf, 100, f) != 0)
			{
				int len = strlen(buf);
				for (int i = 0; i < len; i++) {
					unsigned char val;
					char *huffmanCode = symCode[(int)buf[i]];
					for (int j = 0; j < strlen(huffmanCode); j++) {
						//char val = 0;
						val = 0;
						if (huffmanCode[j] == '0')
						{
							val = 0;
							//printf("0������ �� val: %d\n", val);
						}
						else if (huffmanCode[j] == '1')
						{
							val = 1;
							//printf("1������ �� val: %d\n", val);
						}
						else { printf("-----------------------ERROR-------------------"); }
						//printf("val: %d\n", val);
						val = val << bitShiftCnt;
						//printf("val: %d\n", val);
						bitShiftCnt--;
						bitBuf[bitBufIdx] = bitBuf[bitBufIdx] | val;
						flag = 1;
						//printf("bitBuf: %d\n", bitBuf[bitBufIdx]);
						totalBitNum++;
						if (bitShiftCnt < 0)
						{
							bitShiftCnt = 7;
							bitBufIdx++;
							if (bitBufIdx >= 100) {
								fwrite(bitBuf, 1, 100, fout);
								flag = 0;
								bitBufIdx = 0;
								memset(bitBuf, 0, 100);
							}
						}
					}
				}
			}   //������ 100����Ʈ�� �ȵ� ���
			if (flag == 1) //bitBuf�� ���� �����ִ� ���
			{
				fwrite(bitBuf, 1, bitBufIdx + 1, fout);
			}
			if (fseek(fout, localTotalNumBit, SEEK_SET) == 0) {
				fwrite(&totalBitNum, sizeof(totalBitNum), 1, fout);
			}
			else {
				printf("totalbit���� ����� �� �����ϴ�,\n");
			}
		}
		else
		{
			printf("������ �� �� �����ϴ�.\n");
		}
		printf("*******ECODING SUCCESS********\n");
		fclose(f);
		fclose(fout);
	}
}


void decoding() {   //input.encoded--> decodedinput.txt

	FILE *fin = fopen("input.encoded", "rb");
	//char symbolAndLen[2];  //0: symbol, 1:����
	FILE *dc = fopen("decodedinput.txt", "wt");

	if (dc == 0) {
		printf("������ ���� ���߽��ϴ�\n");
		exit(0);
	}

	if (fin != 0) {
		node *cur = huffRoot;

		cur = huffRoot;

		//���ڵ� ����

		FILE *dc = fopen("decodedinput.txt", "wt");

		int numBitsToRead = 0; //�о�鿩���� ��Ʈ�� �� ����
		fread(&numBitsToRead, sizeof(int), 1, fin);
		printf("numBitsToRead�� %d\n", numBitsToRead);

		if (dc == 0) {
			printf("������ ���� ���߽��ϴ�\n");
			exit(0);
		}

		cur = huffRoot;
		char buf[100];
		while (1)
		{
			int sz = fread(buf, 1, 100, fin);   //���� ����Ʈ ��
			//printf("sz: %d\n", sz);
			if (sz == 0)
			{
					//printf("���� �� �о���\n");
					break;
			}
			else {
				for (int i = 0; i < sz; i++) {
					for (int j = 0; j < 8; j++) { //����Ʈ�� 8���� ��Ʈ�� �����Ǿ� �����ϱ�
						if ((char)(buf[i] & 0x80) == 0) //�������� ����������
						{
								cur = cur->left;
						}
						else {
								cur = cur->right;
						}
						buf[i] = buf[i] << 1;
						numBitsToRead--;

						if (cur->left == 0 && cur->right == 0)
						{
								printf("%c", cur->v);
								fputc(cur->v, dc);
								cur = huffRoot;
						}
						if (numBitsToRead == 0)
						{
								printf("���ڵ� ����\n");
								fclose(fin);
								fclose(dc);
								break;
						}
						}
					}
				}
		}
		printf("*******DECODING SUCCESS********\n");
	}
	else {
		printf("������ �� �� �� �����ϴ�.\n");
	}
}
			
int main(void)
{
			FILE*f = 0;
			f = fopen("input.txt", "rt");
			//char* arr;
			char buf[1024];

			if (f == 0) {
				printf("������ �� �� �����ϴ�\n");
				return -1;
			}

			while (!feof(f))
			{
				fgets(buf, 1023, f);  //input.txt�� �ִ� ������� buf�� 1023��ŭ �����.

				printf("buf : %s\n", buf);

				cntCharacterNum(buf); //temp ���鿡 �� ���ڿ� �� ������ �����
			}

			showAll();

			node* tree = buildHuffmanTree();

			// symCode�� �ʱ�ȭ
			memset(symCode, 0, sizeof(symCode));
			huffRoot = tree;

			Traversal(tree->left, '0'); //�������ڵ� �ο�
			Traversal(tree->right, '1');

			for (int i = 0; i < NUM_ASCII; i++) {
				if (symCode[i] != 0) {
					numOfSym++;
					printf("Symbol %c ==> %s\n", (char)i, symCode[i]);
				}
			}
			printf("numOfSym is %d\n", numOfSym);

			encoding();

			decoding(); 

			return 0;
}