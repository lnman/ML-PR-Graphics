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
double data[MX][2],means[MX][2],means_temp[MX][2];;
int K,cluster[MX],no_of_points[MX];


void input(){
	ifstream in("bisecting.txt");
	rep0(i,MX)in>>data[i][0]>>data[i][1];
}

void rand_k(){
	cin>>K;
	srand(time(NULL));
	int r,used[MX];
	memset(&used,0,sizeof(used));
	rep0(i,K){
		while(true){
			r=rand()%MX;
			if(used[r]==1)continue;
			used[r]=1;
			break;
		}
		means[i][0]=data[r][0];means[i][1]=data[r][1];
	}
}

double distance(double a[],double b[]){
	return sqrt((a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1]));
}


int iteration(){
	int change=0;
	memset(&means_temp,0,sizeof(means_temp));
	memset(&no_of_points,0,sizeof(no_of_points));
	rep0(i,MX){
		int best=-1;
		double dist=inf,temp;
		rep0(j,K){
			temp=distance(data[i],means[j]);
			if(temp<dist){
				dist=temp;
				best=j;
			}
		}
		if(best!=cluster[i]){
			cluster[i]=best;
			change++;
		}
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



int main(int argc, char const *argv[])
{
	//ios_base::sync_with_stdio(0);
	input();
	rand_k();
	memset(&cluster,-1,sizeof(cluster));
	while(iteration()){}
	cout<<K<<endl;
	rep0(i,MX){
		cout<<data[i][0]<<" "<<data[i][1]<<" "<<cluster[i]<<endl;
	}
	return 0;
}