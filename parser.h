//Arnav Arora


/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

struct REG_node;
struct REG;
struct TokenREGPair;

class Parser {
  public:
    void parse_INPUT();
    void readAndPrintAllInput();
    std::pair<Token, std::string>my_GetToken(const std::string& input, size_t& current_position);
    std::set<REG_node*> EpsilonClosure(const std::set<REG_node*>& states);
  private:
    void parse_input();
    LexicalAnalyzer lexer;
    void syntax_error();
    void syntax_error_expr(Token tokenName);
    Token expect(TokenType expected_type);
    Token expect_expr(TokenType expected_type, Token tokenName);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    struct REG* parse_expr(Token tokenName, bool isTopLevel = true);
    std::set<REG_node*> Match_One_Char(const std::set<REG_node*>& S, char c);
    
    size_t match(REG* r, const std::string& s, size_t p);
};

#endif

