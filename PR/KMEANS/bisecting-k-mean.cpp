#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define mp make_pair
#define clr(x) x.clear()
#define sz(x) ((int)(x).size())
#define F first
#define S second
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rep0(i,b) for(int i=0;i<b;i++)
#define rep1(i,b) for(int i=1;i<=b;i++)
#define repi(i,a,b) for(int i=a;i>=b;i--)
#define repx(i,a,b,dx) for(int i=a;i<b;i+=dx)
#define pdn(n) printf("%d\n",n)
#define sl(n) scanf("%lld",&n)
#define sd(n) scanf("%d",&n)
#define pn  printf("\n")

typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef vector<int> VI;
typedef vector<VI> VVI;
typedef long long i64;
typedef string st;
#define MOD 1000000007
#define MX 600
#define SUBMX 1000005

const int inf = 0x3f3f3f3f;
double data[MX][2],means[MX][2],means_temp[MX][2],dist_temp[MX];
int K,iter,cluster[MX],no_of_points[MX],selected;
set<int> myset;

void input(){
	ifstream in("bisecting.txt");
	rep0(i,MX)in>>data[i][0]>>data[i][1];
}

void rand_k(){
	srand(time(NULL));
	int r,used[MX];
	memset(&used,0,sizeof(used));
	//cout<<selected<<" asdsad"<<endl;
	int cc=0;
	rep0(j,MX){
		if(cluster[j]==selected)cc++;
	}
	//cout<<cc<<" 122333"<<endl;
	rep0(i,K){
		while(true){
			r=rand()%MX;
			if(used[r]==1||cluster[r]!=selected)continue;
			used[r]=1;
			break;
		}
		means[i][0]=data[r][0];means[i][1]=data[r][1];
	}
}

double distance(double a[],double b[]){
	return sqrt((a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1]));
}


int iteration(int itttt){
	memset(&means_temp,0,sizeof(means_temp));
	memset(&no_of_points,0,sizeof(no_of_points));
	rep0(nn,K){
		dist_temp[nn+itttt*K]=0;
	}
	myset.clear();
	int change=0;
	rep0(i,MX){
		if(cluster[i]>=itttt*K&&cluster[i]<itttt*K+K){}
		else if(cluster[i]!=selected){
			myset.insert(cluster[i]);
			continue;
		}
		int best=-1;
		double dist=inf,temp;
		rep0(j,K){
			temp=distance(data[i],means[j]);
			if(temp<dist){
				dist=temp;
				best=j;
			}
		}
		if(cluster[i]!=(best+itttt*K)){
			cluster[i]=best+itttt*K;
			change++;
		}
		dist_temp[cluster[i]]+=dist;
		myset.insert(cluster[i]);
		means_temp[best][0]+=data[i][0];
		means_temp[best][1]+=data[i][1];
		no_of_points[best]++;
	}
	rep0(i,K){
		means[i][0]=means_temp[i][0]/no_of_points[i];
		means[i][1]=means_temp[i][1]/no_of_points[i];
	}
	return change;
}


void next_selector(){
	double d=0;
	for (set<int>::iterator it=myset.begin(); it!=myset.end();  ++it)
	{
		if(d<dist_temp[*it]){
			d=dist_temp[*it];
			selected=*it;
		}
	}
}


int main(int argc, char const *argv[])
{
	//ios_base::sync_with_stdio(0);
	input();
	memset(&cluster,0,sizeof(cluster));
	cin>>K>>iter;
	rep0(i,iter){
		rand_k();
		while(iteration(i)){};
		next_selector();
	}
	cout<<iter*K<<endl;
	rep0(i,MX){
		cout<<data[i][0]<<" "<<data[i][1]<<" "<<cluster[i]<<endl;
	}
	return 0;
}