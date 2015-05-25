#ifndef __NEURON__
#define __NEURON__

#include <bits/stdc++.h>
#include <ctime>

#define MX 30

class neuron
{
	double weights[MX];
	int no_of_weight;
	double output;
	void randomize(){
		srand(time(NULL));
		for (int i = 0; i <= no_of_weight; ++i)
		{
			weights[i]=(double)((rand()%201)-100)/100;
		}
	}
	double sigmoid(double x){
		return (1.0/(1.0+exp(-x)));
	}
public:
	neuron(){}
	void init(int w){
		no_of_weight=w;//bias
		randomize();
	}
	~neuron(){}
	void forward(double arr[]);
	double out(){
		return output;
	}
	void add_weight(int ind,double value){
		weights[ind]+=value;
	}
	double get_weight(int ind){
		return weights[ind];
	}
};

inline void neuron::forward(double arr[]){
	double sum=0;
	for (int i = 0; i < no_of_weight; ++i)
	{
		sum+=weights[i]*arr[i];
	}
	sum+=weights[no_of_weight];
	output=sigmoid(sum);
}

#endif