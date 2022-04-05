#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <tuple>
#include <map>
#include <functional>

#include <unittest.hpp>
#include <common.hpp>

class Token{
public: 
    enum Type{ OP,INUM,FNUM,DELIM,ID };
    Type type;
    std::string lexeme;
    Token(Type type,const std::string & lexeme):type(type),lexeme(lexeme){
    }
    static bool is(Token * t,Type type){
        return t->type==type;
    }
};

class Token_OP : public Token{
public:
    char op;
    Token_OP(const std::string & lexeme,char op):Token(Token::OP,lexeme),op(op){
    }
};

class Token_INUM : public Token{
public:
    int32_t inum;
    Token_INUM(const std::string & lexeme,int32_t inum):Token(Token::INUM,lexeme),inum(inum){
    }
};

class Token_FNUM : public Token{
public:
    float fnum;
    Token_FNUM(const std::string & lexeme,float fnum):Token(Token::FNUM,lexeme),fnum(fnum){
    }
};

class Token_DELIM : public Token{
public:
    char delim;
    Token_DELIM(const std::string & lexeme,char delim):Token(Token::DELIM,lexeme),delim(delim){
    }
};

class Token_ID : public Token{
public:
    std::string id;
    Token_ID(const std::string & lexeme,const std::string & id):
        Token(Token::ID,lexeme),id(id){
    }
};


class TokenDFA{
public:
#define ff first
#define ss second
    struct Node{
        static int cnt;
        int id;
        std::map<std::pair<char,char>,Node *>oute;
        std::function<std::shared_ptr<Token>(std::string)>doWhenFail;
        Node():id(++cnt),doWhenFail(nullptr){
        }
    };
    std::vector<std::shared_ptr<Node>>nodes;
    Node * start;
    Node * now;
    TokenDFA():start(new Node()),now(start){
    }

    std::tuple<bool,std::function<std::shared_ptr<Token>(std::string)>>
        trans(char ch){
        bool transited=false;
        std::function<std::shared_ptr<Token>(std::string)>fun=nullptr;
        for(auto [p,n]:now->oute){
            if(p.ff<=ch && ch<=p.ss){
                now=n;transited=true;
            }
        }
        if(!transited) fun=now->doWhenFail;
        return std::tuple(transited,fun);
    }
    
    /*  Merge TokenDFA.
     *  For sysy, after erasing the comments, to merge dfas, we only
     *  need to consider the start state (since the first character can
     *  distinguish the token type).
     *  This function will throw exception if the start states of two dfa
     *  has intersection of out edge.
     * */
    void merge(TokenDFA & dfa){
        for(auto p:dfa.nodes) if(p.get()!=dfa.start) nodes.push_back(p);
        for(auto [po,no]:start->oute){
            for(auto [pa,na]:dfa.start->oute){
                if( (pa.ff<=po.ff && po.ff<=pa.ss) 
                    || (pa.ff<=po.ss && po.ss<=pa.ss) ){
                    com::Throw("Merging TokenDFA whose start state has intersection in out edge.");
                }
            }
        }
        start->oute.insert(dfa.start->oute.begin(),dfa.start->oute.end());
    }

private:
    static TokenDFA getDFAofOP(){
        TokenDFA dfa;
        return dfa;
    }
    
#undef ff
#undef ss
};

namespace unitest{
    
}
