#include <iostream>
#include <vector>
using namespace std;

enum OperandType{
    CONSTANT,
    INPUT
};

struct Operand{
    OperandType type;
    unsigned int value;
};

struct Operation{
    string type;
    vector<Operand> operands;
};

class Runner;

class Program{
    public:
        unsigned int input;
        vector<Operation> operations;
        vector<Runner*> runInstances;
        vector<Runner*> finishedRunners;
        Program(vector<Operation> operations, unsigned int R0);
        void Run();
        unsigned int current_runner;
        unsigned int last_call_input;
        bool no_answer;
        string out;
    private:
        void CheckHasAnswer();
};

class Runner{
    public:
        Runner(unsigned int input, unsigned int output, Program * prg);
        void ReturnToRunner(unsigned int output);
        void Run();
        string out;
        bool finished;
    private:
        void RunSingleOperation();
        void RunMOV();
        void RunADD();
        void RunSUB();
        void RunMUL();
        void RunDIV();
        void RunMOD();
        void RunIFEQ();
        void RunIFNEQ();
        void RunIFG();
        void RunIFL();
        void RunIFGE();
        void RunIFLE();
        void RunCALL();
        void DetermineNextStep();
        void RunRET();
        void RemoveFinishedRunners();

        void PrintOperands();
        unsigned int current_operation;
        Program * program;
        vector<unsigned int> Rvalues{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
};



class Operator{
    public:
        static Operator * GetInstance(string path);
        void PrintPrograms();
        void Run();

    private:
        Operator(string path);
        Operation GenerateOperation(string line);

        vector<Program *> programs;
        static Operator * Instance;
};

