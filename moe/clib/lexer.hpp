#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <tuple>
#include <map>
#include <functional>
#include <set>
#include <sstream>
#include <climits>
#include <regex>

#include <unittest.hpp>
#include <common.hpp>
#include <sysy.hpp>

class Token{
public: 
    enum Type{ OP,INUM,FNUM,DELIM,ID,KEY,STR,CHAR,COMMENT,BLANK };
    std::string tokenName(){
        switch(type){
            case OP: return "OP";
            case INUM: return "INUM";
            case FNUM: return "FNUM";
            case DELIM: return "DELIM";
            case ID: return "ID";
            case KEY: return "KEY";
            case STR: return "STR";
            case CHAR: return "CHAR";
            case COMMENT: return "COMMENT";
            case BLANK: return "BLANK";
        }
    }
    Type type;
    std::string lexeme;
    Token(Type type,const std::string & lexeme):type(type),lexeme(lexeme){
    }
    static bool is(Token * t,Type type){
        return t->type==type;
    }
    std::string toString(){
        return tokenName()+"["+lexeme+"]";
    }
    static std::shared_ptr<Token> getTokenNULL([[maybe_unused]] const std::string & lexeme){
        com::notFinished(FUNINFO,CODEPOS);
        return nullptr;
    }
};

class Token_OP : public Token{
public:
    Token_OP(const std::string & lexeme):Token(Token::OP,lexeme){
    }
    static std::shared_ptr<Token> getTokenOP(const std::string & lexeme){
        Token * ptoken=new Token_OP(lexeme);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_INUM : public Token{
public:
    int32_t inum;
    Token_INUM(const std::string & lexeme,int32_t inum):Token(Token::INUM,lexeme),inum(inum){
    }
    static std::shared_ptr<Token> getTokenHexINUM(const std::string & lexeme){
        Token * ptoken=new Token_INUM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
    static std::shared_ptr<Token> getTokenDecINUM(const std::string & lexeme){
        Token * ptoken=new Token_INUM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
    static std::shared_ptr<Token> getTokenOctINUM(const std::string & lexeme){
        Token * ptoken=new Token_INUM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_FNUM : public Token{
public:
    float fnum;
    Token_FNUM(const std::string & lexeme,float fnum):Token(Token::FNUM,lexeme),fnum(fnum){
    }
    static std::shared_ptr<Token> getTokenHexFNUM(const std::string & lexeme){
        Token * ptoken=new Token_FNUM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
    static std::shared_ptr<Token> getTokenDecFNUM(const std::string & lexeme){
        Token * ptoken=new Token_FNUM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_DELIM : public Token{
public:
    char delim;
    Token_DELIM(const std::string & lexeme,char delim):Token(Token::DELIM,lexeme),delim(delim){
    }
    static std::shared_ptr<Token> getTokenDELIM(const std::string & lexeme){
        Token * ptoken=new Token_DELIM(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_ID : public Token{
public:
    std::string id;
    Token_ID(const std::string & lexeme,const std::string & id):
        Token(Token::ID,lexeme),id(id){
    }
};

class Token_KEY : public Token{
public:
    std::string key;
    Token_KEY(const std::string & lexeme,const std::string & key):
        Token(Token::KEY,lexeme),key(key){
    }
    static std::shared_ptr<Token> getTokenIDOrKEY(const std::string & lexeme){
        Token * ptoken=nullptr;
        if(sysy::reservedWords.count(lexeme)) ptoken=new Token_KEY(lexeme,lexeme);
        else ptoken=new Token_ID(lexeme,lexeme);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_STR : public Token{
public:
    std::string str;
    Token_STR(const std::string & lexeme,const std::string & str):
        Token(Token::STR,lexeme),str(str){
    }
    static std::shared_ptr<Token> getTokenSTR(const std::string & lexeme){
        Token * ptoken=new Token_STR(lexeme,lexeme);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_CHAR : public Token{
public:
    char ch;
    Token_CHAR(const std::string & lexeme,const char & ch):
        Token(Token::CHAR,lexeme),ch(ch){
    }
    static std::shared_ptr<Token> getTokenCHAR(const std::string & lexeme){
        Token * ptoken=new Token_CHAR(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_COMMENT : public Token{
public:
    Token_COMMENT(const std::string & lexeme):Token(Token::COMMENT,lexeme){}

    static std::shared_ptr<Token> getTokenCOMMENT(const std::string & lexeme){
        Token * ptoken=new Token_COMMENT(lexeme);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class Token_BLANK : public Token{
public:
    char blank;
    Token_BLANK(const std::string & lexeme,char blank):Token(Token::BLANK,lexeme),blank(blank){
    }
    static std::shared_ptr<Token> getTokenBLANK(const std::string & lexeme){
        Token * ptoken=new Token_BLANK(lexeme,0);
        com::notFinished(FUNINFO,CODEPOS);
        return std::shared_ptr<Token>(ptoken);
    }
};

class TokenDFA{
public:
#define ff first
#define ss second
#define mp std::make_pair
    struct Node{
        static int cnt;
        int id;
        std::map<std::pair<char,char>,Node *>oute;
        std::function<std::shared_ptr<Token>(const std::string &)>doWhenFail;
        Node():id(++cnt),doWhenFail(nullptr){
        }
        std::string toString() const {
            std::stringstream sstream;
            sstream<<"Node id=["<<id<<"], hasFun=["<<bool(doWhenFail)<<"], oute={ ";
            for(auto [p,n]:oute){
                sstream<<"[<"<<p.ff<<" "<<p.ss<<"> "<<
                    " <"<<int(p.ff)<<" "<<int(p.ss)<<"> , "<<
                    n->id<<"] ";
            }
            sstream<<"}";
            std::string buf;std::getline(sstream,buf);return buf;
        }
    };
    using pnode=Node *;
    std::vector<std::shared_ptr<Node>>nodes;
    Node * start;
    Node * now;

    std::tuple<bool,std::function<std::shared_ptr<Token>(std::string)>>trans(char ch){
        bool transited=false;
        std::function<std::shared_ptr<Token>(std::string)>fun=nullptr;
        for(auto [p,n]:now->oute){
            if(p.ff<=ch && ch<=p.ss){
                now=n;transited=true;
            }
        }
        if(!transited) {
            fun=now->doWhenFail;
            now=start;
        }
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
        for(auto & p:dfa.nodes) {
            if(p.get()!=dfa.start) {
                nodes.push_back(p);
            }
        }
        //mdb::getStatic().infoPrint(std::pair("dfa.nodes[0].count",dfa.nodes[0].use_count()) );
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
    TokenDFA(){
        start=new Node();
        now=start;
        nodes.push_back(std::shared_ptr<Node>(start));
    }
    static void dfsPrint(Node * u,std::map<pnode,bool>&vis){
        vis[u]=true;
        mdb::getStatic().msgPrint(u->toString());
        for(auto [p,v]:u->oute){
            if(!vis[v]) dfsPrint(v,vis);
        }
    }

    static TokenDFA getDFAofOP(){
        const std::set<std::string> & opStr=sysy::operators;
        TokenDFA dfa;
        dfa.nodes.push_back(std::make_shared<Node>());
        dfa.start=dfa.nodes[0].get();
        dfa.now=dfa.nodes[0].get();
        for(auto str:opStr){
            pnode u=dfa.start;
            for(auto ch:str){
                bool fnd=false;
                for(auto [p,v]:u->oute){
                    if(p.ff<=ch && ch<=p.ss){
                        u=v;fnd=true;break;
                    }
                }
                if(!fnd){
                    pnode v=new Node();
                    u->oute.insert(std::make_pair(std::make_pair(ch,ch),v));
                    dfa.nodes.push_back(std::shared_ptr<Node>(v));
                    u=v;
                }
            }
            u->doWhenFail=Token_OP::getTokenOP;
        }

        {   // For comments
            pnode Sls=dfa.start->oute[mp('/','/')];
            pnode Smcs=new Node(),Sstar=new Node(),Sscs=new Node(),Sced=new Node();
            dfa.nodes.push_back(std::shared_ptr<Node>(Smcs));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sstar));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sscs));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sced));
            
            Sls->oute.insert({
                mp(mp('*','*'),Smcs),
                mp(mp('/','/'),Sscs)
            });

            Smcs->oute.insert({
                mp(mp(1,'*'-1),Smcs),
                mp(mp('*','*'),Sstar),
                mp(mp('*'+1,127),Smcs)
            });

            Sscs->oute.insert({
                mp(mp(1,'\n'-1),Sscs),
                mp(mp('\n','\n'),Sced),
                mp(mp('\n'+1,127),Sscs)
            });

            Sstar->oute.insert({
                mp(mp(1,'/'-1),Smcs),
                mp(mp('/','/'),Sced),
                mp(mp('/'+1,127),Smcs),
            });

            Sced->doWhenFail=Token_COMMENT::getTokenCOMMENT;
        }

        {   // For string
            pnode Sdq=new Node(),Stran=new Node(),Ssed=new Node();
            dfa.nodes.push_back(std::shared_ptr<Node>(Sdq));
            dfa.nodes.push_back(std::shared_ptr<Node>(Stran));
            dfa.nodes.push_back(std::shared_ptr<Node>(Ssed));
            
            dfa.start->oute.insert({
                mp(mp('\"','\"'),Sdq)
            });
            
            Sdq->oute.insert({
                mp(mp(1,'\n'-1),Sdq),
                // \n cannot in string expicit.
                mp(mp('\n'+1,'"'-1),Sdq),
                mp(mp('"','"'),Ssed),
                mp(mp('"'+1,'\\'-1),Sdq),
                mp(mp('\\','\\'),Stran),
                mp(mp('\\'+1,127),Sdq)
            });

            Stran->oute.insert({
                mp(mp(1,127),Sdq) // "\\\n" is ignored by scanner.
            });

            Ssed->oute.insert({});
            Ssed->doWhenFail=Token_STR::getTokenSTR;
        }

        {   // For char
            pnode Ssq=new Node(),Stranc=new Node(),Sced=new Node(),Stced=new Node();
            
            dfa.nodes.push_back(std::shared_ptr<Node>(Ssq));
            dfa.nodes.push_back(std::shared_ptr<Node>(Stranc));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sced));
            dfa.nodes.push_back(std::shared_ptr<Node>(Stced));
            
            dfa.start->oute.insert({
                mp(mp('\'','\''),Ssq)
            });
            
            Ssq->oute.insert({
                mp(mp(1,'\''-1),Sced),
                // for '\'', fail with error.
                mp(mp('\'','\\'-1),Sced),
                mp(mp('\\','\\'),Stranc),
                mp(mp('\\'+1,127),Sced)
            });

            Stranc->oute.insert({
                mp(mp(1,127),Sced)
            });

            Sced->oute.insert({
                mp(mp('\'','\''),Stced)
            });

            Stced->doWhenFail=Token_CHAR::getTokenCHAR;
            
        }
        
        return dfa;
    }

    static TokenDFA getDFAofDELIM(){
        const std::set<std::string> & opStr=sysy::delimiter;
        TokenDFA dfa;
        dfa.nodes.push_back(std::make_shared<Node>());
        dfa.start=dfa.nodes[0].get();
        dfa.now=dfa.nodes[0].get();
        for(auto str:opStr){
            pnode u=dfa.start;
            for(auto ch:str){
                bool fnd=false;
                for(auto [p,v]:u->oute){
                    if(p.ff<=ch && ch<=p.ss){
                        u=v;fnd=true;break;
                    }
                }
                if(!fnd){
                    pnode v=new Node();
                    u->oute.insert(mp(mp(ch,ch),v));
                    dfa.nodes.push_back(std::shared_ptr<Node>(v));
                    u=v;
                }
            }
            u->doWhenFail=Token_DELIM::getTokenDELIM;
        }
        
        return dfa;
    }
    
    static TokenDFA getDFAofNUM(){
        TokenDFA dfa;
        dfa.nodes.push_back(std::make_shared<Node>());
        dfa.start=dfa.nodes[0].get();
        dfa.now=dfa.nodes[0].get();
        
        pnode St=dfa.start;
        pnode Sz=new Node();dfa.nodes.push_back(std::shared_ptr<Node>(Sz));
        St->oute.insert({mp(mp('0','0'),Sz)});
        Sz->doWhenFail=Token_INUM::getTokenOctINUM;
        {   // For hex value.
            pnode S0=new Node(),S1=new Node(),S2=new Node(),Se=new Node();
            pnode Sp=new Node(),Sm=new Node(),Sf=new Node();
            dfa.nodes.push_back(std::shared_ptr<Node>(S0));
            dfa.nodes.push_back(std::shared_ptr<Node>(S1));
            dfa.nodes.push_back(std::shared_ptr<Node>(S2));
            dfa.nodes.push_back(std::shared_ptr<Node>(Se));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sp));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sm));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sf));

            Sz->oute.insert({
                mp(mp('x','x'),S0),mp(mp('X','X'),S0),
            });

            S0->oute.insert({
                mp(mp('0','9'),S1),mp(mp('A','F'),S1),
                mp(mp('.','.'),S2)
            });

            S1->oute.insert({
                mp(mp('0','9'),S1),mp(mp('A','F'),S1),
                mp(mp('.','.'),Se),
                mp(mp('p','p'),Sp),mp(mp('P','P'),Sp)
            });
            S1->doWhenFail=Token_INUM::getTokenHexINUM;

            S2->oute.insert({
                mp(mp('0','9'),Se),mp(mp('A','Z'),Se)
            });

            Se->oute.insert({
                mp(mp('p','p'),Sp),mp(mp('P','P'),Sp)
            });

            Sp->oute.insert({
                mp(mp('+','+'),Sm),mp(mp('-','-'),Sm),
                mp(mp('0','9'),Sf)
            });

            Sm->oute.insert({
                mp(mp('0','9'),Sf)
            });

            Sf->oute.insert({
                mp(mp('0','9'),Sf)
            });
            Sf->doWhenFail=Token_FNUM::getTokenHexFNUM;
        }
        
        {   // For dec and oct value
            pnode Snx=new Node(),Smd=new Node(),Sno=new Node(),Sad=new Node(),Sd=new Node();
            pnode Sep=new Node(),Ssep=new Node(),Sef=new Node();
            
            dfa.nodes.push_back(std::shared_ptr<Node>(Snx));
            dfa.nodes.push_back(std::shared_ptr<Node>(Smd));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sno));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sad));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sd));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sep));
            dfa.nodes.push_back(std::shared_ptr<Node>(Ssep));
            dfa.nodes.push_back(std::shared_ptr<Node>(Sef));

            St->oute.insert({
                mp(mp('1','9'),Sno),mp(mp('0','0'),Sz),mp(mp('.','.'),Smd)
            });
            Sz->oute.insert({
                mp(mp('.','.'),Sd),
                mp(mp('e','e'),Sep),mp(mp('E','E'),Sep),
                mp(mp('0','7'),Snx),
                mp(mp('8','9'),Sno)
            });

            Snx->oute.insert({
                mp(mp('0','7'),Snx),
                mp(mp('8','8'),Sno),
                mp(mp('.','.'),Sd),
                mp(mp('e','e'),Sep),mp(mp('E','E'),Sep)
            });
            Snx->doWhenFail=Token_INUM::getTokenOctINUM;

            Smd->oute.insert({
                mp(mp('0','9'),Sad)
            });

            Sno->oute.insert({
                mp(mp('0','9'),Sno),
                mp(mp('.','.'),Sd),
                mp(mp('e','e'),Sep),mp(mp('E','E'),Sep)
            });
            Sno->doWhenFail=Token_INUM::getTokenDecINUM;

            Sad->oute.insert({
                mp(mp('0','9'),Sad),
                mp(mp('e','e'),Sep),mp(mp('E','E'),Sep)
            });
            Sad->doWhenFail=Token_FNUM::getTokenDecFNUM;

            Sd->oute.insert({
                mp(mp('0','9'),Sd),
                mp(mp('e','e'),Sep),mp(mp('E','E'),Sep)
            });
            Sd->doWhenFail=Token_FNUM::getTokenDecFNUM;

            Sep->oute.insert({
                mp(mp('+','+'),Ssep),mp(mp('-','-'),Ssep),
                mp(mp('0','9'),Sef)
            });

            Ssep->oute.insert({
                mp(mp('0','9'),Sef)
            });

            Sef->oute.insert({
                mp(mp('0','9'),Sef)
            });
            Sef->doWhenFail=Token_FNUM::getTokenDecFNUM;
        }
        return dfa;
    }

    static TokenDFA getDFAofID(){
        TokenDFA dfa;
        dfa.nodes.push_back(std::make_shared<Node>());
        dfa.start=dfa.nodes[0].get();
        dfa.now=dfa.nodes[0].get();
        pnode St=dfa.start;
        pnode Sa=new Node();dfa.nodes.push_back(std::shared_ptr<Node>(Sa));
        St->oute.insert({
            mp(mp('a','z'),Sa),mp(mp('A','Z'),Sa),mp(mp('_','_'),Sa)
        });
        Sa->oute.insert({
            mp(mp('0','9'),Sa),mp(mp('a','z'),Sa),mp(mp('A','Z'),Sa),mp(mp('_','_'),Sa)
        });
        Sa->doWhenFail=Token_KEY::getTokenIDOrKEY;
        return dfa;
    }

    static TokenDFA getDFAofBLANK(){
        TokenDFA dfa;
        dfa.nodes.push_back(std::make_shared<Node>());
        dfa.start=dfa.nodes[0].get();
        dfa.now=dfa.nodes[0].get();
        pnode St=dfa.start;
        pnode Sbl=new Node();
        dfa.nodes.push_back(std::shared_ptr<Node>(Sbl));

        St->oute.insert({
            mp(mp('\n','\n'),Sbl),
            mp(mp('\t','\t'),Sbl),
            mp(mp(' ',' '),Sbl)
        });

        Sbl->oute.insert({
            mp(mp('\n','\n'),Sbl),
            mp(mp('\t','\t'),Sbl),
            mp(mp(' ',' '),Sbl)
        });
        Sbl->doWhenFail=Token_BLANK::getTokenBLANK;
        return dfa;
    }

    static TokenDFA init(){
        TokenDFA single;
        TokenDFA dfaOP=getDFAofOP();
        single.merge(dfaOP);
        TokenDFA dfaDELIM=getDFAofDELIM();
        single.merge(dfaDELIM);
        TokenDFA dfaNUM=getDFAofNUM();
        single.merge(dfaNUM);
        TokenDFA dfaID=getDFAofID();
        single.merge(dfaID);
        TokenDFA dfaBLANK=getDFAofBLANK();
        single.merge(dfaBLANK);
        return single;
    }
    
public:
    static TokenDFA & get(){
        static TokenDFA single=init();
        return single;
    }

    static int main(); 
#undef ff
#undef ss
};

class TwoPointerScanner{
private:
    std::string buf;
    int rownum;
    using ps=std::string::iterator;
    ps pl,pr;
    static std::unique_ptr<TwoPointerScanner>single;
    TwoPointerScanner(){
        std::string s;
        sysy::get().resetIn();
        while(std::getline(sysy::get().getInStream(),s)){
            buf+=s;
        }
        buf.push_back('\n');
        pl=buf.begin();pr=buf.begin();
        rownum=1;
    }
public:
    TwoPointerScanner(const TwoPointerScanner &)=delete;
    TwoPointerScanner & operator = (const TwoPointerScanner &)=delete;
    ~TwoPointerScanner(){}
    static TwoPointerScanner & get(){
        if(single==nullptr) com::ThrowSingletonNotInited("TwoPointerScanner");
        return *single;
    }
    static void init(){
        single=std::unique_ptr<TwoPointerScanner>(new TwoPointerScanner());
    }
    static void del(){
        single=nullptr;
    }
    char read(){
        if(pr!=buf.end() && pr+1!=buf.end() && *pr=='\\' && *(pr+1)=='\n') {
            ++rownum;pr=pr+2;
        }
        if(pr==buf.end()) return '\0';
        char res=*pr;++pr;
        if(res=='\n') ++rownum;
        return res;
    }
    void reset(){
        --pr;
    }
    std::string dump(){
        std::string res;
        for(auto it=pl;it!=pr;++it){
            res.push_back(*it);
        }
        pl=pr;
        return res;
    }
};

namespace unitest{
    int dfaTest(std::vector<std::string>);
    __attribute__((constructor)) bool dfaTestAdded();
}
