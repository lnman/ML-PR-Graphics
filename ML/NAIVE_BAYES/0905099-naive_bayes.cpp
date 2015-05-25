#include <bits/stdc++.h>
using namespace std;

#define MX 140

unordered_map<string,int> gl_mm,pos,topics,com,ttest;
int NOD;
vector<string> vvs={"a","an","the","am","is","are","was","were","has","have","had","shall","will","lt","gt","be","of","in","to","by","from","and","or","nor","either","neither","though","although","through","over","under","this","its","for","as","may","might","on","but","with","which","who","whom","where","when","how","do","did","done","that","these","those","always","can","could","after","before","however","against","his"};
struct doc
{
	unordered_map<string, int> mm;
	int sum=0;
}d[MX];

bool common(string & temp){
	return com.find(temp)!=com.end();
}

string nextline(ifstream & in){
	string temp="";
	while(!in.eof()&&getline(in,temp))if(temp!="")break;
	return temp;
}

void tokenize_and_insert(string & str,unordered_map<string, int> & where,int & sum,int global_insert=0){
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	char * writable = new char[str.size()+1];
	char tokens[]="0123456789&(){}[] -,;.!'\"";
	copy(str.begin(), str.end(), writable);
	writable[str.size()] = '\0';
	char * pch= strtok(writable,tokens);//tokenize
	while (pch != NULL)
	{
		string insert(pch);
		if(!common(insert)){
			where[insert]++;sum++;
			if(global_insert)gl_mm[insert]=1;
		}
		pch=strtok(NULL,tokens);
	}
	delete[] writable;
}

void train(){
	ifstream intop("topics.data");
	string temp,top;
	int skip=0,curr=0;
	NOD=0;
	while(getline(intop,temp)){
		if(temp=="")break;
		topics[temp]=0;
	}
	intop.close();
	ifstream in("training.data");
	while(true){
		if(skip==0)top=nextline(in);
		else top=temp;
		if(top=="")break;
		NOD++;
		topics[top]++;
		if(topics[top]==1){
			pos[top]=curr;
			curr++;
		}
		temp=nextline(in);//title
		tokenize_and_insert(temp,d[pos[top]].mm,d[pos[top]].sum,1);
		temp=nextline(in);//ignore date
		skip=0;
		if(topics.find(temp)!=topics.end()){ //no data
			skip=1;continue;
		}
		temp=nextline(in);//first line
		if(topics.find(temp)!=topics.end()){ //no data
			skip=1;continue;
		}
		do{
			if(temp=="")break;
			tokenize_and_insert(temp,d[pos[top]].mm,d[pos[top]].sum,1);
		} while (getline(in,temp));
	}
	in.close();
	cout<<NOD<<endl;
}

void test(){
	ifstream in("test.data");
	string temp,top,res;
	int skip=0,ac=0,tot=0,ppos,ssum;
	double prob,prob1,upper,lower,kk=.047;;
	unordered_map<string,int>::iterator it,it1;
	while(true){
		if(skip==0)top=nextline(in);
		else top=temp;
		if(top=="")break;
		tot++;ssum=0;
		ttest.clear();//clear
		temp=nextline(in);//title
		tokenize_and_insert(temp,ttest,ssum);
		temp=nextline(in);//ignore date
		skip=0;
		if(topics.find(temp)!=topics.end())skip=1;
		else{
			temp=nextline(in);//first line
			if(topics.find(temp)!=topics.end())skip=1;
		}
		if(skip==0){
			do{
				if (temp=="")break;
				tokenize_and_insert(temp,ttest,ssum);
			} while (getline(in,temp));
		}
		prob=0;
		for (it=topics.begin(); it!=topics.end(); ++it)
		{
			prob1=it->second;ppos=pos[it->first];
			lower=kk*gl_mm.size()+d[ppos].sum;
			for(it1=ttest.begin();it1!=ttest.end();++it1){
				upper=kk;
				if(d[ppos].mm.find(it1->first)!=d[ppos].mm.end()){
					upper+=d[ppos].mm[it1->first],it1->second;
				}
				prob1*=upper/lower;
			}
			if(prob1>prob){
				prob=prob1;
				res=it->first;
			}
		}
		//cout<<"Original topic:"<<top<<" found: "<<res<<endl;//break;
		if(top==res)ac++;
	}
	in.close();
	cout<<tot<<" "<<ac<<" "<<double(ac)/tot<<endl;
}

int main(int argc, char const *argv[]){
	for(auto str:vvs){
		com[str]=0;
	}
	train();
	test();
	return 0;
}
