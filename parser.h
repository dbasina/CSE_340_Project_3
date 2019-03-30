#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <map>
#include "lexer.h"
using namespace std;
struct variable
{
  std::string name;
  TokenType variableType;
};
struct scope
{
    vector<string> variableIds;
    map<string,TokenType> localVariables;
    map<string,bool> referenceMap;
    scope *parent;
};

class Parser
{
  public:
    scope *currentScope = NULL;
    std::vector<scope> scopeVector;
    std::vector<string> semanticErrors;
    std::vector<string> typeMismatchErrors;
    LexicalAnalyzer lexer;
    Parser();
    void syntaxError(string location);
    void checkBuiltInType(string variableId);
    bool lookupInLocalScope(scope *scope,string tokenId);
    bool lookup(scope *scope,string tokenId);
    TokenType variableType(scope *scope,string tokenId);
    TokenType typeCheck(TokenType operatorType, TokenType operandType1, TokenType operandType2,int lineNumber);
    bool referenceVariableLocalScope(scope *scope, string variableId);
    void referenceVariable(scope *scope, string variableId);
    TokenType peek();
    TokenType doublePeek();
    Token expect(TokenType expectedTokenType,string location);
    void parseProgram();
    void parseScope();
    void parseScopeList();
    void parseVariableDeclaration();
    vector<string> parseIdList();
    TokenType parseTypeName();
    void parseStatementList();
    void parseStatement();
    void parseAssignmentStatement();
    void parseWhileStatement();
    TokenType parseExpression(int lineNumber);
    void parseArithmeticOperator();
    void parseBinaryBooleanOperator();
    void parseRelationalOperator();
    TokenType parsePrimary();
    TokenType parseBoolConstant();
    void parseCondition();

};