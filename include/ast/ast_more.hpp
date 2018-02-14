#ifndef ast_more_hpp
#define ast_more_hpp

extern std::vector<std::string> g_var;

class Identifier: public Node{
private:
    const std::string* id;
    
public:
    Identifier(const std::string* _id)
        :id(_id){}
        
    void sendGlobal()const{
        g_var.push_back(*id);
    }
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }
    
    virtual void printPy(std::ostream& dst) const override{
        dst << *id;
    }
    
    int getLength() const override{return 1;}

    virtual ~Identifier() override{
        delete id;
    }
};

#endif
