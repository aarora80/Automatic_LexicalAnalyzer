//Arnav Arora

/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <unordered_set>
#include <stack>
#include "parser.h"

using namespace std;

//Global String of Errors
std::string global_errors="";
std:: string global_inputString = "empty";

//REG STRUCTURES and GRAPH FUNCTIONS
struct REG_node {
    struct REG_node* first_neighbor;
    char first_label;
    struct REG_node* second_neighbor;
    char second_label;
};

struct REG {
    struct REG_node* start;
    struct REG_node* accept;
};

// Function to create a new REG node
struct REG_node* create_node(char label) {
    struct REG_node* node = (struct REG_node*)malloc(sizeof(struct REG_node));
    node->first_neighbor = NULL;
    node->first_label = label;
    node->second_neighbor = NULL;
    node->second_label = '\0';
    return node;
}

// Function to create a new REG with the given start and accept nodes
struct REG* create_reg(struct REG_node* start, struct REG_node* accept) {
    struct REG* reg = (struct REG*)malloc(sizeof(struct REG));
    reg->start = start;
    reg->accept = accept;
    //cout<<"This is the header node's accept add: "<<reg->accept<<endl;
    return reg;
}

// Base case for regular expression "_"
struct REG* REG_underscore() {
    // Create a single node for "_"
    struct REG_node* startnode = create_node('_');
    struct REG_node* acceptnode = create_node('_');
    startnode->first_neighbor = acceptnode;
    return create_reg(startnode, acceptnode); 
}

// Base case for regular expression "CHAR"
struct REG* REG_char(char CHAR) {
    // Create a single node for "a"
    struct REG_node* startnode = create_node(CHAR);
    struct REG_node* acceptnode = create_node('_');
    startnode->first_neighbor = acceptnode;
    return create_reg(startnode, acceptnode);
}

// Regular expression for "r*"
struct REG* REG_star(struct REG* r, Token tokenName) {
    // TODO: Implement REG_star based on the provided logic

    // Create two new REG_nodes
    struct REG_node* new_start_node = create_node('_');
    new_start_node->second_label = '_';
    struct REG_node* new_accept_node = create_node('\0');

    // Connect the first new node to the start node of the passed in REG
    new_start_node->first_neighbor = r->start;
    new_start_node->second_neighbor = new_accept_node;

    // Connect the accept node of the passed in REG to the second new node
    r->accept->first_label='_';
    r->accept->first_neighbor = new_accept_node;

    // Create a loop by connecting the accept node of the passed in REG to its start
    r->accept->second_label='_';
    r->accept->second_neighbor = r->start;

    // std::cout <<"Inside REGSTAR"<<endl;
    // std::cout << "RegStartSHOULDBEfADD: " << r->start << std::endl;
    // std::cout << "RegAccept: " << r->accept->first_neighbor << std::endl;
    //std::cout << "SBACCEPT_RegStartSecondNeighbor: " << r->start->second_neighbor << std::endl;
    //std::cout << "SBACCEPT_RegStartSecondNeighborFirstNeighbor: " << r->start->second_neighbor->first_neighbor << std::endl;


    // Return the constructed REG graph
    return create_reg(new_start_node, new_accept_node);
}

// Regular expression for "r1|r2"
struct REG* REG_or(struct REG* r1, struct REG* r2, Token tokenName) {
    // TODO: Implement REG_or based on the provided logic
    struct REG_node* new_start_node = create_node('_');
    struct REG_node* new_accept_node = create_node('\0');
    new_start_node->first_neighbor = r1->start;
    new_start_node->second_label = '_';
    new_start_node->second_neighbor = r2->start;

    r1->accept->first_label='_';
    r1->accept->first_neighbor = new_accept_node;
    r2->accept->first_label='_';
    r2->accept->first_neighbor = new_accept_node;
    
    // Return the constructed REG graph

    return create_reg(new_start_node,new_accept_node);
}

// Regular expression for "r1.r2"
struct REG* REG_dot(struct REG* r1, struct REG* r2, Token tokenName) {
    // TODO: Implement REG_dot based on the provided logic
    r1->accept->first_label = '_';
    r1->accept->first_neighbor = r2->start;

    // Return the constructed REG graph
    return create_reg(r1->start,r2->accept);
}

//GLOBAL Vector of tokens
struct TokenREGPair {
    Token token;
    struct REG* reg;
};

std::vector<TokenREGPair> tokenRegPairs; 
//Global Vector or string of Error Messages

// this syntax error function needs to be 
// modified to produce the appropriate message
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR"<<endl;
    exit(0);
}

void Parser::syntax_error_expr(Token tokenName)
{
    cout << tokenName.lexeme <<" HAS A SYNTAX ERROR IN ITS EXPRESSION"<<endl;
    exit(0);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

Token Parser::expect_expr(TokenType expected_type, Token tokenName)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_expr(tokenName);
    return t;
}

struct REG* Parser::parse_expr(Token tokenName, bool isTopLevel) 
{
    //expr → CHAR
    //expr → LPAREN expr RPAREN DOT LPAREN expr RPAREN
    //expr → LPAREN expr RPAREN OR LPAREN expr RPAREN
    //expr → LPAREN expr RPAREN STAR
    //expr → UNDERSCORE   
    TokenREGPair pair;
    pair.token = tokenName;

    Token t = lexer.peek(1);

    //t.Print();

    if (t.token_type == CHAR){
        expect_expr(CHAR,tokenName);
        //construct REG Graph Base Case
        pair.reg = REG_char(t.lexeme[0]);
        //tokenRegPairs.push_back(pair);
        //return pair.reg;
    }
    else if(t.token_type == UNDERSCORE){
        expect_expr(UNDERSCORE,tokenName);
        //construct REG Graph Base case
        pair.reg = REG_underscore();
        //tokenRegPairs.push_back(pair);
        //return pair.reg;
    }
    else if(t.token_type == LPAREN){
        expect_expr(LPAREN,tokenName);
        struct REG* r = parse_expr(tokenName, false);
        expect_expr(RPAREN,tokenName);
        t = lexer.peek(1);
        //t.Print();
        if(t.token_type == OR){
            expect_expr(OR,tokenName);
            expect_expr(LPAREN,tokenName);
            //construct REG Graph for OR given a REG GRAPH
            struct REG* r2 = parse_expr(tokenName, false);
            expect_expr(RPAREN,tokenName);
            pair.reg = REG_or(r, r2, tokenName);
            //tokenRegPairs.push_back(pair);
            //return pair.reg;
        }
        else if(t.token_type == DOT){
            expect_expr(DOT,tokenName);
            expect_expr(LPAREN,tokenName);
            //construct REG Graph for DOT given a REG GRAPH
            struct REG* r2 = parse_expr(tokenName, false);
            expect_expr(RPAREN,tokenName);
            pair.reg = REG_dot(r, r2, tokenName);
            //tokenRegPairs.push_back(pair);
            //return pair.reg;

        }
        else if(t.token_type == STAR){
            expect_expr(STAR,tokenName);
            pair.reg = REG_star(r, tokenName);
            //tokenRegPairs.push_back(pair);
            //return pair.reg;
        }
        else{
             syntax_error_expr(tokenName);
             return nullptr;
        }
    }
    else{
        syntax_error_expr(tokenName);
        return nullptr;
    }
    if (isTopLevel) {
        tokenRegPairs.push_back(pair);
    }
    
    return pair.reg;

}

void Parser::parse_token()
{
    //token → ID expr
    Token mostrecenttoken = expect(ID);
    //mostrecenttoken.Print();
    //add to global vector of tokens
    auto it = tokenRegPairs.begin();
    for (const auto& pair : tokenRegPairs) {
        if(pair.token.lexeme == mostrecenttoken.lexeme){
            global_errors += "Line " + std::to_string(mostrecenttoken.line_no) + ": " + pair.token.lexeme + " already declared on line " + std::to_string(pair.token.line_no) + "\n";
            break;
        }
    }
    parse_expr(mostrecenttoken);
   
}

void Parser::parse_token_list()
{
    //token_list → token
    //token_list → token COMMA token_list
    parse_token();
    Token t = lexer.peek(1);
    if(t.token_type == COMMA){
        expect(COMMA);
        parse_token_list();
    }
    else if(t.token_type == HASH){
        return;
    }
    else{
        syntax_error();
    }

}

void Parser::parse_tokens_section()
{
    //tokens_section → token_list HASH
    parse_token_list();
    expect(HASH);
    
}

void Parser::parse_input()
{
    //input --> tokens_section INPUT_TEXT
    parse_tokens_section();
    global_inputString = expect(INPUT_TEXT).lexeme;
    //cout<<global_inputString<<endl;
}

void Parser::parse_INPUT()
{
    parse_input();
    expect(END_OF_FILE);
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for your solution and it is only provided to
// illustrate the basic functionality of getToken() and the Token type.

// Function to match one character based on the provided pseudocode
std::set<REG_node*> Parser::Match_One_Char(const std::set<REG_node*>& S, char c) {
    //cout<<"I entered Match_One_Char"<<endl;
    // 1. Find all nodes that can be reached from S by consuming c
    std::set<REG_node*> S_prime;

    for ( auto& n : S) {
        // Assuming that nodes have a first_neighbor and first_label
        if (n->first_label == c) {
            S_prime.insert(n->first_neighbor);
        }
    }

    // for(auto& n:S_prime){
    //     cout<<"This is inside Match_One_Char and these are the nodes reachable by consuming char: "<<n<<endl;
    // }

    // If S' is empty, return an empty set
    if (S_prime.empty()) {
        return {};
    }

    // 2. Find all nodes that can be reached from S' by consuming no input
    bool changed = true;
    std::set<REG_node*> S_double_prime;

    while (changed) {
        changed = false;

        for (auto& n : S_prime) {
            //cout<<"Matchonechar epsilonclosure call"<<endl;
            //cout<<n<<endl;
            S_double_prime.insert(n);


            // Assuming that nodes have a first_neighbor and first_label
            if(n->first_neighbor != NULL){
                if (n->first_label == '_' && S_prime.find(n->first_neighbor) == S_prime.end()) {
                S_double_prime.insert(n->first_neighbor);
                changed = true;
                }

            }
            
            // Assuming that nodes have a second_neighbor and second_label
            if(n->second_neighbor != NULL){
                if (n->second_label == '_' && S_prime.find(n->second_neighbor) == S_prime.end()) {
                S_double_prime.insert(n->second_neighbor);
                changed = true;
                }
            }
            
        }

        

        // If S' is not equal to S'', update S' and reset S''
        if (S_prime != S_double_prime) {
            changed = true;
            S_prime = S_double_prime;
            S_double_prime.clear();
        }
        // for(auto& n:S_prime){
        //         cout<<"This is inside Match_One_Char and this is S_prime: "<<n<<endl;
        //     }

    }
    
    //S_double_prime.clear();
    // At this point, S' contains all nodes that can be reached from S
    // by first consuming c, then traversing 0 or more epsilon edges
    // for(auto& n:S_prime){
    //     cout<<"This is inside Match_One_Char and these are the nodes reachable by consuming char and _ the final set: "<<n<<endl;
    // }
    return S_prime;
}


// Function to implement match(r, s, p)
size_t Parser::match(REG* r, const std::string& s, size_t p) {
    int matchingp = p;
   // std::cout<<"I entered Match"<<endl;

    //std::cout<<"This is s "<<s<<endl;
    //std::cout<<"This is p "<<p<<endl;

    std::set<REG_node*> current_states = {r->start};
    std::set<REG_node*> accepting_states = {r->accept};
    std::string matched_prefix = "";

    // Initial step to find states that can be reached by consuming no input
    std::set<REG_node*> epsilon_closure = EpsilonClosure(current_states);

    //std::cout<<"Epsilon Closure States the initial states that are reachable by the start node "<<endl;
    // for ( auto& state : epsilon_closure) {
    //      std::cout << state->first_label << " "<<endl;  // Assuming REG_node has a suitable way to print
    //     }



    //Check for Epsilon Error
    // for (const auto& state : epsilon_closure) {
    //     if (state == r->accept) {
    //         // Update the global string of errors
    //         global_errors += "EPSILON IS NOOOOOOOT A TOKEN !!!\n";
    //     }
    // }

    //std::cout<<"I made it past the epsilon error"<<endl;

    //std::cout<<"This is the length of s: "<<s.length()<<endl;

    while (p < s.length() && !epsilon_closure.empty()) {


        // Match one character at a time
        // if(isspace(s[p])){
        //     return;
        // }
        char current_char = s[p];

        //std::cout<<"This is the input string to match "<<s<<endl;
        //std::cout<<"This is current_char "<<current_char<<endl;
        std::set<REG_node*> new_states = Match_One_Char(epsilon_closure, current_char);
        
        // std::cout << "Contents of new_states:" << std::endl;
        // // for ( auto& state : new_states) {
        // //      std::cout << state << " ";  // Assuming REG_node has a suitable way to print
        // // }
        
        //std::cout<<"I am now checking if a new accepting state is reached"<<endl;


        // Check if any accepting state is reached
        for ( auto& state : new_states) {
            if (accepting_states.find(state) != accepting_states.end()) {
                // Update the matched prefix
                matchingp = p+1;
                //std::cout<<"I entered this condition so I found the match and here is the matched prefix: "<<matched_prefix<<endl;
            }
            // else{
            //     p++;
            // }
        }

        // Update the current states for the next iteration
        epsilon_closure = new_states;
        p++;
    }

    return matchingp;
}

// Function to compute epsilon closure of a set of states
std::set<REG_node*> Parser::EpsilonClosure(const std::set<REG_node*>& states) {
    std::set<REG_node*> S_prime = states;
    bool changed = true;
    
    while (changed) {
        changed = false;
        std::set<REG_node*> S_double_prime;

        for (auto& n : S_prime) {
            //cout<<"This is first label of n: "<<n->first_label<<endl;
            //cout<<"This is second label of n: "<<n->second_label<<endl;
            //cout<<""<<endl;
            S_double_prime.insert(n);

            // for(auto& n : S_double_prime){
            //     cout<<"Contents of S_double_prime before: "<<n->first_label<<endl;
            // }
        
        
            //cout<<"Before if statement check"<<endl;
            if (n->first_label == '_' && n->first_neighbor && S_prime.find(n->first_neighbor) == S_prime.end()) {
                S_double_prime.insert(n->first_neighbor);
                changed = true;
            }
        
            if (n->second_label == '_' && n->second_neighbor && S_prime.find(n->second_neighbor) == S_prime.end()) {
                S_double_prime.insert(n->second_neighbor);
                changed = true;
            }
            //cout<<"After if statement check"<<endl;
        }
        //cout<<"Did I make it heree"<<endl;
        if (S_prime != S_double_prime) {
            changed = true;
            S_prime = S_double_prime;
            // for(auto& n : S_prime){
            //     cout<<"Contents of S_prime: "<<n->first_label<<endl;
            // }
        }
    }
    //cout<<"Made it out of the while loop"<<endl;
    return S_prime;
}


std::pair<Token, std::string> Parser::my_GetToken(const std::string& input, size_t& current_position) {
    size_t original_position = current_position;

    // // Skip spaces
    // while (current_position < input.length() && isspace(input[current_position])) {
    //     ++current_position;
    // }

    Token longestMatchedToken;
    size_t longestMatchLength = 0;
    std::string longestMatchedLexeme;
     int newposition = 0;
    

    // Iterate over each token in the list
    for (auto& tokenRegPair : tokenRegPairs) {

        if(isspace(input[current_position])){
            current_position++;
        }

        if(current_position == global_inputString.length()){
            exit(0);
        }

        //cout<<"This is current_position going into match"<<endl;
        //cout<<current_position<<endl;

        newposition = match(tokenRegPair.reg, input, current_position);

        //cout<<"This is the current position: "<<current_position<<endl;
        //cout<<"This is the position returned by match: "<<newposition<<endl;

        // Call match for the current REG starting from the current position
        std::string lexeme = input.substr(current_position,newposition-current_position);

        //std::cout<<"This is the lexeme: "<<lexeme<<endl;


        // If a non-empty lexeme is matched, update the current position and check if it's the longest match
        if (!lexeme.empty() && lexeme.length() > longestMatchLength) {
            longestMatchedToken = tokenRegPair.token;
            longestMatchLength = lexeme.length();
            longestMatchedLexeme = lexeme;
            
        }
    }

    // If no match is found, reset the position and return an invalid token
    if (longestMatchLength == 0) {
        //std::cout<<"I entered longestMatchLength Condition"<<endl;
        current_position = original_position;
        return std::make_pair(Token(), std::string());  // Return an empty pair as an indication of no match
    }

    // Update the position to the end of the matched lexeme
    current_position += longestMatchLength-1;

    return std::make_pair(longestMatchedToken, longestMatchedLexeme);
}





int main()
{
    std::string epsilonTokens="";
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will 
    // not work correctly
    Parser parser;

    //cout<<"BEFORE I have parsed the Input"<<endl;

    //parser.readAndPrintAllInput();
        parser.parse_INPUT();

        // for(const auto& pair : tokenRegPairs){
        //     cout<<"Start Reg: "<<pair.reg->start->first_label<<endl;
        //     cout<<"Accept Reg: "<<pair.reg->accept->first_label<<endl;
        // }
        

        for(const auto& pair : tokenRegPairs){
             if (!tokenRegPairs.empty()) {
            auto hasEmptyStringLambda = [pair,&parser](REG* r) {
            std::set<REG_node*> epsilonClosure = parser.EpsilonClosure({r->start});
            //cout<<"I made it past epsilon closure"<<endl;
            return epsilonClosure.find(r->accept) != epsilonClosure.end();
            };

            // Check if the current token's regular expression can generate the empty string
            // Check if the current token's regular expression can generate the empty string
             if (hasEmptyStringLambda(pair.reg)) {
                epsilonTokens += pair.token.lexeme + " ";
                }
             }
        }

        //cout<<"I made it past the for loop "<<endl;
        if (!epsilonTokens.empty()) {
            global_errors += "EPSILON IS NOOOOOOOT A TOKEN !!! " + epsilonTokens + "\n";
        }
//     std::cout<<"This is the global vector after the input has been parsed"<<endl;
//     for (auto& pair : tokenRegPairs) {
//     std::cout << "Token: " << pair.token.lexeme << " |  REG: " << pair.reg->start << " Accept REG: " <<pair.reg->accept<< std::endl;
//     std::cout << "SBf_SAME_RegFirstNeighbor: " << pair.reg->start->first_neighbor<<endl;
//     std::cout << "SBSTARACCEPT_RegFirstFIRSTNeighbor: " << pair.reg->start->first_neighbor->first_neighbor->first_neighbor->first_neighbor->first_neighbor<<endl;
//     std::cout << "SAME_RegFirstFIRSTSECONDNeighbor: " << pair.reg->start->first_neighbor->first_neighbor->second_neighbor<<endl;
//     std::cout << "SBSTARACCEPT_RegSecondNeighbor: " << pair.reg->start->second_neighbor->first_neighbor->first_neighbor<<endl;
//     //std::cout << "RegSecondNeighbor: " << pair.reg->start->second_neighbor->first_label<<endl;
//     //std::cout << "bNeighbor: " << pair.reg->start->first_neighbor->first_neighbor<<endl;
//     //std::cout << "eNeighbor: " << pair.reg->start->second_neighbor->first_neighbor->first_label<<endl;
// }
    
    //cout<<"I have parsed the Input"<<endl;
    //cout<<"This is the Input String: "<<global_inputString<<endl;
    //if error message string/vector not empty then print string and exit (SEMANTIC ERROR)
    if (!global_errors.empty()) {
        // Print error messages and exit
        std::cout << global_errors;
        exit(0);  // Exit with an error code
    }

    //Now check if any of the expressions produce epsion and produce error message if any of them produce epsilon (NO SYNTAX OR SEMANTIC ERROR)

    // for (const auto& pair : tokenRegPairs) {

    // }

    //Now NO SYNTAX NO SEMANTIC NO EPSILON -> Now we do lexical analysis
    // Assuming there are no syntax or semantic errors, proceed to lexical analysis
    size_t current_position = 0;


    global_inputString = global_inputString.substr(1,global_inputString.length()-2);
    //std::cout << "bruh: " << global_inputString << std::endl;
    //bool errorPrinted = false;
    // Iterate over the input string and perform lexical analysis
    while (current_position < global_inputString.length()) {
        //std::cout<<"This is current_position in main: "<<current_position<<endl;
    // Call my_GetToken to get the next token and its lexeme
    std::pair<Token, std::string> tokenAndLexeme = parser.my_GetToken(global_inputString, current_position);

    // Access the Token and its lexeme
    Token nextToken = tokenAndLexeme.first;
    std::string lexeme = tokenAndLexeme.second;

    // Print the Token and its lexeme
    if(nextToken.lexeme.empty()){
            std::cout << "ERROR" << std::endl;
            exit(1);
    }
    else{
        std::cout << nextToken.lexeme << ", \"" << lexeme << "\"" << std::endl;
    }
    
    current_position++;
}

    return 0;
	
}
