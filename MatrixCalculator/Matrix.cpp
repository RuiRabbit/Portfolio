#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <iomanip>

#ifdef _WIN32
#define CLEAR "cls"
#else //In any other OS
#define CLEAR "clear"
#endif

using namespace std;

struct Matrix{
    int m, n;
    vector <array<int, 3>> v; // x, y, value

    void Init(int a, int b){
        m = a;
        n = b;
        if(!v.empty())
            v.clear();
    }

    void Iden(int a){
        m = a;
        n = a;
        if(!v.empty())
            v.clear();
        for(int i = 0; i < m; i++){
            v.push_back({i, i, 1});
        }
    }

    void FullInput(){
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                int value;
                cin>>value;
                if(value != 0){
                    v.push_back({i, j, value});
                }
            }
        }
    }

    void Print(){
        int index = 0;
        vector <int> size(n, 0);
        for(int i = 0; i < v.size(); i++){
            if(log10(v[i][2]) + 1 > size[v[i][1]])
                size[v[i][1]] = log10(v[i][2]) + 1;
        }
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                if(j != 0)
                    cout<<" ";
                if(index < v.size() && v[index][0] == i && v[index][1] == j){
                    cout<<setw(size[v[index][1]])<<v[index][2];
                    index++;
                }
                else{
                    cout<<0;
                }
            }
            cout<<endl;
        }
    }

    Matrix Submatrix(vector <int> row, vector <int> column) const{
        Matrix ret;
        ret.Init(row.size(), column.size());
        if(row.empty() || column.empty())
            return ret;
        for(int i = 0; i < v.size(); i++){
            vector <int>::iterator Rit = lower_bound(row.begin(), row.end(), v[i][0]);
            vector <int>::iterator Cit = lower_bound(column.begin(), column.end(), v[i][1]);
            if(*Rit == v[i][0] && *Cit == v[i][1]){
                ret.v.push_back({(int)(Rit - row.begin()), (int)(Cit - column.begin()), v[i][2]});
            }
        }
        return ret;
    }

    Matrix Transpose() const{
        Matrix ret;
        ret.Init(n, m);
        ret.v.resize(v.size());
        vector <int> starting_pos(n + 1);
        for(int i = 0; i < v.size(); i++){
            starting_pos[v[i][1] + 1]++;
        }
        for(int i = 1; i < n; i++){
            starting_pos[i] += starting_pos[i - 1];
        }
        for(int i = 0; i < v.size(); i++){
            ret.v[starting_pos[v[i][1]]] = {v[i][1], v[i][0], v[i][2]};
            starting_pos[v[i][1]]++;
        }
        return ret;
    }

    Matrix operator + (const Matrix &rhs) const{
        Matrix tmp;
        tmp.Init(m, n);
        int l = 0, r = 0;
        while(l < v.size() && r < rhs.v.size()){
            if(v[l][0] == rhs.v[r][0] && v[l][1] == rhs.v[r][1]){
                if(v[l][2] + rhs.v[r][2] != 0)
                    tmp.v.push_back({v[l][0], v[l][1], v[l][2] + rhs.v[r][2]});
                l++;
                r++;
            }
            else if(v[l] < rhs.v[r]){
                tmp.v.push_back(v[l]);
                l++;
            }
            else{
                tmp.v.push_back(rhs.v[r]);
                r++;
            }
        }
        while(l < v.size()){
            tmp.v.push_back(v[l]);
            l++;
        }
        while(r < rhs.v.size()){
            tmp.v.push_back(rhs.v[r]);
            r++;
        }
        return tmp;
    }

    Matrix operator * (const Matrix &rhs) const{
        Matrix mult = rhs.Transpose();
        Matrix ret;
        ret.Init(m, rhs.n);

        int row = 1;
        int column = 1;

        vector <int> Rpos(m + 1, 0);
        for(int i = 1; i < v.size(); i++){
            if(Rpos[row] == 0 && v[i][0] == row){
                Rpos[row] = i;
                row++;
            }
            if(row == m){
                break;
            }
        }
        Rpos[m] = v.size();
        row = 0;

        vector <int> Cpos(mult.m + 1, 0);
        for(int i = 1; i < mult.v.size(); i++){
            if(Cpos[column] == 0 && mult.v[i][0] == column){
                Cpos[column] = i;
                column++;
            }
            if(column  == mult.m){
                break;
            }
        }
        Cpos[m] = mult.v.size();
        column = 0;

        for(int i = 0; i < m; i++){
            for(int j = 0; j < mult.m; j++){
                int value = 0;
                for(int k = Rpos[i]; k < Rpos[i + 1]; k++){
                    for(int l = Cpos[j]; l < Cpos[j + 1]; l++){
                        if(v[k][1] == mult.v[l][1]){
                            value += v[k][2] * mult.v[l][2];
                        }
                    }
                }
                ret.v.push_back({i, j, value});
            }
        }

        return ret;
    }

    Matrix operator ^ (const int e) const{
        Matrix ret;
        ret.Iden(m);
        Matrix mult = (*this);
        int exp = e;
        
        while(exp){
            if(exp % 2){
                ret = ret * mult;
            }
            mult = mult * mult;
            exp /= 2;
        }

        return ret;
    }
};

map <string, Matrix *> m;

void CLS(){
    system(CLEAR);
    return;
}

void CreatePhase(){
    CLS();
    cout<<"Please enter the name of your new Matrix : ";
    string name;
    cin>>name;
    if(m.find(name) != m.end()){
        cout<<"The name has been used before. Do you want to recreate it? (0 = No, 1 = Yes) : ";
        int choice;
        cin>>choice;
        if(!choice){
			return;
        }
    }
    else{
        m[name] = new Matrix;
    }

    cout<<"Max row : ";
    cin>>m[name]->m;
    cout<<"Max column : ";
    cin>>m[name]->n;

	m[name]->Init(m[name]->m, m[name]->n);

    cout<<"Please enter the value of the whole Matrix : "<<endl;
    m[name]->FullInput();
    return;
}

Matrix* MatrixPickingList(string str){
    CLS();
    cout<<"Here is the list of all your Matrix"<<endl<<endl;
    for(map<string, Matrix *>::iterator it = m.begin(); it != m.end(); it++){
        cout<<"- "<<it->first<<endl;
    }
    cout<<endl;
    cout<<str;
    string name;
    cin>>name;
    if(m.find(name) == m.end()){
        return MatrixPickingList(str);
    }

    CLS();
    cout<<name<<" :"<<endl;
    m[name]->Print();
    return m[name];
}

void PauseAndReturnMainPage(){
    cout<<"Press Enter to return to the main page.";
    string s;
    cin.ignore();
    getline(cin, s);
    return;
}

void ViewPhase(){
    MatrixPickingList("Please enter the name of the Matrix you want to view : ");
    PauseAndReturnMainPage();
    return;
}

void SavingMatrix(Matrix matrix){
    cout<<"Do you want to save the result matrix? (0 = No, 1 = Yes) : ";
	int choice;
	cin>>choice;
	if(choice){
		cout<<"Please enter the result matrix name : ";
        string name;
		cin>>name;
		if(m.find(name) != m.end()){
			cout<<"You're going to overwrite an old matrix. Are you sure about that? (0 = No, 1 = Yes) : ";
			cin>>choice;
			if(choice){
				*m[name] = matrix;
			}
		}
		else{
			m[name] = new Matrix;
			*m[name] = matrix;
		}
	}
}

void GetSubmatrixPhase(){
    Matrix *NowMatrix = MatrixPickingList("Please enter the name of the Matrix you want to get submatrix : ");

    cout<<"Please enter the row numbers of your submatrix (seperated with spaces) :"<<endl;
    string s;
    cin.ignore();
    getline(cin, s);
    stringstream ss;
    ss.str("");
    ss.clear();
    ss<<s;
    vector <int> row;
    while(true){
        int tmp;
        ss>>tmp;
        if(ss.fail())
            break;
        row.push_back(tmp);
    }
    sort(row.begin(), row.end());

    cout<<"Please enter the column numbers of your submatrix (seperated with spaces) :"<<endl;
    ss.str("");
    ss.clear();

    getline(cin, s);
    ss<<s;
    vector <int> column;
    while(true){
        int tmp;
        ss>>tmp;
        if(ss.fail())
            break;
        column.push_back(tmp);
    }
    sort(column.begin(), column.end());

    cout<<endl;
    NowMatrix->Submatrix(row, column).Print();
    SavingMatrix(NowMatrix->Submatrix(row, column));
    PauseAndReturnMainPage();
}

void TransposePhase(){
    Matrix *NowMatrix = MatrixPickingList("Please enter the name of the Matrix you want to get submatrix : ");
    cout<<endl;
    cout<<"Transpose Ver."<<endl;
    NowMatrix->Transpose().Print();
	SavingMatrix(NowMatrix->Transpose());
    PauseAndReturnMainPage();
}

int FormulaCalculate(Matrix &base, stringstream &ss){
    while(true){
        char c;
        string name;
        int e;
        ss>>c;
        if(ss.fail())
            break;

        switch (c){
            case '+':
                ss>>name;
                if(m.find(name) == m.end()){
                    cout<<"The Matrix \""<<name<<"\" is not existed."<<endl;
                    return -1;
                }
                else if(base.m != m[name]->m || base.n != m[name]->n){
                    cout<<"The Matrix sizes doesn't match."<<endl;
                    return -1;
                }
                base = base + *m[name];
                break;

            case '*':
                ss>>name;
                if(m.find(name) == m.end()){
                    cout<<"The Matrix \""<<name<<"\" is not existed."<<endl;
                    return -1;
                }
                else if(base.n != m[name]->m){
                    cout<<"The Matrix sizes doesn't match."<<endl;
                    return -1;
                }
                base = base * *m[name];
                break;
            
            case '^':
                if((ss>>e).fail()){
                    cout<<"Integer exponent needed."<<endl;
                    return -1;
                }
                // ss>>e;
                base = base ^ e;
                break;

            default:
                return -1;
                break;
        }
    }
    return 0;
}

void CalculatePhase(){
    CLS();
    cout<<"Please enter your formula(seperated by space) :"<<endl;
    cout<<"(Support +, *, ^)"<<endl;
    stringstream ss;
    string s;
    cin.ignore();
    getline(cin, s);
    ss<<s;
    string name;
    ss>>name;
    Matrix base;
    if(m.find(name) == m.end()){
        cout<<"The Matrix \""<<name<<"\" is not existed."<<endl;
        cout<<"It's an invalid expression, please try again."<<endl;
        cin.putback('\n');
        PauseAndReturnMainPage();
        return;
    }
    base = *m[name];

    if(FormulaCalculate(base, ss) == - 1){
        cout<<"It's an invalid expression, please try again."<<endl;
    }
    else{
        base.Print();
        SavingMatrix(base);
		cin.ignore();
    }
    cin.putback('\n');
    PauseAndReturnMainPage();
    return;
}

void MainPhase(){
    CLS();
    cout<<"Hello User, what do you want to do today?"<<endl;
    cout<<"1. Create a new Matrix."<<endl;
    cout<<"2. View an existed Matrix."<<endl;
    cout<<"3. Get the submatrix of your Matrix."<<endl;
    cout<<"4. Transpose your Matrix."<<endl;
    cout<<"5. Start calculating."<<endl;
    cout<<"6. Exit."<<endl;
    int choice;
    cin>>choice;
    if(cin.fail()){
        cin.clear();
        cin.ignore(10000, '\n');
        cin.putback('\n');
        cout<<"Wrong input."<<endl;
        PauseAndReturnMainPage();
        MainPhase();
        return;
    }
    if(m.empty() && choice != 6 && choice != 1){
        choice = 922;
    }
    switch(choice){
        case 922:
            CLS();
            cout<<"You should create a matrix first."<<endl;
            PauseAndReturnMainPage();
            break;
        case 1:
            CreatePhase();
            break;
        case 2:
            ViewPhase();
            break;
        case 3:
            GetSubmatrixPhase();
            break;
        case 4:
            TransposePhase();
            break;
        case 5:
            CalculatePhase();
            break;
        case 6:
            return;
            break;
        default:
            break;
    }
    MainPhase();
}

int main(){

    MainPhase();

}