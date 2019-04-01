#include "parser.h"
#include <algorithm>
#include <string>
string builtInTypes[] = { "REAL", "INT", "BOOLEAN", "STRING"};
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

TokenType Parser::variableType(scope *scope,string tokenId)
{
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
      return scope->localVariables[tokenId];
    else
    {
      return variableType(scope->parent,tokenId);  
    }
    
  }
}

bool Parser::checkInitialization(scope *scope,string tokenId)
{
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
      return scope->initializationMap[tokenId];
    else
    {
      return checkInitialization(scope->parent,tokenId);  
    }   
  }
}

bool Parser::setInitialization(scope *scope,string tokenId)
{
  bool retval = false;
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
    {
      if(!(scope->initializationMap[tokenId]))
      {
        scope->initializationMap[tokenId] = true;
        retval = true;
        return retval;
      }
      
    }
      
    else
    {
      return setInitialization(scope->parent,tokenId);  
    }
    
  }
}

void Parser::undoInitialization(scope *scope,string tokenId)
{
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
    {
      scope->initializationMap[tokenId] = false;
    }
      
    else
    {
      undoInitialization(scope->parent,tokenId);  
    }
    
  }
}
TokenType Parser::typeCheck(TokenType type, TokenType operandType1, TokenType operandType2, int lineNumber)
{
  
  if (type == PLUS or type == MINUS or type == MULT or type == DIV)
  {       
       if(operandType1 == REAL or operandType1 == INT)
       {
         if(operandType2 == REAL or operandType2 == INT )
         {
           if(operandType1 == REAL or operandType2 == REAL)
           {
             return REAL;
           }
           else
           {
             if (type == DIV)
             {
               return REAL;
             }
             else
             {
               return INT;
             }
             
           }
         }
         else
         {
           string errorMessage = "TYPE MISMATCH "+std::to_string(lineNumber)+" C3";
           typeMismatchErrors.push_back(errorMessage);
           return ERROR;
         }
       }

       else
       {
         typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+" C3");
         return ERROR;
       }
  }


  else if (type == AND or type == OR or type == XOR)
  {       
      if (operandType1 == BOOLEAN and operandType2 == BOOLEAN)
      {
        return BOOLEAN;
      }
      else
      {
        typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+" C4");
        return ERROR;
      }
  }
  
  else if (type == GREATER or type == GTEQ or type == LESS or type == NOTEQUAL or type == LTEQ)
  {       
       //string
      if(operandType1 == STRING)
      {
        if(operandType2 == STRING)
        {
          return BOOLEAN;
        }
        else
        {
          typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+ " C5");
          return ERROR;
        }
      }

       //boolean
      else if(operandType1 == BOOLEAN)
      {
        if(operandType2 == BOOLEAN)
        {
          return BOOLEAN;
        }
        else
        {
          typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+ " C5");
          return ERROR;
        }
      }

       // int or real
      else if(operandType1 == REAL or operandType1 == INT)
      {
        if(operandType2 == REAL or operandType2 == INT)
        {
          return BOOLEAN;
        }
        else
        {
          typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+ " C6");
          return ERROR;
        }
      }

  }

  
  else if (type == NOT)
  {
    if (operandType1 != operandType2)
    {
      typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(lineNumber)+ " C8");
      return ERROR;
    }
  }
  
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
    {
      scope->referenceMap[variableId] = true;
      
    }
    else
    {
      referenceVariable(scope->parent,variableId);  
    }
    
  }
}

int Parser::findDeclarationLocation(scope *scope,string tokenId)
{
  if (scope!=NULL)
  {
    if(lookupInLocalScope(scope,tokenId))
    {
      return scope->declarationLocation[tokenId];
    }
      
    else
    {
      return findDeclarationLocation(scope->parent,tokenId);  
    }
    
  }
}
void Parser::syntaxError(string location)
{
  
  //cout<<"Syntax Error:"<<location<<endl;
  cout<<"Syntax Error"<<endl;
  exit(0);
}

void Parser::checkBuiltInType(string variableId)
{
  for (int i = 0; i < 4; i++) 
  {
    if (variableId == builtInTypes[i]) 
    {
      syntaxError("checkBuiltInType");
    }
  }
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
  expect(END_OF_FILE,"parseProgram");
}

void Parser::parseScope()
{
  expect(LBRACE,"parseScope");
  // create new scope
  scope *newScope = new scope();
  newScope->parent = currentScope;
  currentScope = newScope;     

  parseScopeList();

  // Undo Initializations in this scope
  for (vector<string>::iterator initIterator = currentScope->initializedInCurrentScope.begin(); initIterator != currentScope->initializedInCurrentScope.end();++initIterator)
  {
    undoInitialization(currentScope,*initIterator);
  }
  expect(RBRACE,"parseScope");
  
  for(map<string,bool>::iterator referenceIterator = currentScope->referenceMap.begin();referenceIterator!=currentScope->referenceMap.end();++referenceIterator)
  {
    if (referenceIterator->second == false)
    {
      declarationErrors.push_back("ERROR CODE 1.3 " + referenceIterator->first);
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

  else
  {
    syntaxError("parseScopeList");
  }

}

void Parser::parseVariableDeclaration()
{
  
    vector<string> newDeclarations = parseIdList();
    Token t = expect(COLON,"parseVariableDeclaration 1");
    TokenType declarationType = parseTypeName();

    // Add newVariables to currentScope
    for (vector<string>::iterator declaredId = newDeclarations.begin(); declaredId!=newDeclarations.end();++declaredId)
    {
      if(std::find(currentScope->variableIds.begin(),currentScope->variableIds.end(),*declaredId) != currentScope->variableIds.end())
      {
        declarationErrors.push_back("ERROR CODE 1.1 " + *declaredId);  
      }

      else
      {
        //Add variable scope
        currentScope->variableIds.push_back(*declaredId);
        currentScope->localVariables.insert({*declaredId,declarationType});
        currentScope->referenceMap.insert({*declaredId,false});
        currentScope->initializationMap.insert({*declaredId,false});
        currentScope->declarationLocation.insert({*declaredId, t.line_no});

      }
    }
    expect(SEMICOLON,"parseVariableDeclaration 2");

}

vector<string> Parser::parseIdList()
{
    vector<string> retVector;
    Token t = expect(ID,"parseIdList");
    checkBuiltInType(t.lexeme);   
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
    checkBuiltInType(t.lexeme);
    if(!lookup(currentScope,t.lexeme))
    {
      declarationErrors.push_back("ERROR CODE 1.2 "+t.lexeme);
    }

    else
    {
      referenceVariable(currentScope, t.lexeme);
      string decLoc = std::to_string(findDeclarationLocation(currentScope,t.lexeme));
      string refLoc = std::to_string(t.line_no);
      noErrorsOutput.push_back(t.lexeme+" "+refLoc+" "+decLoc);
    }

    expect(EQUAL,"parseAssignmentStatement");
    TokenType rhsType = parseExpression(t.line_no);
    
    //Set initialization after parsing rightside
    bool initializedNow = setInitialization(currentScope,t.lexeme);
    if (initializedNow)
    {
      currentScope->initializedInCurrentScope.push_back(t.lexeme);
    }
    

    if(lookup(currentScope,t.lexeme))
    {
      if (rhsType!=ERROR)
      {
        TokenType lhsType = variableType(currentScope,t.lexeme);

        if (lhsType == REAL)
        {
          if (rhsType==INT or rhsType==REAL)
          {}
          else
          {
            typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(t.line_no)+ " C2");
          }
        }
        else
        {
          if(lhsType == INT)
          {
            if (rhsType!=INT)
            {
              typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(t.line_no)+ " C1");
            }
          }

          else if (lhsType == BOOLEAN)
          {
            if (rhsType!=BOOLEAN)
            {
              typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(t.line_no)+ " C1");
            }
          } 

          else if (lhsType == STRING)
          {
            if (rhsType!=STRING)
            {
              typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(t.line_no)+ " C1");
            }
          }
        }        
      }
    }
    
    //cout<<"Expression Type: "<<expressionType<<endl;
    expect(SEMICOLON,"parseAssignmentStatement");
    
}

void Parser::parseWhileStatement()
{
    Token t = expect(WHILE,"parseWhileStatement");
    TokenType conditionType = parseCondition();
    if (conditionType!=ERROR)
    {
      if(conditionType!=BOOLEAN)
      {
        typeMismatchErrors.push_back("TYPE MISMATCH "+std::to_string(t.line_no)+ " C7");
      }
    }

    TokenType type = peek();
    if (type == LBRACE)
    {
        
        expect(LBRACE,"parseWhileStatement");

        scope *newScope = new scope();
        newScope->parent = currentScope;
        currentScope = newScope;     
        parseStatementList();

        for (vector<string>::iterator initIterator = currentScope->initializedInCurrentScope.begin(); initIterator != currentScope->initializedInCurrentScope.end();++initIterator)
        {
          undoInitialization(currentScope,*initIterator);
        }

        expect(RBRACE,"parseScope");
  
        scope *temp = currentScope;
        currentScope = currentScope->parent;
        free(temp);    
        
        //parseScope();
    }
    else
    {
        parseStatement();
    } 
}

TokenType Parser::parseExpression(int lineNumber)
{
    TokenType type = peek();
    if (type == PLUS or type == MINUS or type == MULT or type == DIV)
    {       
       parseArithmeticOperator();
       TokenType operandType1 = parseExpression(lineNumber);
       TokenType operandType2 = parseExpression(lineNumber);
       return typeCheck(type,operandType1,operandType2,lineNumber);
    }

    else if (type == AND or type == OR or type == XOR)
    {       
        parseBinaryBooleanOperator();
        TokenType operandType1 = parseExpression(lineNumber);
        TokenType operandType2 = parseExpression(lineNumber);
        return typeCheck(type,operandType1,operandType2,lineNumber);
    }

    else if (type == GREATER or type == GTEQ or type == LESS or type == NOTEQUAL or type == LTEQ)
    {       
        parseRelationalOperator();
        TokenType operandType1 = parseExpression(lineNumber);
        TokenType operandType2 = parseExpression(lineNumber);
        return typeCheck(type,operandType1,operandType2,lineNumber);
    }

    else if (type == NOT)
    {
        expect(NOT,"parseExpression");
        TokenType operandType1 = parseExpression(lineNumber);
        return typeCheck(type,operandType1,BOOLEAN,lineNumber);
    }
  
    else
    {
        return parsePrimary(lineNumber);
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

TokenType Parser::parsePrimary(int lineNumber)
{
  Token t = lexer.GetToken();

  if (t.token_type == ID or t.token_type == NUM or t.token_type == REALNUM or t.token_type == STRING_CONSTANT)
  {
    if (t.token_type == ID)
    {
      checkBuiltInType(t.lexeme);
      if(!lookup(currentScope,t.lexeme))
      {
        declarationErrors.push_back("ERROR CODE 1.2 "+t.lexeme);
      }

      else
      {
        referenceVariable(currentScope, t.lexeme);
        string decLoc = std::to_string(findDeclarationLocation(currentScope,t.lexeme));
        string refLoc = std::to_string(t.line_no);
        noErrorsOutput.push_back(t.lexeme+" "+refLoc+" "+decLoc);
        if(!checkInitialization(currentScope,t.lexeme))
        {
          /*
          initializationErrors.insert({t.lexeme,"UNINITIALIZED " + t.lexeme + " "+ std::to_string(lineNumber)});
          initializationErrorsVector.push_back(initializationErrors[t.lexeme]);
          */
          initializationErrorsVector.push_back("UNINITIALIZED " + t.lexeme + " "+ std::to_string(lineNumber));
          
        }
        return variableType(currentScope,t.lexeme);
      }
    }
    
    else if(t.token_type == NUM)
    {
      return INT;
    }

    else if(t.token_type == REALNUM)
    {
      return REAL;
    }
    
    else if(t.token_type == STRING_CONSTANT)
    {
      return STRING;
    }
  }

  else
  {
    TokenType type = lexer.UngetToken(t);
    return parseBoolConstant();
  }
}

TokenType Parser::parseBoolConstant()
{
  Token t = lexer.GetToken();
  if (t.token_type == TRUE)
  {
    return BOOLEAN;
  }

  else if (t.token_type == FALSE)
  {
    
    return BOOLEAN;
  }

  else
  {
    string location = "parseBoolConstant";
    syntaxError(location);
  }
}

TokenType Parser::parseCondition()
{
  Token t = expect(LPAREN,"parseCondition");
  TokenType conditionType = parseExpression(t.line_no);
  expect(RPAREN,"parseCondition");
  return conditionType;
  
}

int main()
{
  Parser newParser;
  newParser.parseProgram();

  if (newParser.declarationErrors.size()>0)
  {
    cout<<newParser.declarationErrors[0]<<endl;
  }

  else 
  {
    if (newParser.typeMismatchErrors.size()>0)
    {
      cout<<newParser.typeMismatchErrors[0]<<endl;  
    }

    else
    {
      if (newParser.initializationErrorsVector.size()>0)
      {
        for (std::vector<string>::iterator initError = newParser.initializationErrorsVector.begin(); initError!= newParser.initializationErrorsVector.end();++initError)
        {
          cout<< *initError <<endl;
        }
      }

      else
      {
        for (std::vector<string>::iterator output = newParser.noErrorsOutput.begin(); output!= newParser.noErrorsOutput.end();++output)
        {
          cout<<*output<<endl;
        }
      }
    }
    
  }
}