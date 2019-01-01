#include "pch.h"
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<stack>
#include<set>
#include<cstring>
#include <fstream>
#include <iomanip>
using namespace std;

class LL1
{
private:
	set<string> Ter_colt;//终结符
	set<string> Non_colt;//非终结符
	vector<string>Chars;   //字符表

	vector<vector<string>> Production; //产生式

	vector<set<string>> first;//First集

	vector<set<string>> follow;//Follow集

	vector < vector<int>>PredictTable;//预测分析表
	int charnum;//记录所有的字符数
	string file;//输入文件
	string AnalyStr;//
	ofstream out;
public:
	LL1() {
		file = "test.txt";
		AnalyStr = "";
		string s = "Result_";
		s = s+ "test.txt";
		out.open(s);
	}
	LL1(string f)
	{
		file = f;
		AnalyStr = "";
		string s = "Result_";
		s = s + f;
		out.open(s);
	}
	~LL1() 
	{
		vector < vector<int>>().swap(PredictTable);
		vector<set<string>>().swap(follow);
		vector<set<string>>().swap(first);
		vector<vector<string>>().swap(Production);
		vector<string>().swap(Chars);
		Non_colt.clear();
		Ter_colt.clear();
	}

	int getCharIndex(string target)//获取当前字符在字符表中的下标
	{
		int index = 0;
		for (int i = 0; i < charnum; i++)
		{
			if (target == Chars[i])
			{
				index = i;
				break;
			}
		}
		return index;
	}

	void Initial(string file)
	{
		charnum = 0;
		ifstream in(file);
		string line;
		string x = "";
		Chars.push_back("$");
		Ter_colt.insert("$");
		charnum++;
		while (getline(in, line))
		{
			if (!line.empty())
			{
				if (line[0] == '1')//终结符
				{
					for (int i = 2; i < line.size(); i++)
					{
						if (line[i] != ' ')
						{
							x += line[i];
						}
						else if (line[i] == ' '&&x != "")
						{
							Chars.push_back(x);
							Ter_colt.insert(x);
							charnum++;
							x = "";
						}
					}
					if (x != "")
					{
						Chars.push_back(x);
						Ter_colt.insert(x);
						charnum++;
						x = "";
					}
					//空字
					Chars.push_back("@");
					Ter_colt.insert("@");
					charnum++;
					continue;
				}

				if (line[0] == '2')//非终结符
				{
					for (int i = 2; i < line.size(); i++)
					{
						if (line[i] != ' ')
						{
							x += line[i];
						}
						else if (line[i] == ' '&&x != "")
						{
							Chars.push_back(x);
							Non_colt.insert(x);
							charnum++;
							x = "";
						}
					}
					if (x != "")
					{
						Chars.push_back(x);
						Non_colt.insert(x);
						charnum++;
						x = "";
					}
					continue;
				}
				if (line[0] == '3')//要判别的句子
				{
					for (int i = 2; i < line.size(); i++)
					{
						AnalyStr += line[i];
					}
					continue;
				}
				else          //产生式
				{
					string ss = "";
					string rLeft;
					vector<string> temp;
					int i = 0;
					do {
						if (line[i] == ' ' || line[i] == '-')
						{
							if (ss != "")
							{
								rLeft = ss;
								ss = "";
								break;
							}
						}
						else
						{
							ss += line[i];
						}
						i++;
					} while (i < line.size());
					temp.push_back(rLeft);
					while (line[i] != '>')
					{
						i++;
					}
					for (int j = i + 1; j < line.size(); j++)
					{
						if (line[j] == ' '&&ss != "")
						{
							temp.push_back(ss);
							ss = "";
						}
						else if (line[j] != ' '&&line[j] != '|')
						{
							ss += line[j];
						}
						if (line[j] == '|')
						{
							if (ss != "")
							{
								temp.push_back(ss);
								ss = "";
							}
							Production.push_back(temp);
							temp.clear();
							temp.push_back(rLeft);
							ss = "";
						}
					}
					temp.push_back(ss);
					Production.push_back(temp);
					continue;
				}
			}
		}
		PredictTable = vector<vector<int>>(charnum, vector<int>(charnum, -1));
		first = vector<set<string>>(charnum);
		follow = vector<set<string>>(charnum);
	}

	set<string> Union(set<string> a, set<string> b)  //计算并集
	{
		set<string> temp;
		set<string>::iterator it;
		for (it = a.begin(); it != a.end(); it++)
		{
			temp.insert(*it);
		}
		for (it = b.begin(); it != b.end(); it++)
		{
			temp.insert(*it);
		}
		return temp;
	}

	set<string> deletechar(set<string> tt, string ch)//删除集合中的某个元素
	{
		set<string> r;
		set<string>::iterator it;
		for (it = tt.begin(); it != tt.end(); it++)
		{
			if (*it != ch)
			{
				r.insert(*it);
			}
		}
		return r;
	}

	set<string> getfirst(string vn)//求当前字符的First集
	{
		set<string> s;
		//如果字符为终结符，则将该终结符计入左部的first集
		if (Ter_colt.count(vn) != 0)
		{
			int index = getCharIndex(vn);
			return first[index];
		}
		else   //为非终结符
		{
			for (int i = 0; i < Production.size(); i++)//求该非终结符的first集
			{
				if (Production[i][0] == vn)//找到当前非终结符的产生式，求该非终结符的first集
				{
					string next = Production[i][1];//产生式右部第一个字符
					set<string> sn = getfirst(next);
					s = Union(s, sn);//合并,处理数组中重复的first集中的终结符
				}
			}
			return  s;
		}
	}

	void getfirst()
	{
		for (int i = 1; i < charnum; i++)   //对于终结符，其first集是其本身。
		{
			if (Ter_colt.count(Chars[i]) != 0)
			{
				int index = getCharIndex(Chars[i]);
				first[i].insert(Chars[index]);
			}

		}
		for (int j = 0; j < Production.size(); j++) //对于非终结符，扫描所有产生式
		{
			string Left = Production[j][0];//产生式左部
			if (Ter_colt.count(Production[j][1]) != 0)//如果产生式右部第一个字符为终结符，则将其计入左部first集
			{
				set<string> s;
				s.insert(Production[j][1]);
				first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], s);
				continue;
			}
			else // 如果产生式右部第一个字符为非终结符，求该非终结符的first集
			{
				int k = 0;
				bool IstoE = false;
				do {
					IstoE = false;//判断当前非终结符能否推出空字
					k++;
					if (k < Production[j].size())
					{
						string nextnon_colt = Production[j][k];//将指向产生式的指针右移
						set<string> tempfirst = getfirst(nextnon_colt);
						if (tempfirst.count("@") != 0)//含有空字，计算当前非终结符的非$ first集计入左部的first集
						{
							tempfirst = deletechar(tempfirst, "@");//删除空字
							IstoE = true;
						}
						//合并,处理数组中重复的first集中的终结符
						first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], tempfirst);
					}
					else //若已经到达产生式的最右部的非终结符，则将空字加入左部的first集
					{
						set<string> s;
						s.insert("@");
						//合并,处理数组中重复的first集中的终结符
						first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], s);
						break;
					}
				} //若当前非终结符能推出空字，则将指向产生式的指针右移，计算下一个字符的非$ first集计入左部的first集
				while (IstoE != false);
			}
		}
		print_first();
	}

	void print_first()
	{
		cout << "First集:" << endl;
		out << "First集:" << endl;
		for (int i = 1; i < charnum; i++)
		{
			if (Ter_colt.count(Chars[i]) == 0)
			{
				cout << "first[" << Chars[i] << "]: ";
				out << "first[" << Chars[i] << "]: ";
				set<string>::iterator j;
				for (j = first[i].begin(); j != first[i].end(); j++) {
					cout << *j << "  ";
					out << *j << "  ";
				}
				cout << endl;
				out << endl;
			}
		}
		cout << endl;
		out << endl;
	}

	bool IsFollowIncrease(vector<set<string>> temp)//判断Follow集是否还在增长
	{
		bool is = false;
		for (int ii = 0; ii < charnum; ii++)
		{
			if (follow[ii] != temp[ii])
			{
				is = true;
				break;
			}
		}
		return is;
	}

	void getfollow()
	{
		string firstP = Production[0][0];//对第一条产生式先添加‘$’
		set<string> te;
		te.insert("$");
		//对第一条产生式先添加‘$’
		vector<set<string>> tempfollow = vector<set<string>>(charnum);
		tempfollow[getCharIndex(firstP)] = Union(tempfollow[getCharIndex(firstP)], te);

		do {
			for (int i = 0; i < charnum; i++)//保存上一次计算出的follow集
			{
				follow[i] = tempfollow[i];
			}
			for (int i = 0; i < Production.size(); i++)    //遍历所有产生式
			{
				string Left = Production[i][0];               //产生式左部
				for (int j = 1; j < Production[i].size(); j++) //遍历产生式右部，计算每个非终结符的follow集
				{
					string now_Nt = Production[i][j];  //当前要求follow集的非终结符
					if (Ter_colt.count(now_Nt) == 0) //非终结符
					{
						int k = j;
						bool IstoE;//判断当前非终结符右部的字符都能推出空字
						do {
							IstoE = false;//判断当前非终结符右部的字符都能推出空字
							k++;
							if (k < Production[i].size())
							{
								//获取在产生式中在当前非终结符右部的字符
								if (Ter_colt.count(Production[i][k]) != 0)//为终结符,将该终结符加入当前非终结符的follow集
								{
									set<string> s;
									s.insert(Production[i][k]);
									tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], s);
									break;
								}
								else      //为非终结符
								{
									//获取在产生式中在当前非终结符右部的字符的first集
									set<string> nextFirst = first[getCharIndex(Production[i][k])];
									if (nextFirst.count("@") != 0)//含有空字
									{
										nextFirst = deletechar(nextFirst, "@");//删除空字
										IstoE = true;
									}
									//在当前非终结符右部的字符的非$ first集 加至当前非终结符的follow集中
									tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], nextFirst);
								}
							}
							else  //如果当前非终结符右部的字符都能推出空字
							{
								set<string> Leftfollow = tempfollow[getCharIndex(Left)];
								tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], Leftfollow);
								break;
							}

						} while (IstoE != false);//如果当前非终结符右部的字符能推出空字，则右移指针继续求follow集
					}
				}
			}
		} while (IsFollowIncrease(tempfollow)); //判断follow集是否还在增长，直到follow集不在增大为止

		print_follow();
	}

	void print_follow()
	{
		cout << "Follow集：" << endl;
		out << "Follow集：" << endl;
		for (int i = 1; i < charnum; i++)
		{
			if (Ter_colt.count(Chars[i]) == 0)
			{
				cout << "follow [" << Chars[i] << "]: ";
				out << "follow [" << Chars[i] << "]: ";
				set<string>::iterator j;
				for (j = follow[i].begin(); j != follow[i].end(); j++) {
					cout << *j << "  ";
					out << *j << "  ";
				}
				cout << endl;
				out << endl;
			}

		}
		cout << endl;
		out << endl;
	}

	void getTable()          //预测分析表
	{
		for (int i = 0; i < Production.size(); i++)   //扫所有产生式
		{
			if (Production[i][1] != "@")     //如果当前产生式不能推出空字
			{
				set<string> firsSet = first[getCharIndex(Production[i][1])];//产生式右部的第一个字符
				set<string>::iterator it;
				for (it = firsSet.begin(); it != firsSet.end(); it++)//考察产生式右部的第一个字符的first集
				{
					if (*it != "@")//对每个终结符，如果不为空字，将当前产生式直接加入预测分析表中
					{
						PredictTable[getCharIndex(Production[i][0])][getCharIndex(*it)] = i;

					}
					else        //有空字的，考察产生式右部的第一个字符的follw集，将当前产生式加入预测分析表中
					{
						set<string> follSet = follow[getCharIndex(Production[i][1])];
						set<string>::iterator itr;
						for (itr = follSet.begin(); itr != follSet.end(); itr++)//将当前产生式加入预测分析表中
						{
							PredictTable[getCharIndex(Production[i][0])][getCharIndex(*itr)] = i;
						}
					}
				}
			}
			else //如果当前产生式直接推出空字，将当前产生式直接加入预测分析表中
			{
				set<string> follSet = follow[getCharIndex(Production[i][0])];
				set<string>::iterator it;
				for (it = follSet.begin(); it != follSet.end(); it++)
				{
					PredictTable[getCharIndex(Production[i][0])][getCharIndex(*it)] = i;
				}
			}
		}
		printTable();
	}

	string get_Production(int index)  //由对应下标获得对应产生式。
	{
		string prs;
		if (index >= 0)
		{
			prs += Production[index][0];
			prs += "->";
			for (int i = 1; i < Production[index].size(); i++)
			{
				prs = prs + Production[index][i] + " ";
			}
		}
		else //无该产生式
		{
			prs = "Error";
		}
		return prs;
	}

	void printTable()
	{
		cout << "预测分析表：" << endl;
		out << "预测分析表：" << endl;
		for (int i = 0; i < charnum; i++)
		{
			if (Ter_colt.count(Chars[i]) != 0 && Chars[i] != "@") {
				cout << setw(15) << Chars[i]; out << setw(15) << Chars[i];
			}
		}
		cout << endl; out << endl;
		for (int i = 0; i < charnum; i++)
		{
			if (Non_colt.count(Chars[i]) != 0)
			{
				cout << Chars[i]; out << Chars[i];
				for (int j = 0; j < charnum - Non_colt.size() - 1; j++)
				{
					cout << setw(15) << get_Production(PredictTable[i][j]);
					out << setw(15) << get_Production(PredictTable[i][j]);
				}
				cout << endl; out << endl;
			}

		}
		cout << endl; out << endl;
	}

	string getStackString(vector<string> Stack)
	{
		string re = "";
		for (int i = Stack.size() - 1; i >= 0; i--)
		{
			re = re + Stack[i] + " ";
		}
		return re;
	}

	bool analysis()
	{
		Initial(file);
		getfirst();
		getfollow();
		getTable();
		string s = AnalyStr;
		bool result = true;
		vector<string> AnaStack;//分析栈
		vector<string> StrStack;//输入的字符栈
		StrStack.push_back("$");
		string ss = "";
		vector<string> temp;
		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] != ' ')
			{
				ss += s[i];
			}
			else if (s[i] == ' '&&ss != "")
			{
				temp.push_back(ss);
				ss = "";
			}
		}
		if (ss != "")
		{
			temp.push_back(ss);
			ss = "";
		}
		while (!temp.empty())
		{
			StrStack.push_back(temp.back());
			temp.pop_back();
		}
		AnaStack.push_back("$");
		AnaStack.push_back(Production[0][0]);
		cout << setw(20) << "分析栈" << setw(30) << "剩余输入串" << setw(20) << "Action" << endl;
		out << setw(20) << "分析栈" << setw(30) << "剩余输入串" << setw(20) << "Action" << endl;
		while (StrStack.size() > 0)
		{
			string an = AnaStack.back();
			string str = StrStack.back();
			Chars[0];
			Production[0];
			cout << setw(20) << getStackString(AnaStack) << setw(30) << getStackString(StrStack);//输出当前栈中的元素
			out << setw(20) << getStackString(AnaStack) << setw(30) << getStackString(StrStack);//输出当前栈中的元素
			if (an == str)   //匹配
			{
				if (an == "$"&&str == "$")//成功
				{
					cout << setw(20) << "Accepted..." << endl;
					out << setw(20) << "Accepted..." << endl;
					result = true;
					break;
				}
				else
				{
					cout << setw(20) << str + " Terminal" << endl;//匹配
					out << setw(20) << str + " Terminal" << endl;//匹配
					AnaStack.pop_back();
					StrStack.pop_back();
				}
			}
			else       //替换产生式
			{
				int A = getCharIndex(an);
				int S = getCharIndex(str);
				if (PredictTable[A][S] != -1)
				{
					cout << setw(20) << get_Production(PredictTable[A][S]) << endl;//输出当前栈中的元素
					out << setw(20) << get_Production(PredictTable[A][S]) << endl;//输出当前栈中的元素
					vector<string> Pro = Production[PredictTable[A][S]];//需要替换的产生式
					AnaStack.pop_back();
					if (Pro[1] != "@")
					{
						for (int j = Pro.size() - 1; j > 0; j--)//反序入栈
						{
							AnaStack.push_back(Pro[j]);
						}
					}
				}
				else
				{
					cout << setw(20) << " Error!!" << endl;//错误
					out << setw(20) << " Error!!" << endl;//错误
					result = false;
					break;
				}
			}

		}
		out.close();
		return result;
	}
};