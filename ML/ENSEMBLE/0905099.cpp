#include <bits/stdc++.h>
#include <time.h>
using namespace std;

#define TOTAL 683
#define TRAINING_NO 520
#define TESTING_NO 163
#define ATTR 10
#define CHILDREN 10
#define POSITIVE 0
#define NEGATIVE 1
#define TP 0
#define TN 1
#define FP 2
#define FN 3



int training[TRAINING_NO][ATTR],testing[TESTING_NO][ATTR];
int original_training[TRAINING_NO][ATTR],original_testing[TESTING_NO][ATTR];

double weights[TRAINING_NO];
int ok[TRAINING_NO];

struct node
{
	node * childs[CHILDREN];
	int label=-1;
	int attr=-1;
};


void input(){
	ifstream in("data.csv");
	int i,j,temp,pos1,pos2;
	char cc;
	srand(time(NULL));
	//training input
	for (i = 0; i < TRAINING_NO; ++i)
	{
		for (j = 0; j < ATTR; ++j)
		{
			in>>training[i][j];
			original_training[i][j]=training[i][j];
			if(j<ATTR-1)in>>cc;
		}
	}
	//testing input
	for (i = 0; i < TESTING_NO; ++i)
	{
		for (j = 0; j < ATTR; ++j)
		{
			in>>testing[i][j];
			original_testing[i][j]=testing[i][j];
			if(j<ATTR-1)in>>cc;
		}
	}
	in.close();
	//return;
	//randomization by swapping all test value
	for (i = 0; i < TESTING_NO; ++i)
	{
		pos1=rand()%TESTING_NO;
		pos2=rand()%TRAINING_NO;
		for (j = 0; j < ATTR; ++j)
		{
			temp=testing[pos1][j];
			testing[pos1][j]=training[pos2][j];
			original_testing[pos1][j]=training[pos2][j];
			training[pos2][j]=temp;
			original_training[pos2][j]=temp;
		}
	}
	
}

int no_of_example(int constraints[]){
	int cnt=0,i,j;
	bool ok;
	for (i = 0; i < TRAINING_NO; ++i)
	{
		ok=true;
		for (j = 0; j < ATTR; ++j)
		{
			if(constraints[j]!=-1 && training[i][j]!=constraints[j]){
				ok=false;
				break;
			}
		}
		if(ok)cnt++;
	}
	return cnt;
}

bool check_all(int i,int constraints[]){
	int count_all=no_of_example(constraints);
	constraints[9]=i;
	int count_now=no_of_example(constraints);
	constraints[9]=-1;
	return count_now==count_all;
}

bool check_attr(int attrs[]){
	for (int i = 0; i < ATTR-1; ++i)
	{
		if(attrs[i])return false;
	}
	return true;
}

int most_common(int constraints[]){
	constraints[9]=POSITIVE;
	int count_pos=no_of_example(constraints);
	constraints[9]=NEGATIVE;
	int count_neg=no_of_example(constraints);
	constraints[9]=-1;
	return ((count_pos<count_neg)?NEGATIVE:POSITIVE);
}


int count_this(int pos,int i,int constraints[]){
	constraints[pos]=i;
	int count_now=no_of_example(constraints);
	constraints[pos]=-1;
	return count_now;
}

double entropy(int p1,int p2){
	double tot=p1+p2;
	double res=(double)p1/tot;
	if(p1)res*=(-log2(res));
	double temp=(double)p2/tot;
	if(p2)temp*=(-log2(temp));
	return res+temp;
}

double entropy_sv(int i,int j,int constraints[]){
	constraints[i]=j;
	int p1,p2;
	p1=count_this(9,POSITIVE,constraints);
	p2=count_this(9,NEGATIVE,constraints);
	double ent=entropy(p1,p2);
	constraints[i]=-1;
	return ent;
}

int find_best(int constraints[],int attrs[]){
	double mx_val=-1,ent;
	int best=-1,propval,p1,p2,tot;
	for (int i = 0; i < ATTR-1; ++i)
	{
		if(attrs[i]){
			p1=count_this(9,POSITIVE,constraints);
			p2=count_this(9,NEGATIVE,constraints);
			tot=p1+p2;
			ent=entropy(p1,p2);
			for (int j = 1; j < 11; ++j)
			{
				propval=count_this(i,j,constraints);
				if(propval)ent-=((double)propval/tot)*entropy_sv(i,j,constraints);
			}
			if(ent>mx_val){
				mx_val=ent;
				best=i;
			}
		}
	}
	return best;
}


bool check_empty(int constraints[]){
	return no_of_example(constraints)==0;
}

node * ID3(int constraints[],int attrs[]){
	node * nn=new node();
	if(check_all(POSITIVE,constraints)){
		nn->label=POSITIVE;
		return nn;
	}
	if(check_all(NEGATIVE,constraints)){
		nn->label=NEGATIVE;
		return nn;
	}
	if(check_attr(attrs)){
		nn->label=most_common(constraints);
		return nn;
	}
	int best_attr=find_best(constraints,attrs);
	nn->attr=best_attr;
	for (int i = 0; i < ATTR-1; ++i)
	{
		attrs[i]=0;
	}
	for (int i = 1; i < 11; ++i)
	{
		if(check_empty(constraints)){
			node * mew=new node();
			mew->label=most_common(constraints);
			nn->childs[i-1]=mew;
		}
		else {
			constraints[best_attr]=i;
			nn->childs[i-1]=ID3(constraints,attrs);
			constraints[best_attr]=-1;
		}
	}
	return nn;
}

void walk(node * n,int i,int cnt[]){
	if(n->label!=-1){
		if(n->label!=testing[i][9])cnt[n->label+2]++;
		else cnt[n->label]++;
	}else{
		walk(n->childs[testing[i][n->attr]-1],i,cnt);
	}

}


void test(node * n){
	int cnt[]={0,0,0,0};
	for (int i = 0; i < TESTING_NO; ++i)
	{
		walk(n,i,cnt);
	}
	cout<<cnt[TP]<<" "<<cnt[TN]<<" "<<cnt[FP]<<" "<<cnt[FN]<<" "<<endl;
}

void walk2(node * n,int i,double& cnt,double pr[]){
	if(n->label!=-1){
		if(n->label!=original_training[i][9]){
			cnt+=pr[i];
			ok[i]=1;
		}else ok[i]=0;
	}else{
		walk2(n->childs[original_training[i][n->attr]-1],i,cnt,pr);
	}

}

double test_traindata(node * n,double pr[]){
	double cnt=0;
	for (int i = 0; i < TRAINING_NO; ++i)
	{
		walk2(n,i,cnt,pr);
	}
	return cnt;
}


double sum_weights;

void random_resample(double pr[]){
	for (int i = 0; i < TRAINING_NO; ++i)
	{
		double rr1=(double)(rand()%TRAINING_NO)/TRAINING_NO;
		int rr=0;
		for (int j = 0; j < TRAINING_NO; ++j)
		{
			rr1-=pr[j];
			if(rr1<0.0){
				rr=j;
				break;
			}
		}
		//cout<<rr<<endl;
		for (int j = 0; j < ATTR; ++j)
		{
			training[i][j]=original_training[rr][j];
		}
	}
}



void walk3(node * n,int i,int& cnt){
	if(n->label!=-1){
		cnt=n->label;
	}else{
		walk3(n->childs[testing[i][n->attr]-1],i,cnt);
	}

}

node * arr[TESTING_NO];
double beta[TESTING_NO];


void wrapper(int& r){
	
	double pr[TRAINING_NO];
	for (int i = 0; i < TRAINING_NO; ++i)
	{
		weights[i]=1.0/TRAINING_NO;
	}
	for (int i = 0; i < r; ++i)
	{
		int cons[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
		int attrs[]={1,1,1,1,1,1,1,1,1};
		sum_weights=0.0;
		for (int j = 0; j < TRAINING_NO; ++j)
		{
			sum_weights+=weights[j];
		}
		for (int j = 0; j < TRAINING_NO; ++j)
		{
			pr[j]=weights[j]/sum_weights;
		}
		random_resample(pr);
		arr[i]=ID3(cons,attrs);
		double ee=test_traindata(arr[i],pr);
		if(ee>.5){
			r=i;
			break;
		}
		beta[i]=ee/(1-ee);
		for (int j = 0; j < TRAINING_NO; ++j)
		{
			weights[j]*=pow(beta[i],1-ok[j]);
		}

	}

	cout<<r<<endl;
}


void test_this(int r){

	int thik=0;
	for (int i = 0; i < TESTING_NO; ++i)
	{
		int out;
		double oo[2]={0,0};
		int kk=0;
		for (int j = 0; j < r; ++j)
		{
			walk3(arr[j],i,out);
			oo[out]+=log(1.0/beta[j]);
		}
		if(oo[0]>oo[1]){
			if(testing[i][9]==0)thik++;
		}else{
			if(testing[i][9]==1)thik++;
		}
		
	}
	
	cout<<(double)thik/TESTING_NO<<endl;
	

}

void all_test(int r){
	int thik[10]={0};
	for (int i = 0; i < TESTING_NO; ++i)
	{
		int out;
		double oo[2]={0,0};
		int kk=0;
		for (int j = 0; j < r; ++j)
		{
			walk3(arr[j],i,out);
			oo[out]+=log(1.0/beta[j]);
			if(j%5==0){
				if(oo[0]>oo[1]){
					if(testing[i][9]==0)thik[kk]++;
				}else{
					if(testing[i][9]==1)thik[kk]++;
				}
				kk++;

			}
		}
		
	}
	for (int i = 0; i < 8; ++i)
	{
		cout<<i*5<<" "<<(double)thik[i]/TESTING_NO<<endl;
	}
}


void base(){
	int cons[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int attrs[]={1,1,1,1,1,1,1,1,1};
	node* temp=ID3(cons,attrs);
	int thik=0,out;
	for (int i = 0; i < TESTING_NO; ++i)
	{
		walk3(temp,i,out);
		if(testing[i][9]==out)thik++;
	}
	cout<<(double)thik/TESTING_NO<<endl;
}


int main(int argc, char const *argv[])
{
	input();
	int r=50;

	//base();
	cin>>r;
	wrapper(r);
	//all_test(r);
	test_this(r);

	
	return 0;
}
