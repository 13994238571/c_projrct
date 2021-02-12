#include <stdio.h>
#include <stdlib.h>

typedef struct Persion
{
    char name[64];
    int age; //不要在定义时赋初值
	

}myPerson; //写typedef myPersion 是结构体变量的别名

void test01() {
	myPerson p = {"aa", 10};

}

struct Peesion2
{
	char name[64];
	int age;

}myPersion = {"bb", 200}; //myPersion 是一个结构体变量

struct
{
	char name[64];
	int age;



}myPersion3 = {"ccc", 30};
void test02()
{
	printf("姓名： %s年龄: %d\n", myPersion.name, myPersion.age);
	myPersion.age = 100;
	strcpy(myPersion.name, "aaa");
	printf("姓名， %s年龄： %d\n",myPersion.name, myPersion.age);



}

void test04()
{
	//在栈上创建结构体变量
	struct Persion p1 = {"aaa", 10};
	//在堆上创建结构体体变量
	struct Persion* p2 = malloc(sizeof(struct Persion));
	p2->age = 100;
	strcpy(p2->name, "bbb");
}

void printfArray(struct Persion persions[], int len)
{
	for(int i = 0; i < len; i++) {
		printf("姓名：%s, 年龄 %d\n", persions[i].name, persions[i].age);
	
	
	}



}	

void test05()
{
	//结构体变量数组、
	//在栈上创建
	struct Persion persions[] =
	{
		{"aaa", 10},
		{"bbb", 20},
		{"ccc", 30},
		{"ddd", 40},
	};
    int len = sizeof(persions)/sizeof(struct Persion);
	 printfArray(persions, len);

    //在堆区创建
	struct Persion* personArr = malloc(sizeof(struct Persion)*4);
	for (int i = 0; i < 4; i++) {
		sprintf(personArr[i].name, "name_%d", i);
		personArr[i].age = i+18;

	}

	printfArray(personArr, 4);
	if(personArr != NULL);
	{
		free(personArr);
		personArr = NULL;
	
	}


}


int main(int argc, char *argv[]) {
    //test02();
	test05();

}
