#include <bits/stdc++.h>

using namespace std;

int fp,fn,tp,tn;
int input[4];
int main(int argc, char const *argv[])
{
	ifstream in("out.txt");

	for (int i = 0; i < 100; ++i)
	{
		in>>input[0]>>input[1]>>input[2]>>input[3];
		tp+=input[0];
		tn+=input[1];
		fp+=input[2];
		fn+=input[3];
	}
	int sum=tp+tn+fp+fn;
	double accuracy=(double)(tp+tn)/sum;
	double precision=(double)(tp)/(tp+fp);
	double recall=(double)(tp)/(tp+fn);
	double f_measure=2*precision*recall/(precision+recall);
	double g_mean=sqrt(precision*recall);
	cout<<"Accuracy: "<<accuracy<<endl;
	cout<<"Precision: "<<precision<<endl;
	cout<<"Recall: "<<recall<<endl;
	cout<<"F-measure: "<<f_measure<<endl;
	cout<<"G-mean: "<<g_mean<<endl;
	return 0;
}