// LL1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include"ll1.h"
#include<iostream>
using namespace std;
int main()
{
	cout << "输入文件：" << endl;
	string file;
	cin >> file;
	LL1 ll(file);
		
	ll.analysis();

	system("pause");
	return 0;
	
}
