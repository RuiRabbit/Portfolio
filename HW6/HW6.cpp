#include <iostream>
#include <ctype.h>
#include <string>
#include <string.h>
#include <queue>

#ifdef _WIN32
#include <conio.h>
#define CLEAR "cls"
#else //In any other OS
#include <termio.h>
#define CLEAR "clear"

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

int getch(void)
{
        struct termios tm, tm_old;
        int fd = STDIN_FILENO, c;
        if(tcgetattr(fd, &tm) < 0)
                return -1;
        tm_old = tm;
        cfmakeraw(&tm);
        if(tcsetattr(fd, TCSANOW, &tm) < 0)
                return -1;
        c = fgetc(stdin);
        if(tcsetattr(fd, TCSANOW, &tm_old) < 0)
                return -1;
        return c;
}
#endif

#define isOP(c) (c == '+' || c == '-' || c == '*' || c == '/')

enum CheckFlag{
    ILLEGAL_CHAR = 1,
    FIRST_OP = 2,
    LAST_OP = 4,
    RP_I = 8,
    OP_OP = 16,
    LP_OP = 32,
    I_LP = 64,
    RP_LP = 128,
    OP_RP = 256,
    LP_RP = 512,
    UNMATCHED_LP = 1024,
    UNMATCHED_RP = 2048,
    MORE_THAN_20 = 4096
};

using namespace std;

typedef struct Node Node;

struct OP{
    char c;
    int id;
    int priority;
    Node **lhs, **rhs;
    bool operator <(const OP &rhs) const{
        if(priority != rhs.priority){
            return priority < rhs.priority;
        }
        return id > rhs.id;
    }
};

struct Node{
    Node *parent = NULL;
    Node *lchild = NULL;
    Node *rchild = NULL;
    enum{
        TYPE_NUM,
        TYPE_OP
    } DataType;
    union{
        int num;
        char c;
    };
};

int ExpressionCheck(string &str);

void PrintErrorMessage(int ErrorCode);

void Build(string &str, int &now, Node *&root);

Node* MergeBranch(OP op);

void MakeLeaf(string &str, int &now, Node *&whence);

int getNumFromStr(string &str, int &now);

void PrintPosfix(Node *root);

void ResetTree(Node *root);

double Calculate(Node *root);

int main(){
    Node *root = NULL;
    while(1){
        string str = "";
        bool finish = false;
        while(str == ""){
            str = "";
            cout<<"Please enter an infix expression and press enter : ";
            char c;
            while((c = getch()) != EOF && c != '\n' && c != '\r'){
                if(c == 27){
                    finish = true;
                    if(root)
                    ResetTree(root);
                    break;
                }
                if(c == '\b' || c == 127){
                    if(str.size() > 0){
                        cout<<"\b \b";
                        str.pop_back();
                    }
                }
                else{
                    cout<<c;
                    str += c;
                }
            }
            cout<<endl;
            if(finish)
                break;
        }
        if(finish)
            break;
        int ErrorCode = 0;
        ErrorCode = ExpressionCheck(str);
        if(ErrorCode){
            PrintErrorMessage(ErrorCode);
            cout<<endl;
            continue;
        }
        if(root)
            ResetTree(root);
        int now = 0;

        Build(str, now, root);

        cout<<"The postfix expression : ";
        PrintPosfix(root);
        cout<<endl;
        printf("= %.2lf\n\n", Calculate(root));
    }
}

int ExpressionCheck(string &str){
    int ErrorCode = 0;
    if(str.size() > 20){
        ErrorCode |= MORE_THAN_20;
    }
    if(strspn(str.c_str(), "1234567890()+-*/") != str.size()){
        ErrorCode |= ILLEGAL_CHAR;
    }
    if(isOP(str[0])){
        ErrorCode |= FIRST_OP;
    }
    if(isOP(str[str.size() - 1])){
        ErrorCode |= LAST_OP;
    }
    int p = 0;
    for(int i = 0; i < str.size() - 1; i++){
        if(!(ErrorCode & RP_I) && str[i] == ')' && isdigit(str[i + 1]))
            ErrorCode |= RP_I;

        if(!(ErrorCode & OP_OP) && isOP(str[i]) && isOP(str[i + 1]))
            ErrorCode |= OP_OP;

        if(!(ErrorCode & LP_OP) && str[i] == '(' && isOP(str[i + 1]) && str[i + 1] != '-')
            ErrorCode |= LP_OP;

        if(!(ErrorCode & I_LP) && isdigit(str[i]) && str[i + 1] == '(')
            ErrorCode |= I_LP;

        if(!(ErrorCode & RP_LP) && str[i] == ')' && str[i + 1] == '(')
            ErrorCode |= RP_LP;

        if(!(ErrorCode & OP_RP) && isOP(str[i]) && str[i + 1] == ')')
            ErrorCode |= OP_RP;

        if(!(ErrorCode & LP_RP) && str[i] == '(' && str[i + 1] == ')')
            ErrorCode |= LP_RP;

        if(str[i] == '(')
            p++;
        else if(str[i] == ')'){
            if(--p < 0){
                p = 0;
                ErrorCode |= UNMATCHED_RP;
            }
        }
    }
    if(str[str.size() - 1] == '(')
        p++;
    else if(str[str.size() - 1] == ')'){
        if(--p < 0){
            p = 0;
            ErrorCode |= UNMATCHED_RP;
        }
    }
    if(p > 0)
        ErrorCode |= UNMATCHED_LP;
    return ErrorCode;
}

void PrintErrorMessage(int ErrorCode){
    if(ErrorCode & MORE_THAN_20)
        cout<<"Error - line contains more characters than allowed."<<endl;

    if(ErrorCode & ILLEGAL_CHAR)
        cout<<"Illegal character."<<endl;

    if(ErrorCode & FIRST_OP)
        cout<<"First character an operator."<<endl;
        
    if(ErrorCode & LAST_OP)
        cout<<"Last character an operator."<<endl;

    if(ErrorCode & RP_I)
        cout<<"Right parenthesis followed by an identifier."<<endl;

    if(ErrorCode & OP_OP)
        cout<<"Operator followed by an operator."<<endl;
        
    if(ErrorCode & LP_OP)
        cout<<"Left parenthesis followed by an operator."<<endl;

    if(ErrorCode & I_LP)
        cout<<"Identifier followed by a left parenthesis."<<endl;

    if(ErrorCode & RP_LP)
        cout<<"Right parenthesis followed by a left parenthesis."<<endl;

    if(ErrorCode & OP_RP)
        cout<<"Operator followed by a right parenthesis."<<endl;

    if(ErrorCode & LP_RP)
        cout<<"Left parenthesis followed by a right parenthesis."<<endl;

    if(ErrorCode & UNMATCHED_LP)
        cout<<"Unmatched left parenthesis."<<endl;

    if(ErrorCode & UNMATCHED_RP)
        cout<<"Unmatched right parenthesis."<<endl;

    return;
}

void Build(string &str, int &now, Node *&root){
    priority_queue <OP> pq;
    vector <Node*> operand(20);
    int priority = 0;
    OP tmp;
    while(now < str.size()){
        if(str[now] == '(' && str[now + 1] != '-'){
            priority += 2;
            now++;
        }
        else if(str[now] == ')'){
            priority -= 2;
            now++;
        }
        else if(isOP(str[now])){
            tmp.c = str[now];
            tmp.id = pq.size();
            tmp.priority = priority;
            if(str[now] == '*' || str[now] == '/')
                tmp.priority++;
            tmp.lhs = &operand[tmp.id];
            tmp.rhs = &operand[tmp.id + 1];
            pq.push(tmp);
            now++;
        }
        else{
            MakeLeaf(str, now, operand[pq.size()]); 
        }
    }
    if(pq.empty()){
        root = operand.front();
    }
    else{
        while(!pq.empty()){
            OP op = pq.top();
            pq.pop();
            root = MergeBranch(op);
        }
    }
}

Node* MergeBranch(OP op){
    Node *root = new Node;
    root->DataType = Node::TYPE_OP;
    root->c = op.c;
    Node *lhs = *op.lhs, *rhs = *op.rhs;

    while(lhs->parent)
        lhs = lhs->parent;
    while(rhs->parent)
        rhs = rhs->parent;
    root->lchild = lhs;
    lhs->parent = root;
    root->rchild = rhs;
    rhs->parent = root;
    return root;
}

void MakeLeaf(string &str, int &now, Node *&whence){
    whence = new Node;
    whence->DataType = Node::TYPE_NUM;
    if(str[now] == '('){
        now += 2;
        whence->num = (-1) * getNumFromStr(str, now);
        now++;
    }
    else{
        whence->num = getNumFromStr(str, now);
    }
    return;
}

int getNumFromStr(string &str, int &now){
    int ret = 0;
    while(now < str.size() && isdigit(str[now])){
        ret = ret * 10 + (str[now] - '0');
        now++;
    }
    return ret;
}

void PrintPosfix(Node *root){
    if(root->lchild)
        PrintPosfix(root->lchild);
    if(root->rchild)
        PrintPosfix(root->rchild);
    if(root->DataType == Node::TYPE_NUM){
        if(root->num < 0){
            cout<<'('<<root->num<<')';
        }
        else{
            cout<<root->num;
        }
    }
    else{
        cout<<root->c;
    }
    // cout<<" ";
    return;
}

void ResetTree(Node *root){
    if(root->lchild)
        ResetTree(root->lchild);
    if(root->rchild)
        ResetTree(root->rchild);
    free(root);
    root = NULL;
    return;
}

double Calculate(Node *root){
    if(root->DataType == Node::TYPE_NUM){
        return (double)root->num;
    }
    if(root->c == '+'){
        return Calculate(root->lchild) + Calculate(root->rchild);
    }
    else if(root->c == '-'){
        return Calculate(root->lchild) - Calculate(root->rchild);
    }
    else if(root->c == '*'){
        return Calculate(root->lchild) * Calculate(root->rchild);
    }
    else if(root->c == '/'){
        return Calculate(root->lchild) / Calculate(root->rchild);
    }
    return 0;
}

