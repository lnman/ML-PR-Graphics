#include <bits/stdc++.h>
#include <thread>
#include <chrono>
using namespace std;

#define MX 6600

unordered_map<string,int> gl_mm,topics,com,ttest;
unordered_map<string,double>ttest2;
vector<thread> vthread;
vector<pair<double,int>> vp;
int NOD,no_thread,ssum,acc[9],times,qsum;
double dsum;
vector<string> vvs={"a","an","the","am","is","are","was","were","has","have","had","shall","will","lt","gt","be","of","in","to","by","from","and","or","nor","either","neither","though","although","through","over","under","this","its","for","as","may","might","on","but","with","which","who","whom","where","when","how","do","did","done","that","these","those","always","can","could","after","before","however","against","his"};
struct doc
{
	unordered_map<string, int> mm;
	unordered_map<string, double> mm1;
	int sum;
	double dsum;
	int qsum;
	string topic;
}d[MX];

double dist_all[MX][3];

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
			if(global_insert==1 && where[insert]==1)gl_mm[insert]++;
		}
		pch=strtok(NULL,tokens);
	}
	delete[] writable;
}

void calc_cos(unordered_map<string, double>& mm1,unordered_map<string, int> & mm,int & sum,double & dsum,int & qsum){
	unordered_map<string, int>::iterator it;
	double tf,idf;
	dsum=0;
	qsum=0;
	for (it=mm.begin(); it!=mm.end(); ++it)
	{
		tf=(double)it->second/sum;
		if(gl_mm.find(it->first)!=gl_mm.end())idf=log((double)NOD/gl_mm[it->first]);
		else idf=log((double)NOD);
		mm1[it->first]=tf*idf;
		dsum+=mm1[it->first]*mm1[it->first];
		qsum+=it->second*it->second;
	}
	dsum=sqrt(dsum);
}

void train(){
	ifstream intop("topics.data");
	string temp,top;
	int skip=0;
	NOD=-1;
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
		d[NOD].topic=top;
		temp=nextline(in);//title
		tokenize_and_insert(temp,d[NOD].mm,d[NOD].sum,1);
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
			tokenize_and_insert(temp,d[NOD].mm,d[NOD].sum,1);
		} while (getline(in,temp));
	}
	in.close();
	NOD++;
	for (int i = 0; i < NOD; ++i)
	{
		calc_cos(d[i].mm1,d[i].mm,d[i].sum,d[i].dsum,d[i].qsum);
	}
	cout<<NOD<<endl;
}


//type 1 hamming,type 2 euclidean,type 3 tf-idf
void distance(unordered_map<string, int> & m1,unordered_map<string, int> & m2,double dist[],unordered_map<string, double> & mm1,unordered_map<string, double> & mm2,double d1,int qsum1,int qsum2){
	dist[0]=0,dist[1]=0,dist[2]=0;
	int ccm=0,ll=qsum1+qsum2;
	unordered_map<string, int>::iterator it;
	if(m1.size()<=m2.size()){
		for (it=m1.begin(); it!=m1.end(); ++it)
		{
			if(m2.find(it->first)!=m2.end()){
				ccm++;
				ll-=2*it->second*m2[it->first];
				dist[2]+=mm1[it->first]*mm2[it->first];
			}
		}
	}else{
		for (it=m2.begin(); it!=m2.end(); ++it)
		{
			if(m1.find(it->first)!=m1.end()){
				ccm++;
				ll-=2*it->second*m1[it->first];
				dist[2]+=mm1[it->first]*mm2[it->first];
			}
		}
		
	}
	dist[0]=m1.size()+m2.size()-2*ccm;
	dist[1]=ll;
	dist[2]/=d1;
}

void wrapper(int j){
	times=6517/no_thread;
	j*=times;
	for (int i = j; i<j+times; ++i)
	{
		distance(d[i].mm,ttest,dist_all[i],d[i].mm1,ttest2,d[i].dsum,d[i].qsum,qsum);
	}
}

bool comp1(const std::pair<double,int>& firstElem, const std::pair<double,int>& secondElem) {
  return firstElem.first < secondElem.first;
}

bool comp2(const std::pair<double,int>& firstElem, const std::pair<double,int>& secondElem) {
  return firstElem.first > secondElem.first;
}


string majority(int k,int start){
	unordered_map<string,int> res;
	string tt;
	int not_done=0;
	for (int i = start; i < start+5; ++i)
	{
		res[d[vp[i].second].topic]++;
		if(res[d[vp[i].second].topic]==2 && not_done==0){
			tt=d[vp[i].second].topic;
			not_done=1;
		}
	}
	unordered_map<string, int>::iterator it;
	for (it=res.begin(); it!=res.end(); ++it)
	{
		if(it->second>k/2){
			return it->first;
		}
	}
	if(k==3)return d[vp[start].second].topic;
	if(not_done){
		return tt;
	}
	return d[vp[start].second].topic;
}


void find_result(string ans){
	double di[3][5]={1e9,1e9,1e9,1e9,1e9,1e9,1e9,1e9,1e9,1e9,0,0,0,0,0};
	vp.clear();
	for (int i = 0; i < 15; ++i)
	{
		vp.push_back(make_pair(di[0][i],0));
	}
	int how_many=no_thread*times;
	for (int i = 0; i < how_many; ++i)
	{
		if(dist_all[i][0]<vp[4].first){
			vp[4]=make_pair(dist_all[i][0],i);
		}
		if(dist_all[i][1]<vp[9].first){
			vp[9]=make_pair(dist_all[i][1],i);
		}
		if(dist_all[i][2]>vp[14].first){
			vp[14]=make_pair(dist_all[i][2],i);
		}
		sort(&vp[0],&vp[5],comp1);
		sort(&vp[5],&vp[10],comp1);
		sort(&vp[10],&vp[15],comp2);
	}
	//k==1
	for (int i = 0; i < 3; ++i)
	{
		if(d[vp[i*5].second].topic==ans)acc[i]++;
		if(majority(3,i*5)==ans)acc[3+i]++;
		if(majority(5,i*5)==ans)acc[6+i]++;
	}
}

void test(){
	ifstream in("test.data");
	string temp,top;
	int skip=0,tot=0;
	while(true){
		if(skip==0)top=nextline(in);
		else top=temp;
		if(top=="")break;
		tot++;ssum=0;dsum=0;
		ttest.clear();ttest2.clear();;//clear
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
		calc_cos(ttest2,ttest,ssum,dsum,qsum);
		vthread.clear();
		for (int i = 0; i < no_thread; ++i)
		{
			vthread.push_back(thread(&wrapper,i));
		}
		for (int i = 0; i < no_thread; ++i)
		{
			vthread[i].join();
		}
		find_result(top);
	}
	in.close();
	printf("K    Hamming   Euclidean  Cosine\n");
	for (int i = 0; i < 3; ++i)
	{
		printf("%d    %.3f    %.3f     %.3f\n",2*i+1,double(acc[i*3+0]*100)/tot,double(acc[i*3+1]*100)/tot,double(acc[i*3+2]*100)/tot );
	}
}

int main(int argc, char const *argv[]){
	auto start = chrono::high_resolution_clock::now();
	for(auto str:vvs){
		com[str]=0;
	}
	train();
	no_thread=4;
	test();
	auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end-start;
    cout<<"Time spent : "<<elapsed.count()/1000<<" s\n";
	return 0;
}
