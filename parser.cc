#include "parser.h"
#include <algorithm>
Parser::Parser()
{

}
bool Parser::lookupInLocalScope(scope *scope, string tokenId)
{
  if(std::find(scope->variableIds.begin(),scope->variableIds.end(),tokenId) != scope->variableIds.end())
  {
    return true;
  }

  else
  {
    return false;
  }
}

bool Parser::lookup(scope *scope,string tokenId)
{
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
      return true;
    else
    {
      return lookup(scope->parent,tokenId);  
    }
    
  }

  return false;
}

bool Parser::referenceVariableLocalScope(scope *scope, string variableId)
{
  if(scope->referenceMap.find(variableId) != scope->referenceMap.end())
  {
    return true;
  }

  else
  {
    return false;
  }
}
void Parser::referenceVariable(scope *scope, string variableId)
{
  if (scope!=NULL)
  {
    if(referenceVariableLocalScope(scope,variableId))
      scope->referenceMap[variableId] = true;
    else
    {
      return referenceVariable(scope->parent,variableId);  
    }
    
  }
}
void Parser::syntaxError(string location)
{
  
  //cout<<"Syntax Error:"<<location<<endl;
  cout<<"Syntax Error"<<endl;
  exit(0);
}

TokenType Parser::peek()
{
    Token t = lexer.GetToken();
    TokenType retval = lexer.UngetToken(t);
    return retval;
    
}

TokenType Parser::doublePeek()
{
    Token t = lexer.GetToken();
    Token t1= lexer.GetToken();
    TokenType retval = lexer.UngetToken(t1);
    TokenType type = lexer.UngetToken(t);
    return retval;
}

Token Parser::expect(TokenType expectedTokenType,string location)
{
    Token t = lexer.GetToken();
    if (t.token_type == expectedTokenType)
    {
        return t;
    }

    else
    {
        syntaxError(location);
    }

}
void Parser::parseProgram()
{
  parseScope();
}

void Parser::parseScope()
{
  expect(LBRACE,"parseScope");
  // create new scope
  scope *newScope = new scope();
  newScope->parent = currentScope;
  currentScope = newScope;     

  parseScopeList();
  expect(RBRACE,"parseScope");
  
  for(map<string,bool>::iterator referenceIterator = currentScope->referenceMap.begin();referenceIterator!=currentScope->referenceMap.end();++referenceIterator)
  {
    if (referenceIterator->second == false)
    {
      semanticErrors.push_back("ERROR CODE 1.3 " + referenceIterator->first);
    }
  }

  scope *temp = currentScope;
  currentScope = currentScope->parent;
  free(temp);
}

void Parser::parseScopeList()
{
  // scope_list -> scope
  // scope_list -> scope scope_list
  TokenType type = peek();

  if (type == LBRACE)
  {
    parseScope();


    TokenType type1 = peek();
    if (type1 == LBRACE)
    {
      parseScopeList();
    }

    else if (type1 == ID)
    {
      parseScopeList();
    }

    else if (type1 == WHILE)
    {
      parseScopeList();
    }

    else
    {
      // Do nothing
    }
  }

  // scope_list -> var_decl
  // scope_list -> var_decl scope_list
  // scope_list -> stmt(assign_stmt)
  // scope_list -> stmt scope_list
  else if (type == ID)
  {
    TokenType type1 = doublePeek();

    // var_decl -> id_list COLON type_name SEMICOLON

    // id_list -> ID
    if (type1 == COLON)
    {
      parseVariableDeclaration();

     TokenType type2 = peek();


      if (type2 == LBRACE)
      {
        parseScopeList();
      }

      else if (type2 == ID)
      {
       
        parseScopeList();
      }

      else if (type2 == WHILE)
      {
        
        parseScopeList();
      }

      else
      {
        
        // Do nothing
      }
    }
    // id_list -> ID COMMA
    else if (type1  == COMMA)
    {
      parseVariableDeclaration();

      TokenType type2 = peek();
      if (type2 == LBRACE)
      {
        parseScopeList();
      }

      else if (type2 == ID)
      {
        
        parseScopeList();
      }

      else if (type2 == WHILE)
      {
        
        parseScopeList();
      }

      else
      {
        
        // Do nothing
      }
    }

    // assign_stmt -> ID EQUAL expr SEMICOLON
    else if (type1  == EQUAL)
    {
      parseAssignmentStatement();

      TokenType type2 = peek();
      if (type2 == LBRACE)
      {
        parseScopeList();
      }

      else if (type2 == ID)
      {
        parseScopeList();
      }

      else if (type2 == WHILE)
      {
        parseScopeList();
      }

      else
      {
        
        // Do nothing
      }
    }

    else
    {
      string location = "parseScopeList";
      syntaxError(location);
    }
  }

  // scope_list -> stmt(while_stmt)
  // scope_list -> stmt(while_stmt) scope_list
  else if (type == WHILE)
  {
    parseWhileStatement();

    TokenType type1 = peek();
    if (type1 == LBRACE)
    {
      parseScopeList();
    }

    else if (type1 == ID)
    {
      parseScopeList();
    }

    else if (type1 == WHILE)
    {
      parseScopeList();
    }

    else
    {
      // Do nothing
    }
  }

}

void Parser::parseVariableDeclaration()
{
  
    vector<string> newDeclarations = parseIdList();
    expect(COLON,"parseVariableDeclaration 1");
    TokenType declarationType = parseTypeName();

    // Add newVariables to currentScope
    for (vector<string>::iterator declaredId = newDeclarations.begin(); declaredId!=newDeclarations.end();++declaredId)
    {
      if(std::find(currentScope->variableIds.begin(),currentScope->variableIds.end(),*declaredId) != currentScope->variableIds.end())
      {
        semanticErrors.push_back("ERROR CODE 1.1 " + *declaredId);  
      }

      else
      {
        //Add variable scope
        currentScope->variableIds.push_back(*declaredId);
        currentScope->localVariables.insert({*declaredId,declarationType});
        currentScope->referenceMap.insert({*declaredId,false});


      }
    }
    expect(SEMICOLON,"parseVariableDeclaration 2");

}

vector<string> Parser::parseIdList()
{
    vector<string> retVector;
    Token t = expect(ID,"parseIdList");    
    retVector.push_back(t.lexeme);
    TokenType type = peek();
    if (type == COMMA)
    {
        expect(COMMA,"parseIdList");

        //Construct return vector
        vector<string> multipleInputs;
        multipleInputs = parseIdList();
        for(vector<string>::iterator IdIterator= multipleInputs.begin(); IdIterator!=multipleInputs.end(); ++IdIterator)
        {
          retVector.push_back(*IdIterator);
        }
    }

    return retVector;
}

TokenType Parser::parseTypeName()
{
  Token t = lexer.GetToken();
  if (t.token_type == REAL)
  {
      return t.token_type;
  }
  else if (t.token_type == INT)
  {
      return t.token_type;
  }
  else if (t.token_type == BOOLEAN)
  {
      return t.token_type;
  }
  else if (t.token_type == STRING)
  {
      return t.token_type;
  }
  else
  {
    string location = "parseTypeName";
    syntaxError(location);
  }
}

void Parser::parseStatementList()
{
  parseStatement();
  TokenType type = peek();
  if (type == ID)
  {
    TokenType type1 = doublePeek();
    if (type1 == EQUAL)
    {
      parseStatementList();
    }
  }

  else if (type == WHILE)
  { 
    parseStatementList();
  }

  else
  {
    // Do Nothing
  }
}

void Parser::parseStatement()
{
  TokenType type = peek();
  if (type == ID)
  {
    parseAssignmentStatement();
  }

  else if (type == WHILE)
  {
    parseWhileStatement();
  }
  else
  {
    string location = "parseStatement";
    syntaxError(location);
  }
}

void Parser::parseAssignmentStatement()
{
    Token t = expect(ID,"parseAssignmentStatement");
    if(!lookup(currentScope,t.lexeme))
    {
      semanticErrors.push_back("ERROR CODE 1.2 "+t.lexeme);
    }

    else
    {
      referenceVariable(currentScope, t.lexeme);
    }

    expect(EQUAL,"parseAssignmentStatement");
    parseExpression();
    expect(SEMICOLON,"parseAssignmentStatement");
    
}

void Parser::parseWhileStatement()
{
    expect(WHILE,"parseWhileStatement");
    parseCondition();

    TokenType type = peek();
    if (type == LBRACE)
    {
        expect(LBRACE,"parseWhileStatement");
        parseStatementList();
        expect(RBRACE,"parseWhileStatement");
    }
    else
    {
        parseStatement();
    } 
}

void Parser::parseExpression()
{
    TokenType type = peek();
    if (type == PLUS or type == MINUS or type == MULT or type == DIV)
    {       
        parseArithmeticOperator();
        parseExpression();
        parseExpression();
    }

    else if (type == AND or type == OR or type == XOR)
    {       
        parseBinaryBooleanOperator();
        parseExpression();
        parseExpression();
    }

    else if (type == GREATER or type == GTEQ or type == LESS or type == NOTEQUAL or type == LTEQ)
    {       
        parseRelationalOperator();
        parseExpression();
        parseExpression();
    }

    else if (type == NOT)
    {
        expect(NOT,"parseExpression");
        parseExpression();
    }

    else
    {
        parsePrimary();
    }
}

void Parser::parseArithmeticOperator()
{
  Token t = lexer.GetToken();
  if (t.token_type == PLUS or t.token_type == MINUS or t.token_type == MULT or t.token_type == DIV)
  {
    //Do something
  }
  else
  {
    string location = "parseArithmeticOperator";
    syntaxError(location);
  }
}

void Parser::parseBinaryBooleanOperator()
{
  Token t = lexer.GetToken();
  if (t.token_type == AND or t.token_type == OR or t.token_type == XOR)
  {
    // Do something
  }
  else
  {
    string location = "parseBinaryBooleanOperator";
    syntaxError(location);
  }
}

void Parser::parseRelationalOperator()
{
  Token t = lexer.GetToken();
  if (t.token_type == GREATER or t.token_type == GTEQ or t.token_type == LESS or t.token_type == NOTEQUAL or t.token_type == LTEQ)
  {
    //Do something
  }
  else
  {
    string location = "parseRelationalOperator";
    syntaxError(location);
  }
}

void Parser::parsePrimary()
{
  Token t = lexer.GetToken();

  if (t.token_type == ID or t.token_type == NUM or t.token_type == REALNUM or t.token_type == STRING_CONSTANT)
  {
    if (t.token_type == ID)
    {
      if(!lookup(currentScope,t.lexeme))
      {
        semanticErrors.push_back("ERROR CODE 1.2 "+t.lexeme);
      }

      else
      {
        referenceVariable(currentScope, t.lexeme);
      }
    }
  }

  else
  {
    TokenType type = lexer.UngetToken(t);
    parseBoolConstant();
  }
}

void Parser::parseBoolConstant()
{
  Token t = lexer.GetToken();
  if (t.token_type == TRUE)
  {
    // do something
  }

  else if (t.token_type == FALSE)
  {
    // do something
  }

  else
  {
    string location = "parseBoolConstant";
    syntaxError(location);
  }
}

void Parser::parseCondition()
{
  expect(LPAREN,"parseCondition");
  parseExpression();
  expect(RPAREN,"parseCondition");
}

int main()
{
  Parser newParser;
  newParser.parseProgram();

  for( vector<string>::iterator semanticError = newParser.semanticErrors.begin(); semanticError!=newParser.semanticErrors.end(); ++semanticError)
  {
    cout<<*semanticError<<endl;
  } 
}