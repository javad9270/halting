#include "headers/operator.h"
#include <fstream>
#include<sstream>
#include <string>
#include <cstring>

Program::Program(vector<Operation> ops, unsigned int R0){
    operations = ops;
    input = R0;
    last_call_input = 0;
    current_runner = 0;
    no_answer = false;
    out = "*";
    this->CheckHasAnswer();
    
}

void Runner::RemoveFinishedRunners(){
    vector<Runner *> temp = this->program->finishedRunners;
    int count = 0;
    for(auto item:temp){
        if(item != this){
            this->program->finishedRunners.erase(this->program->finishedRunners.begin() + count);
            delete item;
        }
        count++;
    }
}

void Program::CheckHasAnswer(){
    vector<int> call_indices;
    vector<int> return_indices;
    int count = 0;
    for(auto op:operations){
        if(op.type == "CALL"){
            call_indices.push_back(count);
        }
        count++;
    }
    count = 0;
    for(auto op:operations){
        if(op.type == "RET"){
            return_indices.push_back(count);
        }
        count++;
    }
}

Runner::Runner(unsigned int input, unsigned int output, Program * prg){
    Rvalues[0] = input;
    Rvalues[9] = output;
    current_operation = 0;
    program = prg;
    out = "*";
    finished = false;
}

void Runner::Run(){
    //cout << "running runner!" << current_operation << "\n";
    while(!finished){
        //cout << current_operation <<  " " << this->program->is_call_count << " " << Rvalues[9] << "\n";
        this->RunSingleOperation();
    }
    if(!this->program->no_answer){
        out = to_string(Rvalues[9]);
    }
    this->RemoveFinishedRunners();
}

void Runner::ReturnToRunner(unsigned int output){
    this->Rvalues[9] = output;
    // cout << "return to runner ====>" << this->Rvalues[9] << " " << this->program->runInstances[this->program->current_runner]->Rvalues[9] << " " 
    // << this->program->current_runner << "\n";
}

void Program::Run(){
    if(this->no_answer){
        return;
    }
    Runner * runInstance = new Runner(input, 0, this);
    runInstances.push_back(runInstance);
    runInstance->Run();
    out = runInstances[0]->out;
}

void Runner::PrintOperands(){
    Operation operation = program->operations[this->current_operation];
    for(auto operand: operation.operands){
        if(operand.type == INPUT){
            cout << "R" << operand.value << " = " << this->Rvalues[operand.value] << "\n";
        }
        else{
            cout << "const " << operand.value << "\n";
        }
    }
    cout << this->program->current_runner << "\n";
}

void Runner::RunSingleOperation(){
    Operation operation = program->operations[this->current_operation];
    //cout << operation.type <<  " ,current runner===>" << program->current_runner << "\n";
    //this->PrintOperands();
    if(operation.type == "RET"){
        this->RunRET();
    }
    else if(operation.type == "CALL"){
        //cout << "call--->" << this->program->is_call_count << " \n";
        this->RunCALL();
    }
    else if(operation.type == "MOV"){
        this->RunMOV();
    }
    else if(operation.type == "ADD"){
        this->RunADD();
    }
    else if(operation.type == "SUB"){
        this->RunSUB();
    }
    else if(operation.type == "MUL"){
        this->RunMUL();
    }
    else if(operation.type == "DIV"){
        this->RunDIV();
    }
    else if(operation.type == "MOD"){
        this->RunMOD();
    }
    else if(operation.type == "IFEQ"){
        this->RunIFEQ();
    }
    else if(operation.type == "IFNEQ"){
        this->RunIFNEQ();
    }
    else if(operation.type == "IFG"){
        this->RunIFG();
    }
    else if(operation.type == "IFGE"){
        this->RunIFGE();
    }
    else if(operation.type == "IFL"){
        this->RunIFL();
    }
    else if(operation.type == "IFLE"){
        this->RunIFLE();
    }
    else{
        this->current_operation++;
    }
    if(this->current_operation == this->program->operations.size()){
        this->finished = true;
    }

    return;
}

void Runner::RunMOV(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(operand1.type == INPUT && operand2.type == INPUT){
        this->Rvalues[operand1.value] = this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        this->Rvalues[operand1.value] = operand2.value;
    }
    this->current_operation++;
    return;
}

void Runner::RunADD(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(operand1.type == INPUT && operand2.type == INPUT){
        this->Rvalues[operand1.value] += this->Rvalues[operand2.value];
        //this->Rvalues[operand1.value] %= 1000;
    }
    else if(operand1.type == INPUT){
        this->Rvalues[operand1.value] += operand2.value;
        //this->Rvalues[operand1.value] %= 1000;
    }
    this->current_operation++;
    return;
}

void Runner::RunSUB(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(operand1.type == INPUT && operand2.type == INPUT){
        if(this->Rvalues[operand1.value] >= this->Rvalues[operand2.value]){
            this->Rvalues[operand1.value] -= this->Rvalues[operand2.value];
        }
        else{
            this->Rvalues[operand1.value] = 1000 - this->Rvalues[operand2.value];
        }
        //this->Rvalues[operand1.value] %= 1000;
    }
    else if(operand1.type == INPUT){
        if(this->Rvalues[operand1.value] >= operand2.value){
            this->Rvalues[operand1.value] -= operand2.value;
        }
        else{
            this->Rvalues[operand1.value] = 1000 - operand2.value;
        }    
        //this->Rvalues[operand1.value] %= 1000;
    }
    this->current_operation++;
    return;
}

void Runner::RunMUL(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(operand1.type == INPUT && operand2.type == INPUT){
        this->Rvalues[operand1.value] *= this->Rvalues[operand2.value];
        //this->Rvalues[operand1.value] %= 1000;
    }
    else if(operand1.type == INPUT){
        this->Rvalues[operand1.value] *= operand2.value;
        //this->Rvalues[operand1.value] %= 1000;
    }
    this->current_operation++;
    return;
}

void Runner::RunDIV(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(this->Rvalues[operand1.value] > 0){
        if(operand1.type == INPUT && operand2.type == INPUT){
            this->Rvalues[operand1.value] /= this->Rvalues[operand2.value];
            //this->Rvalues[operand1.value] %= 1000;
        }
        else if(operand1.type == INPUT){
            this->Rvalues[operand1.value] /= operand2.value;
            //this->Rvalues[operand1.value] %= 1000;
        }
    }
    this->current_operation++;
    return;
}

void Runner::RunMOD(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    if(operand1.type == INPUT && operand2.type == INPUT){
        this->Rvalues[operand1.value] %= this->Rvalues[operand2.value];
        //this->Rvalues[operand1.value] %= 1000;
    }
    else if(operand1.type == INPUT){
        this->Rvalues[operand1.value] %= operand2.value;
        //this->Rvalues[operand1.value] %= 1000;
    }
    this->current_operation++;
    return;
}

void Runner::RunIFEQ(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = (this->Rvalues[operand1.value] == this->Rvalues[operand2.value]);
    }
    else if(operand1.type == INPUT){
        output = (this->Rvalues[operand1.value] == operand2.value);
    }
    if(output == true){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;
}

void Runner::RunIFNEQ(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = this->Rvalues[operand1.value]  != this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        output = this->Rvalues[operand1.value] != operand2.value;
    }
    if(output){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;
}

void Runner::RunIFG(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = this->Rvalues[operand1.value]  > this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        output = this->Rvalues[operand1.value] > operand2.value;
    }
    if(output){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;   
}

void Runner::RunIFL(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = this->Rvalues[operand1.value] < this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        output = this->Rvalues[operand1.value] < operand2.value;
    }
    if(output){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;
}

void Runner::RunIFGE(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = this->Rvalues[operand1.value] >= this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        output = this->Rvalues[operand1.value] >= operand2.value;
    }
    if(output){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;
}

void Runner::RunIFLE(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    Operand operand2 = operation.operands[1];
    bool output = false;
    if(operand1.type == INPUT && operand2.type == INPUT){
        output = this->Rvalues[operand1.value] <= this->Rvalues[operand2.value];
    }
    else if(operand1.type == INPUT){
        output = this->Rvalues[operand1.value] <= operand2.value;
    }
    if(output){
        this->current_operation++;
    }
    else{
        for(int i = this->current_operation; i < this->program->operations.size(); i++){
            const char * temp = this->program->operations[i].type.c_str();
            if(strcmp(temp, "ENDIF") == 0){
                this->current_operation = i + 1;
                break;
            }
        }
    }
    return;
}

void Runner::RunCALL(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    unsigned value;
    if(operand1.type == INPUT){
        value = this->Rvalues[operand1.value];
    }
    else{
        value = operand1.value;
    }
    if(program->current_runner > 1000){
        this->program->no_answer = true;
        this->finished = true;
    }
    else{
        this->program->last_call_input = value;
        Runner * runnerInstance = new Runner(value, 0, this->program);
        this->program->runInstances.push_back(runnerInstance);
        this->program->current_runner++;
        this->current_operation++;
        runnerInstance->Run();
    }

    return;
}

void Runner::RunRET(){
    Operation operation = program->operations[this->current_operation];
    Operand operand1 = operation.operands[0];
    if(operand1.type == INPUT){
        Rvalues[9] = Rvalues[operand1.value]%1000;
    }
    else{
        Rvalues[9] = operand1.value%1000;
    }
    if(this->program->current_runner > 0){
        this->program->current_runner--;
        this->program->runInstances.pop_back();
        this->program->runInstances[this->program->current_runner]->ReturnToRunner(Rvalues[9]);
        this->program->finishedRunners.push_back(this);
    }
    this->finished = true;
    return;

}

void Operator::PrintPrograms(){
    for(vector<Program *>::iterator it = this->programs.begin(); it != this->programs.end(); it++){
        cout << "Input:  " << (*it)->input << "\n";
        // for(Operation op:(*it)->operations){
        //     cout << "operation type " << op.type << " operands: ";
        //     for(Operand operand: op.operands){
        //         cout << "(type :" << operand.type << " ," << "value: " << operand.value << ") ";
        //     }
        //     cout << "\n";
        // }
        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
    }
    return;
}

Operator * Operator::GetInstance(string path){
    if(!Instance){
        Instance = new Operator(path);
    }
    return Instance;
}

Operation Operator::GenerateOperation(string line){
    istringstream eachLine;
    eachLine.str(line);
    Operation out;
    int count2 = 0;
    while(getline(eachLine, line, ' ')){
        if(count2 == 0){
            out.type = line;
        }
        else{
            istringstream operands;
            operands.str(line);
            while(getline(operands, line, ',')){
                char * operand = new char[line.length()];
                strcpy(operand, line.c_str());
                if(operand[0] == 'R'){
                    Operand item;
                    item.value = int(operand[1]) - 48;
                    item.type = INPUT;
                    out.operands.push_back(item);
                }
                else{
                    Operand item;
                    item.type = CONSTANT;
                    item.value = stoi(line);
                    out.operands.push_back(item);
                }
            }
        }
        count2++;
    }
    return out;
}

Operator::Operator(string path){
    ifstream input;
    input.open(path);
    string line;
    int lineCount, inputValue, count = 0;
    vector<Operation> operations;
    while(getline(input, line)){
        istringstream eachLine;
        eachLine.str(line);
        if(count == 0){
            int count2 = 0;
            while(getline(eachLine, line, ' ')){
                switch(count2){
                    case 0:
                        lineCount = stoi(line);
                        break;
                    case 1:
                        inputValue = stoi(line);
                        break;
                    default:
                        break;
                }
                count2++;
            }
        }
        else if(count < lineCount + 1){
            Operation op = this->GenerateOperation(line);
            operations.push_back(op);
        }
        count++;
        if(count == lineCount + 1){
            count = 0;
            if(lineCount > 0){
                Program * prg = new Program(operations, inputValue);
                this->programs.push_back(prg);
            }
            operations.clear();
            lineCount = 0;
            inputValue = 0;
        }
    }
    input.close();
    return;
}

void Operator::Run(){
    ofstream output;
    output.open("output.txt");
    for(auto prg:this->programs){
        prg->Run();
        cout << "output===> " << prg->out << "\n";
        output << prg->out << "\n";
    }
    output.close();
    return;
}