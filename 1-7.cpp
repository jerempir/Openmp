
#include <omp.h>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void read(vector<vector<int>>& v, int n){
    ifstream file;
    file.open("vectors.txt");
    for(;file;){
        for(int o = 0;o<n;o++){
            string s;
            vector<int> vec;
            getline(file,s);
            if(!s.empty()){
                for(int i = 0;i<s.size();i++){
                    if(s[i] != ' ')
                        vec.push_back(s[i] - '0');
                }
                v[o] = vec;
            }
        }
    }
}

int reduc(vector<int> v1, vector<int> v2){
    int m = 0, sum = 0;
    for(int i = 0;i<v1.size();i++){
        m = v1[i] * v2[i];
    sum += m;
    }
    return sum;
}

int main(){
    omp_set_num_threads(2);
    int s = 0;
    int n = 2000000;
    int q = n-1;
    vector<vector<int>> v(n);
    vector<int> v1(q,0);
    double t1 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
                read(v,n);

            #pragma omp section
            {
                while(q > 0){
                #pragma omp parallel for reduction(+:s)
                for(int i = 1;i < n;i++){
                        if(v[i].size() == 5 & v[i-1].size() == 5 & v1[i] == 0){
                            v1[i] = 1;
                            s = s + reduc(v[i], v[i-1]);
                            #pragma omp atomic 
                                q--;
                        }
                    }
                }
            }
        }
    }
    cout<<s<<endl;
    double t2 = omp_get_wtime();
    cout<<t2-t1<<" time"<<endl;
    return 0;
}