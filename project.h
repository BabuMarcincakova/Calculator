#pragma once

#include <iostream>
#include <map>
#include <vector>

enum States { VAR, OPER, NUM, LEFTB, RIGHTB, EQUAL, DEFAULT = -1};
enum TypeOfExpression { ASSIGNMENT, EXPRESSION, NONE = -1};
enum Valid { INVALID, VALID, FAIL, SUCCESS, ZERO_DIVISION};
enum Operations { ADD, SUB, MUL, DIV, POW, MOD, LBR, RBR, EQ, RES};
const std::string signs[] = {"+", "-", "*", "/", "^", "%", "(", ")", "=", "res"};
const std::string EMPTY_STRING = "";

class Calculator {
    std::string expression;
    int expressionType = NONE;
    std::map<char, int> variables;
    std::vector<std::string> expressionList;
    std::string result;
    std::string historyFile;
    std::vector<char> localVariables;
    int writtenInHistory = 0;

    bool const oper(char const o);
    int const priority(char const op);
    void addVariable(char const variable, int const value);
    Valid const calculation(std::string const &expression);
public:
    Calculator(std::string const &history = "history.txt");
    void operator=(const std::string& expr);
    int const getResult() { return std::stoi(result); };
    Valid const writeHistory();
    Valid clearAll();
    std::string const getVariables();
    Valid validExpression(std::string const &expression);
    Valid validExpression();
    bool const isNumber(std::string const &str);
    std::vector<std::string> const toPostfix(bool const variable = false);
    std::string const operation(std::string const &a, std::string const &b, std::string const &op);
    Valid const calculate();
    void removeVariable(char const var);
    void removeVariable(std::vector<char> const var);
    Valid loadFromHistory(int const step);
    Valid loadFromFile(std::string const &fileName);
};

