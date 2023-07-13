#include "headers/operator.h"

Operator * Operator::Instance;

int main(){
    Operator * op = Operator::GetInstance("input.txt");
    op->PrintPrograms();
    op->Run();
}