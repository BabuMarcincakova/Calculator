// Copyright 2005, Google Inc.
// All rights reserved.

#include <iostream>
#include <fstream>
#include "gtest/gtest.h"

#include "project.h"

using namespace ::testing;

TEST(Validation, ValidExpression) {
    Calculator calc;
    calc = "(3 + 5)*2";
    ASSERT_EQ(VALID, calc.validExpression());
}

TEST(Validation, ValidExpression2) {
    Calculator calc;
    calc = "x=3";
    ASSERT_EQ(VALID, calc.calculate());
    calc = "x + 7";
    ASSERT_EQ(VALID, calc.validExpression());
}

TEST(Validation, InvalidExpression) {
    Calculator calc;
    calc = "(3 + 5) * 2)";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, InvalidExpression2) {
    Calculator calc;
    calc = "(3 + 5) * 2 +";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, InvalidExpression3) {
    Calculator calc;
    calc = "x = 5 = 3";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, InvalidExpression4) {
    Calculator calc;
    calc = "x =";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, InvalidMultiExpression) {
    Calculator calc;
    calc = "x = 5, 4+ 3";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, UndefinedVariable) {
    Calculator calc;
    calc = "x + 3";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, UndefinedVariable2) {
    Calculator calc;
    calc = "x = 5";
    calc.calculate();
    calc = "x + y";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, WrongVariableFormat) {
    Calculator calc;
    calc = "var = 5";
    ASSERT_EQ(INVALID, calc.validExpression());
    calc = "5 + var";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Validation, InvalidAssignment) {
    Calculator calc;
    calc = "x = / 4 +2";
    ASSERT_EQ(INVALID, calc.validExpression());
}

TEST(Variables, AssignValue) {
    Calculator calc;
    calc = "x   =5";
    ASSERT_EQ(VALID, calc.validExpression());
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ("x = 5", calc.getVariables());
}

TEST(Variables, AssignMultipleValues) {
    Calculator calc;
    calc = "x=5; y=3";
    ASSERT_EQ(VALID, calc.validExpression());
    calc.calculate();
    ASSERT_EQ("x = 5; y = 3", calc.getVariables());
}

TEST(AdvancedVariables, AssignExpression) {
    Calculator calc;
    calc = "x= 5*3";
    ASSERT_EQ(VALID, calc.validExpression());
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(15, calc.getResult());
    ASSERT_EQ("x = 15", calc.getVariables());
}

TEST(Variables, RemoveVariable) {
    Calculator calc;
    calc = "x = 5+2";
    calc.calculate();
    ASSERT_EQ("x = 7", calc.getVariables());
    calc.removeVariable('x');
    ASSERT_EQ("", calc.getVariables());
}

TEST(Variables, RemoveVariables) {
    Calculator calc;
    calc = "x =5; y=3;z = 2";
    calc.calculate();
    ASSERT_EQ("x = 5; y = 3; z = 2", calc.getVariables());
    std::vector<char> vars = {'x', 'a', 'y'};
    calc.removeVariable(vars);
    ASSERT_EQ("z = 2", calc.getVariables());
}

TEST(Testing, ClearAll) {
    Calculator calc("testHistory.txt");
    calc = "x = 5; y=6";
    calc.calculate();
    ASSERT_EQ("x = 5; y = 6", calc.getVariables());
    ASSERT_EQ(SUCCESS, calc.clearAll());
    ASSERT_EQ("", calc.getVariables());
    ASSERT_EQ(INVALID, calc.loadFromHistory(0));
}

TEST(Testing, PostfixCheck) {
    Calculator calc;
    calc = "  (3+15  )* 2";
    ASSERT_EQ(VALID, calc.validExpression());
    std::vector<std::string> postfix = calc.toPostfix();
    std::vector<std::string> expected = {"3", "15", "+", "2", "*"};
    ASSERT_EQ(expected, postfix);
}

TEST(Testing, PostfixCheck2) {
    Calculator calc;
    calc = "x =4";
    calc.calculate();
    calc = "  (3+x  )* 2";
    ASSERT_EQ(VALID, calc.validExpression());
    std::vector<std::string> postfix = calc.toPostfix();
    std::vector<std::string> expected = {"3", "x", "+", "2", "*"};
    ASSERT_EQ(expected, postfix);
}

TEST(Testing, MultiInput) {
    Calculator calc;
    calc = "x = 5; x + 3";
    ASSERT_EQ(VALID, calc.validExpression());
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(8, calc.getResult());
}

TEST(Testing, MultiInput2) {
    Calculator calc;
    calc = "x = 5; y = 3; x + y;-(-6 +3) * (-2); res +7";
    ASSERT_EQ(VALID, calc.validExpression());
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(1, calc.getResult());
}

TEST(Files, LoadFromHistory) {
    Calculator calc("historyFile.txt");
    calc = "-(-6-2) * 2; res + 1";
    calc.calculate();
    calc = "x = 5";
    calc.calculate();
    ASSERT_EQ(VALID, calc.loadFromHistory(2));
    ASSERT_EQ(16, calc.getResult());
    ASSERT_EQ("", calc.getVariables());
}

TEST(Files, LoadHistoryFail) {
    Calculator calc;
    calc = "x = 5; y = 3; x + y;-(-6 +3) * (-2); res +7";
    calc.calculate();
    ASSERT_EQ(VALID, calc.loadFromHistory(2));
    ASSERT_EQ(8, calc.getResult());
    ASSERT_EQ("x = 5; y = 3", calc.getVariables());
    calc.clearAll();
    ASSERT_EQ(INVALID, calc.loadFromHistory(2));
}

TEST(Files, LoadFromFile) {
    remove("load.txt");
    std::ofstream file("load.txt");
    file << "x = 5; y = 3; x + y;-(-6 +3) * (-2); res +7";
    file.close();

    Calculator calc;
    ASSERT_EQ(SUCCESS, calc.loadFromFile("load.txt"));
    ASSERT_EQ(1, calc.getResult());
}

TEST(Files, LoadFromFile2) {
    remove("load.txt");
    std::ofstream file("load.txt");
    file << "(5-4-2) * 12\n";
    file << "x = 5\n";
    file << "res *2";
    file.close();

    Calculator calc;
    ASSERT_EQ(SUCCESS, calc.loadFromFile("load.txt"));
    ASSERT_EQ(10, calc.getResult());
}

TEST(Results, Evaluation) {
    Calculator calc;
    calc = "(5-4) *12";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(12, calc.getResult());
}

TEST(Results, Evaluation2) {
    Calculator calc;
    calc = "-(2 +1) % 2";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-1, calc.getResult());
}

TEST(Results, PriorityCheck) {
    Calculator calc;
    calc = "11 % 3 ^ 2";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(2, calc.getResult());
}

TEST(Results, MultipleBrackets) {
    Calculator calc;
    calc = "((2 + 1) ^ 3)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(27, calc.getResult());
}

TEST(Results, MultipleBrackets2) {
    Calculator calc;
    calc = "((5 /4) *12) + 3";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(15, calc.getResult());
}

TEST(Results, Negative) {
    Calculator calc;
    calc = "-5";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-5, calc.getResult());
}

TEST(Variables, AssignNegative) {
    Calculator calc;
    calc = "x=-3";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-3, calc.getResult());
}

TEST(Results, DoubleNegative) {
    Calculator calc;
    calc = "-(-5)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(5, calc.getResult());
}

TEST(Results, NegativeBrackets) {
    Calculator calc;
    calc = "-(5 + 3)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-8, calc.getResult());
}

TEST(Results, NegativeBrackets2) {
    Calculator calc;
    calc = "-(-6 +3) * (-2)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-6, calc.getResult());
}

TEST(AdvancedVariables, AssignNegativeBrackets) {
    Calculator calc;
    calc = "x =-(-6 +3) * (-2)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ("x = -6", calc.getVariables());
}

TEST(Results, Division) {
    Calculator calc;
    calc = "9 /3";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(3, calc.getResult());
}

TEST(Results, DivZero) {
    Calculator calc;
    calc = "2/0";
    ASSERT_EQ(ZERO_DIVISION, calc.calculate());
}

TEST(Results, DivZero2) {
    Calculator calc;
    calc = "(6-5-1)";
    calc.calculate();
    ASSERT_EQ(0, calc.getResult());
    calc = "5 / (6 - 5 - 1)";
    ASSERT_EQ(ZERO_DIVISION, calc.calculate());
}

TEST(Variables, EvaluateVariable) {
    Calculator calc;
    calc = "x = 5";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(5, calc.getResult());
}

TEST(Variables, NegativeVariable) {
    Calculator calc;
    calc = "x = -5";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-5, calc.getResult());
}

TEST(Variables, ReturnVariable) {
    Calculator calc;
    calc = "x=6";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(6, calc.getResult());
    calc = "x";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(6, calc.getResult());
}

TEST(Variables, EvaluateWithVariable) {
    Calculator calc;
    calc = "x=5";
    calc.calculate();
    calc = "(x + 3) / (2)";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(4, calc.getResult());
}

TEST(Variables, EvaluateWithNegativeVariable) {
    Calculator calc;
    calc = "x = -5";
    calc.calculate();
    calc = "(x + 3) / 2";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(-1, calc.getResult());
}

TEST(AdvancedVariables, EvaluateWithTwoVariables) {
    Calculator calc;
    calc = "x = 5";
    calc.calculate();
    calc = "y = 3";
    calc.calculate();
    calc = "(x + y) * 2";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(16, calc.getResult());
}

TEST(AdvancedVariables, ReferenceVariable) {
    Calculator calc;
    calc = "x=23";
    ASSERT_EQ(VALID, calc.calculate());
    calc = "y= x*2";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(46, calc.getResult());
    ASSERT_EQ("x = 23; y = 46", calc.getVariables());
}

TEST(AdvancedVariables, ResultAssignment) {
    Calculator calc;
    calc = "4* -12 + 52";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(4, calc.getResult());
    calc = "x = res /2 *5";
    calc.calculate();
    calc = "x +2";
    calc.calculate();
    ASSERT_EQ(12, calc.getResult());
}

TEST(Variables, UsingResult) {
    Calculator calc;
    calc = " 3*8";
    calc.calculate();
    calc = "res + 5";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(29, calc.getResult());
}

TEST(Variables, UsingResult2) {
    Calculator calc;
    calc = " 3*8";
    calc.calculate();
    calc = "res + 5";
    calc.calculate();
    calc = "res + 5";
    ASSERT_EQ(VALID, calc.calculate());
    ASSERT_EQ(34, calc.getResult());
}

TEST(Variables, NoResult) {
    Calculator calc;
    calc = "res + 5";
    ASSERT_EQ(INVALID, calc.calculate());
}
