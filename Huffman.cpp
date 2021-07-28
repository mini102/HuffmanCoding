#define _CRT_SECURE_NO_WARNINGS
#define BufSize =100;
#define NUM_ASCII 256

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//허프만 코딩: 인코더, 디코더 포함
//입력 파일: input.txt  (텍스트 파일)
//인코딩된 파일: input.encoded  (바이너리 파일)
//디코딩된 파일: decodedinput.txt  (텍스트 파일)
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
int totalBitNum = 0;  //전체 비트 개수

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
		if (findVertex(v) == 0) //중복된 것이 없을 경우
		{
			createVertex(v); //새로운 노드를 만들어줌
		}
		else
		{
			node*find = findVertex(v); //중복된 것이 있을 경우
			find->cnt += 1;  //중복된 노드의 cnt를 +1
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
		node* first = findSmallest();   // 빈도수 제일 낮은 것을 뽑는다.
		node* second = findSmallest();
		//printf("first: %d, second: %d\n", first->cnt, second->cnt);

		if (second == 0)   // 마지막에 트리만 뽑힌다.
		{
			return first;
		}

		node*newOne = (node*)malloc(sizeof(node));
		newOne->next = 0;
		newOne->left = first;
		newOne->right = second;
		newOne->cnt = first->cnt + second->cnt;

		addToSLL(newOne);   // 트리 전체를 구성원으로 넣는다.
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
		//symCode에 심볼((int)cur->v))에 해당하는 허프만 코드(codeBuf) 저장
		char *hufCode = (char*)malloc(strlen(codeBuf) + 1); //null때문에 +1
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
		printf("파일을 열 수 없습니다\n");
		return;
	}

	FILE *fout = fopen("input.encoded", "wb");//출력파일 지정, 문자와 그의 허프만 코드, 총 갯수를 binary 파일에 출력한다.
	if (fout != 0)    //"input.encoded"에 디코딩에 필요한 정보를 입력한다.
	{
		//input.txt를 다시 연다.
		FILE *fin = fopen("input.txt", "rt");
		if (fin != 0)
		{
			//전체 비트 개수를 기록할 위치를 기억해둔다.
			int localTotalNumBit;
			localTotalNumBit = ftell(fout);
			if (fseek(fout, 4, SEEK_CUR) != 0)
			{
				printf("파일 포인터 이동 실패\n");
				fclose(fin);
				fclose(fout);
				return;
			}
			char bitBuf[100]; //생성되는 비트스트림을 저장할 버퍼
			int bitBufIdx = 0;   //현재 비트스트림을 저장하는 위치
			int bitShiftCnt = 7;  //비트 시프트 회수
			char flag = 0; //flag =0이면 기록할 것이 없음을 의미

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
							//printf("0들어왔을 때 val: %d\n", val);
						}
						else if (huffmanCode[j] == '1')
						{
							val = 1;
							//printf("1들어왔을 때 val: %d\n", val);
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
			}   //파일이 100바이트가 안될 경우
			if (flag == 1) //bitBuf에 값이 남아있는 경우
			{
				fwrite(bitBuf, 1, bitBufIdx + 1, fout);
			}
			if (fseek(fout, localTotalNumBit, SEEK_SET) == 0) {
				fwrite(&totalBitNum, sizeof(totalBitNum), 1, fout);
			}
			else {
				printf("totalbit수를 기록할 수 없습니다,\n");
			}
		}
		else
		{
			printf("파일을 열 수 없습니다.\n");
		}
		printf("*******ECODING SUCCESS********\n");
		fclose(f);
		fclose(fout);
	}
}


void decoding() {   //input.encoded--> decodedinput.txt

	FILE *fin = fopen("input.encoded", "rb");
	//char symbolAndLen[2];  //0: symbol, 1:길이
	FILE *dc = fopen("decodedinput.txt", "wt");

	if (dc == 0) {
		printf("파일을 열지 못했습니다\n");
		exit(0);
	}

	if (fin != 0) {
		node *cur = huffRoot;

		cur = huffRoot;

		//디코딩 수행

		FILE *dc = fopen("decodedinput.txt", "wt");

		int numBitsToRead = 0; //읽어들여야할 비트의 총 개수
		fread(&numBitsToRead, sizeof(int), 1, fin);
		printf("numBitsToRead는 %d\n", numBitsToRead);

		if (dc == 0) {
			printf("파일을 열지 못했습니다\n");
			exit(0);
		}

		cur = huffRoot;
		char buf[100];
		while (1)
		{
			int sz = fread(buf, 1, 100, fin);   //읽을 바이트 수
			//printf("sz: %d\n", sz);
			if (sz == 0)
			{
					//printf("파일 다 읽었음\n");
					break;
			}
			else {
				for (int i = 0; i < sz; i++) {
					for (int j = 0; j < 8; j++) { //바이트는 8개의 비트로 구성되어 있으니까
						if ((char)(buf[i] & 0x80) == 0) //왼쪽으로 내려가야함
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
								printf("디코딩 버전\n");
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
		printf("파일을 열 수 가 없습니다.\n");
	}
}
			
int main(void)
{
			FILE*f = 0;
			f = fopen("input.txt", "rt");
			//char* arr;
			char buf[1024];

			if (f == 0) {
				printf("파일을 열 수 없습니다\n");
				return -1;
			}

			while (!feof(f))
			{
				fgets(buf, 1023, f);  //input.txt에 있는 내용들이 buf에 1023만큼 저장됨.

				printf("buf : %s\n", buf);

				cntCharacterNum(buf); //temp 노드들에 각 문자와 그 개수가 저장됨
			}

			showAll();

			node* tree = buildHuffmanTree();

			// symCode를 초기화
			memset(symCode, 0, sizeof(symCode));
			huffRoot = tree;

			Traversal(tree->left, '0'); //허프만코드 부여
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