#include "neuron.hpp"
#include<bits/stdc++.h>
using namespace std;

#define MX 30
#define MAX_ITER 100
#define LEARNING_RATE 0.5
#define INPUT_NO 300
#define ATTR 4


int hidden_layers,no_of_neurons[MX];
double input[INPUT_NO][ATTR];
neuron nn[MX][MX];
double error[MX][MX];


void get_input(char * st){
	ifstream in(st);
	double ff;
	in>>ff>>ff>>ff;
	for (int i = 0; i < INPUT_NO; ++i)
	{
		for (int j = 0; j < ATTR; ++j)
		{
			in>>input[i][j];
		}
	}
	in.close();
}

void rand_init(){
	//init all neurons with 
	for (int i = 0; i <= hidden_layers; ++i)
	{
		for (int j = 0; j < no_of_neurons[i]; ++j)
		{
			if(i==0)nn[i][j].init(3);
			else nn[i][j].init(no_of_neurons[i-1]);
		}
	}
}

void feedforward(int ind){
	double arr[2][MX];
	for (int i = 0; i <= hidden_layers; ++i)
	{
		for (int j = 0; j < no_of_neurons[i]; ++j)
		{
			if(i==0)nn[i][j].forward(input[ind]);
			else nn[i][j].forward(arr[i%2]);
			arr[(i+1)%2][j]=nn[i][j].out();
		}
	}
}

double get_target(int ind,int pos){
	return (static_cast<int>(input[ind][3]+.5)==pos+1)?1:0;
}


void backpropagation(int ind){
	for (int i = hidden_layers; i >=0; --i)
	{
		for (int j = 0; j < no_of_neurons[i]; ++j)
		{
			//calc error
			double res=nn[i][j].out();
			if(i==hidden_layers){
				double target=get_target(ind,j);
				error[i][j]=res*(1-res)*(target-res);
			}else{
				double mix=0;
				for (int k = 0; k < no_of_neurons[i+1]; ++k)
				{
					mix+=error[i+1][k]*nn[i+1][k].get_weight(j);
				}
				error[i][j]=res*(1-res)*mix;
			}
		}
	}
	for (int i = hidden_layers; i >=0; --i)
	{
		for (int j = 0; j < no_of_neurons[i]; ++j)
		{
			//update
			if(i==0){
				for (int k = 0; k < 3; ++k)
				{
					nn[i][j].add_weight(k,LEARNING_RATE*error[i][j]*input[ind][k]);
				}
				nn[i][j].add_weight(3,LEARNING_RATE*error[i][j]);

			}else{
				for (int k = 0; k < no_of_neurons[i-1]; ++k)
				{
					nn[i][j].add_weight(k,LEARNING_RATE*error[i][j]*nn[i-1][k].out());
				}
				nn[i][j].add_weight(no_of_neurons[i-1],LEARNING_RATE*error[i][j]);
			}
		}
	}

}

void train(){
	int l=0;
	while(true)
	{
		l++;
		double err=0;
		for (int j = 0; j < INPUT_NO; ++j)
		{
			feedforward(j);
			for (int k = 0; k < no_of_neurons[hidden_layers]; ++k){
				err+=pow(nn[hidden_layers][k].out()-get_target(j,k),2);
			}
			backpropagation(j);
		}
		cout<<err<<endl;
		if(err<1.00)break;
		if(l>10000)break;
		if(l%100==0)rand_init();
	}
}


void test(){
	int count=0;
	for (int i = 0; i < INPUT_NO; ++i)
	{
		feedforward(i);
		cout<<input[i][3];
		int pos=0;
		double mxx=0;
		for (int j = 0; j < 3; ++j)
		{
			cout<<" "<<nn[hidden_layers][j].out();
			if(nn[hidden_layers][j].out()>mxx){
				mxx=nn[hidden_layers][j].out();
				pos=j+1;
			}
		}
		if(pos==static_cast<int>(input[i][3]+.5)){}else count++;
		cout<<" "<<count<<endl;
	}
}




int main(){
	// input for hidden layers
	cin>>hidden_layers;
	for (int i = 0; i <hidden_layers; ++i)
	{
		cin>>no_of_neurons[i];
	}
	no_of_neurons[hidden_layers]=3;

	rand_init();

	//take input
	char *ch="Train.txt";
	get_input(ch);
	
	train();
	char *ss="Test.txt";
	get_input(ss);
	test();
	return 0;
}