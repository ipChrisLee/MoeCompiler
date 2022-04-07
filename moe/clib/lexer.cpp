#include <lexer.hpp>


int TokenDFA::Node::cnt=0;
std::unique_ptr<TwoPointerScanner>TwoPointerScanner::single=nullptr;

int TokenDFA::main(){
    if(!mdb::setSysEnable()) return 0;
    std::map<pnode,bool>vis;
    cprt::cprintLn(int(int(TokenDFA::get().nodes.size())==Node::cnt));
    dfsPrint(get().start,vis);
    TwoPointerScanner::init();
    std::vector<std::shared_ptr<Token>>tokens;
    char ch='\0';
    while( (ch=TwoPointerScanner::get().read())!='\0' ){
        auto [b,fun] = TokenDFA::get().trans(ch);
        if(!b && !fun){
            cprt::cprintLn("Fail with null function.");
        }else if(!b){
            TwoPointerScanner::get().reset();
            std::shared_ptr<Token>ptoken=fun(TwoPointerScanner::get().dump());
            if(ptoken==nullptr) continue;
            tokens.push_back(ptoken);
            cprt::cprintLn(ptoken->toString());
            cprt::printLn(
                ptoken->toString(),
                sysy::get().getOutStream()
            );
        }
    }
    
    return 0;
}

namespace unitest{
    int lexerTest(std::vector<std::string>){
        return TokenDFA::main();
    }
    bool lexerTestAdded(){
        static bool added=unitest::TestMain::get().addTestCase("lexerTest",lexerTest);
        return added;
    }
}
