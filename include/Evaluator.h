#pragma once
#include "AST.h"
#include <unordered_map>
#include <vector>
#include <memory>

struct FunctionInfo {
    std::vector<std::string> argNames;
    std::unique_ptr<ASTNode> body;
};

class Evaluator {
    std::unordered_map<std::string, double> variables;
    std::unordered_map<std::string, FunctionInfo> functions;

public:
    Evaluator();
    double evaluate(const ASTNode& node, std::unordered_map<std::string, double>* localVars = nullptr);
};