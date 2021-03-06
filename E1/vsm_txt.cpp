// vsm_txt.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

using namespace std;

//定义常量
const	int DICTIONARYMAXSIZE = 500;		//特征词典大小的最大值	
const   int DOCUMENTMAXSIZE = 100;		//文档个数的最大值

										//定义文件对象
fstream fin, fout;						//文件读写对象


										//子操作，定义为外部函数
char * getterm(char ch);							//特征词获取
char * gettype(char ch);							//特征词词性获取

													//特征词
struct term {
	int	 term_ID;				//特征词标号
	char t[10];					//特征词
	char type[5];				//特征词的词性
};
//类，特征词典
class TermDictionary {
public:
	struct term TermArray[DICTIONARYMAXSIZE];		//特征词数组
	int	dictionarysize;							//特征词典大小
public:
	TermDictionary() { dictionarysize = 0; }			//构造函数
	void CreateDictionary();						//特征词典生成
}TD;

//特征词典生成
void TermDictionary::CreateDictionary()
{
	fin.open("../res/58343", ios::in, 0);
	if (!fin) {
		cout << "打不开文件--文档集.txt!\n";
		exit(0);
	}
	char ch, str1[10], str2[5];
	char type[50][5] = { "a","ad","Ag","b","c","d","e","m","ms","o","p","q","u","w","y","z","" };    //不纳入特征词典的词性
	int id = 0, flag1 = 0, flag2 = 0;
	ch = fin.get();
	while (!fin.eof()) {
		if (ch == '\n') { ch = fin.get(); continue; }
		strcpy_s(str1, getterm(ch));
		for (int i = 0; i<dictionarysize; i++)			//提取特征词后,扫描特征词典,若词典中存在此特征词,则置标志位为1
			if (!strcmp(TD.TermArray[i].t, str1)) { flag1 = 1; break; }
		ch = fin.get();
		strcpy_s(str2, gettype(ch));
		for (int i = 0; i<50; i++)							//提取特征词词性以后,扫描不纳入特征词典的词性数组,若存在此词性,则置标志位为1
			if (!strcmp(str2, type[i])) { flag2 = 1; break; }
		if (!flag1 && !flag2) {						//若两个标志位均为0,则把特征词id,特征词,词性插入特征词典
			TD.TermArray[dictionarysize].term_ID = id;
			strcpy_s(TD.TermArray[dictionarysize].t, str1);
			strcpy_s(TD.TermArray[dictionarysize].type, str2);
			dictionarysize++;
			id++;
		}
		flag1 = 0; flag2 = 0;
		ch = fin.get();
		ch = fin.get();
	}
	fin.close();
	fout.open("特征词典.txt", ios::out, 0);
	if (!fout) {
		cout << "打不开文件--特征词典.txt!\n";
		exit(0);
	}
	for (int i = 0; i<dictionarysize; i++)			    //把特征词典输出到文件-特征词典.txt	
		fout << TD.TermArray[i].term_ID << "	" << TermArray[i].t << "	" << TD.TermArray[i].type << endl;
	fout.close();
}

//索引项
struct Index {
	int term_ID;				//特征词标号
	int TF[DOCUMENTMAXSIZE];	//词频数组
	int ni;						//词i在段落中出现的段落数目
};
//类，倒排索引表
class IndexTable {
public:
	struct Index IndexArray[DICTIONARYMAXSIZE];	//索引项数组
	int	documentsize;							//文档的个数
public:
	IndexTable() { documentsize = 0; }					//构造函数
	void CreateIndexTable();						//倒排索引表生成
}IT;

//倒排索引表生成
void IndexTable::CreateIndexTable()
{
	char ch;
	int i=0,j = 0;
	for (int i = 0; i<DICTIONARYMAXSIZE; i++)
		for (int j = 0; j<DOCUMENTMAXSIZE; j++)
			IT.IndexArray[i].TF[j] = 0;							//给词频矩阵赋初值
	fin.open("文档集.txt", ios::in, 0);
	if (!fin) {
		cout << "打不开文件--文档集.txt!\n";
		exit(0);
	}
	ch = fin.get();												//计算document的数目
	while (!fin.eof()) { if (ch == '\n')IT.documentsize++; ch = fin.get(); }
	IT.documentsize++;
	fin.close();
	//cout<<IT.documentsize<<endl;								
	//cout<<TD.dictionarysize<<endl;
	for (i = 0; i<TD.dictionarysize; i++) {							//计算词频TF
		fin.open("文档集.txt", ios::in, 0);
		if (!fin) {
			cout << "打不开文件--文档集.txt!\n";
			exit(0);
		}
		IT.IndexArray[i].term_ID = TD.TermArray[i].term_ID;
		//cout<<fin.tellg()<<endl;
		//fin.seekg(0,ios::beg);
		//cout<<fin.tellg()<<endl;
		ch = fin.get();
		while (!fin.eof()) {								//若读到的字符为文本结束符EOF,则扫描结束
			if (ch == '\n') { j++; ch = fin.get(); }				//若读到的字符为回车符\n，则段落数增1，扫描下一字符
			if (!strcmp(TD.TermArray[i].t, getterm(ch)))IT.IndexArray[i].TF[j]++; //若特征词典中当前获取特征词和此特征词相等，则TF[][]增1
			ch = fin.get();
			gettype(ch);
			ch = fin.get();
			ch = fin.get();
		}
		j = 0;
		fin.close();
	}
	fin.close();
	for (i = 0; i<TD.dictionarysize; i++)IT.IndexArray[i].ni = 0;
	//cout<<TD.dictionarysize<<endl;
	//cout<<IT.documentsize<<endl;
	for (i = 0; i<TD.dictionarysize; i++)					//计算ni,扫描倒排索引表，对于每一个特征词，统计大于0的词频个数
		for (int j = 0; j<IT.documentsize; j++)
			if (IT.IndexArray[i].TF[j])IT.IndexArray[i].ni++;
	fout.open("倒排索引表.txt", ios::out, 0);
	if (!fout) {
		cout << "打不开文件--倒排索引表.txt!\n";
		exit(0);
	}
	char s[100];
	for (i = 0; i<TD.dictionarysize; i++) {					//把特征词id,词频矩阵,ni输出到文件-倒排索引表.txt
		sprintf_s(s, "%3d", IT.IndexArray[i].term_ID);
		fout << s;
		//fout<<IT.IndexArray[i].term_ID<<"	";
		for (int j = 0; j<IT.documentsize; j++) {
			sprintf_s(s, "%5d", IT.IndexArray[i].TF[j]);
			fout << s;
			//fout<<IT.IndexArray[i].TF[j]<<"	";
		}
		sprintf_s(s, "%5d", IT.IndexArray[i].ni);
		fout << s << endl;
		//fout<<IT.IndexArray[i].ni<<endl;
	}
	fout.close();
}

//类，向量空间模型
class VectorSpaceModel {
public:
	double Vector[DICTIONARYMAXSIZE][DOCUMENTMAXSIZE];		//向量矩阵
public:
	void	CreateVSM();				//向量空间模型生成
	double * GetVector(int j);			//获取第j个特征向量
	void   PrintVetor(int j);			//打印第j个特征向量
}VSM;

//向量空间模型生成
void VectorSpaceModel::CreateVSM()
{
	char s[100];
	double IDF[DICTIONARYMAXSIZE];
	for (int i = 0; i<TD.dictionarysize; i++) {						//计算idfi = log(N/ni)
		IDF[i] = log((double)IT.documentsize / (double)IT.IndexArray[i].ni) / log(10);
		//printf("%.20f\n",IDF[i]);
	}
	double TFIDF[DICTIONARYMAXSIZE][DOCUMENTMAXSIZE];
	double SQU[DOCUMENTMAXSIZE];								//向量元素平方和
	double MOD[DOCUMENTMAXSIZE];								//向量模					
	for (int j = 0; j<IT.documentsize; j++) {
		SQU[j] = 0;
		for (int i = 0; i<TD.dictionarysize; i++) {
			TFIDF[i][j] = (double)IT.IndexArray[i].TF[j] * IDF[i];	//计算tfidfij=fj*log(N/ni)
			SQU[j] += TFIDF[i][j] * TFIDF[i][j];					//计算向量元素平方和
		}
		MOD[j] = sqrt(SQU[j]);									//计算向量模	
	}
	//生成文件--向量空间模型.txt
	fout.open("向量空间模型.txt", ios::out, 0);
	if (!fout) {
		cout << "打不开文件--向量空间模型.txt!\n";
		exit(0);
	}
	for (int i = 0; i<TD.dictionarysize; i++) {							//计算并打印向量空间模型
		for (int j = 0; j<IT.documentsize; j++) {
			VSM.Vector[i][j] = TFIDF[i][j] / MOD[j];			    //计算新的向量元素
			sprintf_s(s, "%15.10f", VSM.Vector[i][j]);
			fout << s;
			//fout<<"	"<<VSM.Vector[i][j]<<"	";
		}
		fout << endl;
	}
	fout.close();
	//生成文件--vsm.arff
	fout.open("vsm.arff", ios::out, 0);
	if (!fout) {
		cout << "打不开文件--vsm.arff!\n";
		exit(0);
	}
	fout << "@relation" << "	" << "vsm" << endl;
	for (int i = 0; i<TD.dictionarysize; i++) {
		fout << "@attribute" << "	" << "term" << i + 1;
		if (i<9)fout << "  ";
		else if (i<99)fout << " ";
		fout << "     " << "real" << endl;
	}
	fout << "@data" << endl;
	for (int j = 0; j<IT.documentsize; j++) {
		for (int i = 0; i<TD.dictionarysize; i++) {
			sprintf_s(s, "%6.6f", VSM.Vector[i][j]);
			fout << s;
			if (i<TD.dictionarysize - 1)fout << ',';
			//fout<<"	"<<VSM.Vector[i][j]<<"	";
		}
		fout << endl;
	}
	fout.close();
}

//获取第j个特征向量
double * VectorSpaceModel::GetVector(int j)
{
	double p[DICTIONARYMAXSIZE], *q;
	q = p;
	for (int k = 0; k<DICTIONARYMAXSIZE; k++)p[k] = VSM.Vector[k][j - 1];	//把特征向量存入p
	return q;
}

//打印第j个特征向量
void   VectorSpaceModel::PrintVetor(int j)
{
	char s[100];
	for (int k = 0; k<TD.dictionarysize; k++) {						    //打印特征向量
		sprintf_s(s, "%10.10f", VSM.Vector[k][j - 1]);
		cout << s << endl;
	}
}

//子操作，定义为外部函数
char * getterm(char ch)				//提取term的内容
{
	int i = 0;
	char str[10], *p;
	while (ch != '/') {					//把单个字符存入数组,直到碰到'/'
		str[i++] = ch; ch = fin.get();
	}
	str[i] = '\0';
	p = str;
	return p;
}
//子操作，定义为外部函数
char * gettype(char ch)			    //提取term的词性
{
	int i = 0;
	char str[5], *p;
	while (ch != EOF && ch != ' ') {		////把单个字符存入数组,直到碰到' '
		str[i++] = ch; ch = fin.get();
	}
	str[i] = '\0';
	p = str;
	return p;
}


//主函数
void main()
{
	TD.CreateDictionary();			//特征词典生成
	IT.CreateIndexTable();			//倒排索引表生成
	VSM.CreateVSM();				//向量空间模型生成
									//VSM.PrintVetor(3);			//打印第3个向量
}

