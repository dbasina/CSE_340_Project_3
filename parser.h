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
    vector<string> initializedInCurrentScope;
    map<string,int> declarationLocation;
    map<string,TokenType> localVariables;
    map<string,bool> referenceMap;
    map<string,bool> initializationMap;
    scope *parent;
};

class Parser
{
  public:
    scope *currentScope = NULL;
    std::vector<scope> scopeVector;
    std::vector<string> declarationErrors;
    std::vector<string> typeMismatchErrors;
    map<string,string> initializationErrors;
    std::vector<string> initializationErrorsVector;
    std::vector<string> noErrorsOutput;
    LexicalAnalyzer lexer;
    Parser();
    void syntaxError(string location);
    void checkBuiltInType(string variableId);
    bool lookupInLocalScope(scope *scope,string tokenId);
    bool lookup(scope *scope,string tokenId);
    TokenType variableType(scope *scope,string tokenId);
    bool checkInitialization(scope *scope,string tokenId);
    bool setInitialization(scope *scope,string tokenId);
    void undoInitialization(scope *scope,string tokenId);
    int findDeclarationLocation(scope *scope,string tokenId);
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
    TokenType parsePrimary(int lineNumber);
    TokenType parseBoolConstant();
    TokenType parseCondition();

};