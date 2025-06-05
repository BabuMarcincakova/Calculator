#include "project.h"
#include <cerrno>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <stack>

Calculator::Calculator(std::string const &history) {
    historyFile = history;
    clearAll();
}

void Calculator::operator=(const std::string& expr) {
    expression = expr;
}

Valid Calculator::clearAll() {
    variables.clear();
    expressionList.clear();
    result.clear();
    expressionType = NONE;
    expression.clear();
    localVariables.clear();
    int r = remove(historyFile.c_str());
    if (r != 0) {
        return FAIL;
    }
    writtenInHistory = 0;
    return SUCCESS;
}

Valid const Calculator::writeHistory() {
    if (result.empty()) {
        return INVALID;
    }
    std::ofstream file(historyFile, std::ios::app);
    if (!file) {
        return FAIL;
    }
    writtenInHistory++;
    std::string line = getVariables() + "; ";
    std::string number = ":" + std::to_string(writtenInHistory) + "\n";

    file << number;

    if (line != "; ") {
        file << line;
    }
    file << result << "\n";

    file.close();
    return SUCCESS;
}

Valid Calculator::loadFromHistory(int const step) {
    if (writtenInHistory - step < 0 || writtenInHistory == 0) {
        return INVALID;
    }
    std::ifstream file(historyFile);
    if (!file) {
        return FAIL;
    }
    std::string line;
    std::string number = ":" + std::to_string(writtenInHistory - step);
    int get = 0;
    variables.clear();
    while (getline(file, line)) {
        if (line == number) {
            getline(file, line);
            expression = line;
            calculate();
            break;
        }
    }
    return VALID;
}

Valid Calculator::loadFromFile(std::string const &fileName) {
    std::ifstream file(fileName);
    if (!file) {
        return FAIL;
    }
    std::string line;
    std::string tempExpression = expression;
    std::map<char, int> tempVariables = variables;
    while (getline(file, line)) {
        expression = line;
        if (validExpression() == INVALID) {
            clearAll();
            expression = tempExpression;
            variables = tempVariables;
            calculate();
            return FAIL;
        }
        calculate();
    }
    return SUCCESS;
}

std::string const Calculator::getVariables() {
    std::string vars;
    for (auto iterator = variables.cbegin(); iterator != variables.cend(); ++iterator) {
        vars += std::string(1, iterator->first) + " = " + std::to_string(iterator->second) + "; ";
    }
    return vars.substr(0, vars.size() - 2);
}

void Calculator::removeVariable(char const var) {
    if (variables.find(var) != variables.end()) {
        variables.erase(var);
    }
}

void Calculator::removeVariable(std::vector<char> const var) {
    for (auto i = var.begin(); i != var.end(); ++i) {
        if (variables.find(*i) != variables.cend()) {
            variables.erase(*i);
        }
    }
}

bool const Calculator::oper(char const op) {
    if (op == signs[ADD][0] || op == signs[SUB][0] || op == signs[MUL][0] || op == signs[DIV][0] || op == signs[POW][0] || op == signs[MOD][0]) {
        return true;
    }
    return false;
}

bool inLocalVars(std::vector<char> const &localVariables, char const var) {
    for (int i = 0; i < localVariables.size(); i++) {
        if (localVariables[i] == var) {
            return true;
        }
    }
    return false;
}

Valid Calculator::validExpression(std::string const &expression) {
    expressionList.clear();
    std::vector<char> newVariables;
    int leftBracket = 0;
    int rightBracket = 0;
    int state = DEFAULT;  // 0 - variable, 1 - operator, 2 - number, 3 - left bracket, 4 - right bracket, 5 - equal
    int space = 0;
    expressionType = NONE;

    for (int i = 0; i < expression.size();  i++) {
        if (expression[i] == ' ') {
            space = 1;
        } else if (expression[i] == signs[EQ][0] && state == VAR) {
            state = EQUAL;
            space = 0;
            expressionType = ASSIGNMENT;
            expressionList.push_back(signs[EQ]);
        } else if (expression[i] == signs[LBR][0] && (state == DEFAULT || state == OPER || state == LEFTB)) {
            leftBracket++;
            state = LEFTB;
            space = 0;
            expressionList.push_back(signs[LBR]);
        } else if (expression[i] == signs[RBR][0] && (state == VAR || state == NUM || state == RIGHTB)) {
            rightBracket++;
            state = RIGHTB;
            space = 0;
            expressionList.push_back(signs[RBR]);
        } else if (oper(expression[i])
            && ((expression[i] == signs[SUB][0] && (state == DEFAULT || state == EQUAL || state == OPER || state == LEFTB)) || state == VAR || state == NUM || state == RIGHTB)) {
            state = OPER;
            space = 0;
            expressionList.push_back(std::string(1, expression[i]));
        } else if (expression[i] >= '0' && expression[i] <= '9'
            && (state == DEFAULT || state == OPER || state == LEFTB || state == NUM || state == EQUAL)
            && !(space == 0 && state == NUM)) {
            std::string number;
            while (expression[i] >= '0' && expression[i] <= '9' && i < expression.size()) {
                number += expression[i];
                i++;
            }
            i--;

            //subtraction is rewritten to addition of negative number
            if (!expressionList.empty() && expressionList.back() == signs[SUB]) {
                expressionList.pop_back();
                number = signs[SUB] + number;
                //do not add plus sign if negative number follows other operator, left bracket or is at the beginning of the expression
                if (expressionType != ASSIGNMENT && expressionList.back() != signs[LBR] && !expressionList.empty() && !(oper(expressionList.back()[0]) && expressionList.back().size() == 1)) {
                    expressionList.push_back(signs[ADD]);
                }
            }
            expressionList.push_back(number);
            state = NUM;
            space = 0;
        } else if (expression[i] >= 'a' && expression[i] <= 'z' && (state == DEFAULT || state == OPER || state == LEFTB || state == EQUAL)) {
            state = VAR;
            space = 0;
            std::string var;
            if (i + 2 < expression.size()) {
                for (int j = i; j < i+3; j++) {
                    var += expression[j];
                }
                if (var == signs[RES]) {
                    expressionList.push_back(signs[RES]);
                    i += 2;
                    continue;
                }
            }
            newVariables.push_back(expression[i]);
            expressionList.push_back(std::string(1, expression[i]));
        } else {
            expressionList.clear();
            expressionType = NONE;
            return INVALID;
        }
    }

    if (leftBracket != rightBracket) {
        expressionList.clear();
        expressionType = NONE;
        return INVALID;
    }

    if (state == OPER || state == LEFTB || state == EQUAL) {
        return INVALID;
    }

    if (expressionType != ASSIGNMENT) {
        for (int i = 0; i < newVariables.size(); i++) {
            if (variables.find(newVariables[i]) == variables.end() && !inLocalVars(localVariables, newVariables[i])) {
                expressionList.clear();
                expressionType = NONE;
                return INVALID;
            }
        }
    }

    if (expressionType != ASSIGNMENT) {
        expressionType = EXPRESSION;
    }

    for (int i = 0; i < newVariables.size(); i++) {
        localVariables.push_back(newVariables[i]);
    }

    return VALID;
}

Valid Calculator::validExpression() {
    std::istringstream input(this->expression);
    std::string expr;
    localVariables.clear();
    while (getline(input, expr, ';')) {
        if (validExpression(expr) == INVALID) {
            return INVALID;
        }
    }
    return VALID;
}

bool const Calculator::isNumber(std::string const &str) {
    int i = 0;
    if (str[0] == signs[SUB][0] && str.size() > 1) {
        i = 1;
    }

    for (; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }

    return true;
}

int const Calculator::priority(char const op) {
    if (op == signs[ADD][0] || op == signs[SUB][0]) {
        return 1;
    }
    if (op == signs[MUL][0] || op == signs[DIV][0] || op == signs[MOD][0]) {
        return 2;
    }
    if (op == signs[POW][0]) {
        return 3;
    }

    return 0;
}


std::vector<std::string> const Calculator::toPostfix(bool const variable) {
    std::vector<std::string> postfix;
    std::stack<char> operators;
    int in = 0;
    int i = 0;
    if (variable) { //if expression is assigned to variable, convert only expression part, not variable and '=' as well
        i = 2;
    }

    for (; i < expressionList.size(); i++) {
        if (expressionList[i][0] >= 'a' && expressionList[i][0] <= 'z') {
            postfix.push_back(expressionList[i]);
        } else if (isNumber(expressionList[i])) {
            postfix.push_back(expressionList[i]);
        } else if (expressionList[i][0] == signs[LBR][0]) {
            in++;
            operators.push(signs[LBR][0]);
        } else if (expressionList[i][0] == signs[RBR][0]) {
            while (!operators.empty() && operators.top() != signs[LBR][0]) {
                postfix.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) {
                operators.pop();
            }
            in--; 
        } else {
            while (!operators.empty() && priority(operators.top()) >= priority(expressionList[i][0]) && in == 0) {
                postfix.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(expressionList[i][0]);
        }
    }

    while (!operators.empty()) {
        postfix.push_back(std::string(1, operators.top()));
        operators.pop();
    }

    return postfix;
}

std::string const Calculator::operation(std::string const &a, std::string const &b, std::string const &op) {
    int x = stoi(a);
    int y = stoi(b);

    if (op[0] == signs[ADD][0]) {
        return std::to_string(x + y);
    }
    if (op[0] == signs[SUB][0]) {
        return std::to_string(x - y);
    }
    if (op[0] == signs[MUL][0]) {
        return std::to_string(x * y);
    }
    if (op[0] == signs[DIV][0]) {
        return std::to_string(x / y);
    }
    if (op[0] == signs[MOD][0]) {
        return std::to_string(x % y);
    }
    if (op[0] == signs[POW][0]) {
        int pow = x;
        for (int i = 1; i < y; i++) {
            x *= pow;
        }
        return std::to_string(x);
    }

    return EMPTY_STRING;
}

void Calculator::addVariable(char const variable, int const value) {
    if (variables.find(variable) == variables.end()) {
        variables.insert({variable, value});
    } else {
        variables.at(variable) = value;
    }
}

Valid const Calculator::calculation(std::string const &expr) {
    if (validExpression(expr) == INVALID) {
        return INVALID;
    }
    int listSize = expressionList.size();

    bool variableWithExpression = expressionType == ASSIGNMENT;

    std::vector<std::string> postfixExpression = toPostfix(variableWithExpression);
    std::stack<std::string> stack;

    for (int i = 0; i < postfixExpression.size(); i++) {
        if (postfixExpression[i][0] >= 'a' && postfixExpression[i][0] <= 'z') {
            if (postfixExpression[i] == signs[RES]) {
                if (result.empty()) {
                    return INVALID;
                }
                stack.push(result);
            } else {
                stack.push(std::to_string(variables.at(postfixExpression[i][0])));
            }
        } else if (isNumber(postfixExpression[i])) {
            stack.push(postfixExpression[i]);
        } else {
            if (stack.size() < 2) {
                if (stack.size() == 1 && isNumber(stack.top()) && postfixExpression[i] == signs[SUB]) {
                    std::string a = stack.top();
                    stack.pop();
                    stack.push(std::to_string(-1 * std::stoi(a)));
                    continue;
                }
                return INVALID;
            }
            std::string a = stack.top();
            stack.pop();
            std::string b = stack.top();
            stack.pop();
            if (postfixExpression[i][0] == signs[DIV][0] && a == "0") {
                return ZERO_DIVISION;
            }
            stack.push(operation(b, a, postfixExpression[i]));
        }
    }

    if (stack.empty()) {
        return INVALID;
    }

    result = stack.top();
    if (variableWithExpression) {
        addVariable(expressionList[0][0], std::stoi(result));
    }

    Valid write = writeHistory();
    if (write == FAIL) {
        return FAIL;
    }
    return VALID;
}

Valid const Calculator::calculate() {
    std::istringstream input(this->expression);
    std::string expr;
    while (getline(input, expr, ';')) {
        Valid result = calculation(expr);
        if (result != VALID) {
            return result;
        }
    }
    return VALID;
}
