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
	set<string> Ter_colt;//�ս��
	set<string> Non_colt;//���ս��
	vector<string>Chars;   //�ַ���

	vector<vector<string>> Production; //����ʽ

	vector<set<string>> first;//First��

	vector<set<string>> follow;//Follow��

	vector < vector<int>>PredictTable;//Ԥ�������
	int charnum;//��¼���е��ַ���
	string file;//�����ļ�
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

	int getCharIndex(string target)//��ȡ��ǰ�ַ����ַ����е��±�
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
				if (line[0] == '1')//�ս��
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
					//����
					Chars.push_back("@");
					Ter_colt.insert("@");
					charnum++;
					continue;
				}

				if (line[0] == '2')//���ս��
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
				if (line[0] == '3')//Ҫ�б�ľ���
				{
					for (int i = 2; i < line.size(); i++)
					{
						AnalyStr += line[i];
					}
					continue;
				}
				else          //����ʽ
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

	set<string> Union(set<string> a, set<string> b)  //���㲢��
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

	set<string> deletechar(set<string> tt, string ch)//ɾ�������е�ĳ��Ԫ��
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

	set<string> getfirst(string vn)//��ǰ�ַ���First��
	{
		set<string> s;
		//����ַ�Ϊ�ս�����򽫸��ս�������󲿵�first��
		if (Ter_colt.count(vn) != 0)
		{
			int index = getCharIndex(vn);
			return first[index];
		}
		else   //Ϊ���ս��
		{
			for (int i = 0; i < Production.size(); i++)//��÷��ս����first��
			{
				if (Production[i][0] == vn)//�ҵ���ǰ���ս���Ĳ���ʽ����÷��ս����first��
				{
					string next = Production[i][1];//����ʽ�Ҳ���һ���ַ�
					set<string> sn = getfirst(next);
					s = Union(s, sn);//�ϲ�,�����������ظ���first���е��ս��
				}
			}
			return  s;
		}
	}

	void getfirst()
	{
		for (int i = 1; i < charnum; i++)   //�����ս������first�����䱾��
		{
			if (Ter_colt.count(Chars[i]) != 0)
			{
				int index = getCharIndex(Chars[i]);
				first[i].insert(Chars[index]);
			}

		}
		for (int j = 0; j < Production.size(); j++) //���ڷ��ս����ɨ�����в���ʽ
		{
			string Left = Production[j][0];//����ʽ��
			if (Ter_colt.count(Production[j][1]) != 0)//�������ʽ�Ҳ���һ���ַ�Ϊ�ս�������������first��
			{
				set<string> s;
				s.insert(Production[j][1]);
				first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], s);
				continue;
			}
			else // �������ʽ�Ҳ���һ���ַ�Ϊ���ս������÷��ս����first��
			{
				int k = 0;
				bool IstoE = false;
				do {
					IstoE = false;//�жϵ�ǰ���ս���ܷ��Ƴ�����
					k++;
					if (k < Production[j].size())
					{
						string nextnon_colt = Production[j][k];//��ָ�����ʽ��ָ������
						set<string> tempfirst = getfirst(nextnon_colt);
						if (tempfirst.count("@") != 0)//���п��֣����㵱ǰ���ս���ķ�$ first�������󲿵�first��
						{
							tempfirst = deletechar(tempfirst, "@");//ɾ������
							IstoE = true;
						}
						//�ϲ�,�����������ظ���first���е��ս��
						first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], tempfirst);
					}
					else //���Ѿ��������ʽ�����Ҳ��ķ��ս�����򽫿��ּ����󲿵�first��
					{
						set<string> s;
						s.insert("@");
						//�ϲ�,�����������ظ���first���е��ս��
						first[getCharIndex(Left)] = Union(first[getCharIndex(Left)], s);
						break;
					}
				} //����ǰ���ս�����Ƴ����֣���ָ�����ʽ��ָ�����ƣ�������һ���ַ��ķ�$ first�������󲿵�first��
				while (IstoE != false);
			}
		}
		print_first();
	}

	void print_first()
	{
		cout << "First��:" << endl;
		out << "First��:" << endl;
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

	bool IsFollowIncrease(vector<set<string>> temp)//�ж�Follow���Ƿ�������
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
		string firstP = Production[0][0];//�Ե�һ������ʽ����ӡ�$��
		set<string> te;
		te.insert("$");
		//�Ե�һ������ʽ����ӡ�$��
		vector<set<string>> tempfollow = vector<set<string>>(charnum);
		tempfollow[getCharIndex(firstP)] = Union(tempfollow[getCharIndex(firstP)], te);

		do {
			for (int i = 0; i < charnum; i++)//������һ�μ������follow��
			{
				follow[i] = tempfollow[i];
			}
			for (int i = 0; i < Production.size(); i++)    //�������в���ʽ
			{
				string Left = Production[i][0];               //����ʽ��
				for (int j = 1; j < Production[i].size(); j++) //��������ʽ�Ҳ�������ÿ�����ս����follow��
				{
					string now_Nt = Production[i][j];  //��ǰҪ��follow���ķ��ս��
					if (Ter_colt.count(now_Nt) == 0) //���ս��
					{
						int k = j;
						bool IstoE;//�жϵ�ǰ���ս���Ҳ����ַ������Ƴ�����
						do {
							IstoE = false;//�жϵ�ǰ���ս���Ҳ����ַ������Ƴ�����
							k++;
							if (k < Production[i].size())
							{
								//��ȡ�ڲ���ʽ���ڵ�ǰ���ս���Ҳ����ַ�
								if (Ter_colt.count(Production[i][k]) != 0)//Ϊ�ս��,�����ս�����뵱ǰ���ս����follow��
								{
									set<string> s;
									s.insert(Production[i][k]);
									tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], s);
									break;
								}
								else      //Ϊ���ս��
								{
									//��ȡ�ڲ���ʽ���ڵ�ǰ���ս���Ҳ����ַ���first��
									set<string> nextFirst = first[getCharIndex(Production[i][k])];
									if (nextFirst.count("@") != 0)//���п���
									{
										nextFirst = deletechar(nextFirst, "@");//ɾ������
										IstoE = true;
									}
									//�ڵ�ǰ���ս���Ҳ����ַ��ķ�$ first�� ������ǰ���ս����follow����
									tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], nextFirst);
								}
							}
							else  //�����ǰ���ս���Ҳ����ַ������Ƴ�����
							{
								set<string> Leftfollow = tempfollow[getCharIndex(Left)];
								tempfollow[getCharIndex(now_Nt)] = Union(tempfollow[getCharIndex(now_Nt)], Leftfollow);
								break;
							}

						} while (IstoE != false);//�����ǰ���ս���Ҳ����ַ����Ƴ����֣�������ָ�������follow��
					}
				}
			}
		} while (IsFollowIncrease(tempfollow)); //�ж�follow���Ƿ���������ֱ��follow����������Ϊֹ

		print_follow();
	}

	void print_follow()
	{
		cout << "Follow����" << endl;
		out << "Follow����" << endl;
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

	void getTable()          //Ԥ�������
	{
		for (int i = 0; i < Production.size(); i++)   //ɨ���в���ʽ
		{
			if (Production[i][1] != "@")     //�����ǰ����ʽ�����Ƴ�����
			{
				set<string> firsSet = first[getCharIndex(Production[i][1])];//����ʽ�Ҳ��ĵ�һ���ַ�
				set<string>::iterator it;
				for (it = firsSet.begin(); it != firsSet.end(); it++)//�������ʽ�Ҳ��ĵ�һ���ַ���first��
				{
					if (*it != "@")//��ÿ���ս���������Ϊ���֣�����ǰ����ʽֱ�Ӽ���Ԥ���������
					{
						PredictTable[getCharIndex(Production[i][0])][getCharIndex(*it)] = i;

					}
					else        //�п��ֵģ��������ʽ�Ҳ��ĵ�һ���ַ���follw��������ǰ����ʽ����Ԥ���������
					{
						set<string> follSet = follow[getCharIndex(Production[i][1])];
						set<string>::iterator itr;
						for (itr = follSet.begin(); itr != follSet.end(); itr++)//����ǰ����ʽ����Ԥ���������
						{
							PredictTable[getCharIndex(Production[i][0])][getCharIndex(*itr)] = i;
						}
					}
				}
			}
			else //�����ǰ����ʽֱ���Ƴ����֣�����ǰ����ʽֱ�Ӽ���Ԥ���������
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

	string get_Production(int index)  //�ɶ�Ӧ�±��ö�Ӧ����ʽ��
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
		else //�޸ò���ʽ
		{
			prs = "Error";
		}
		return prs;
	}

	void printTable()
	{
		cout << "Ԥ�������" << endl;
		out << "Ԥ�������" << endl;
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
		vector<string> AnaStack;//����ջ
		vector<string> StrStack;//������ַ�ջ
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
		cout << setw(20) << "����ջ" << setw(30) << "ʣ�����봮" << setw(20) << "Action" << endl;
		out << setw(20) << "����ջ" << setw(30) << "ʣ�����봮" << setw(20) << "Action" << endl;
		while (StrStack.size() > 0)
		{
			string an = AnaStack.back();
			string str = StrStack.back();
			Chars[0];
			Production[0];
			cout << setw(20) << getStackString(AnaStack) << setw(30) << getStackString(StrStack);//�����ǰջ�е�Ԫ��
			out << setw(20) << getStackString(AnaStack) << setw(30) << getStackString(StrStack);//�����ǰջ�е�Ԫ��
			if (an == str)   //ƥ��
			{
				if (an == "$"&&str == "$")//�ɹ�
				{
					cout << setw(20) << "Accepted..." << endl;
					out << setw(20) << "Accepted..." << endl;
					result = true;
					break;
				}
				else
				{
					cout << setw(20) << str + " Terminal" << endl;//ƥ��
					out << setw(20) << str + " Terminal" << endl;//ƥ��
					AnaStack.pop_back();
					StrStack.pop_back();
				}
			}
			else       //�滻����ʽ
			{
				int A = getCharIndex(an);
				int S = getCharIndex(str);
				if (PredictTable[A][S] != -1)
				{
					cout << setw(20) << get_Production(PredictTable[A][S]) << endl;//�����ǰջ�е�Ԫ��
					out << setw(20) << get_Production(PredictTable[A][S]) << endl;//�����ǰջ�е�Ԫ��
					vector<string> Pro = Production[PredictTable[A][S]];//��Ҫ�滻�Ĳ���ʽ
					AnaStack.pop_back();
					if (Pro[1] != "@")
					{
						for (int j = Pro.size() - 1; j > 0; j--)//������ջ
						{
							AnaStack.push_back(Pro[j]);
						}
					}
				}
				else
				{
					cout << setw(20) << " Error!!" << endl;//����
					out << setw(20) << " Error!!" << endl;//����
					result = false;
					break;
				}
			}

		}
		out.close();
		return result;
	}
};