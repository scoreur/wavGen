//
//  xml.cpp
//  
//
//  Created by user on 8/13/14.
//
//

#include "../include/xml.h"
using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

typedef map<string, string>::iterator info_p;
template <typename T>
int find(vector<T> const &s, T const &t){
    
    for(int iter = 0;iter < s.size();++iter)
    {
        if(s[iter] == t)
            return iter;
    }
    return -1;
    
}
//****************************************************

////////////////////////////////////////////////////////////
/// @ brief constructor
Node::Node(string _tag, Node *p):
    tag(_tag),attributes(),contents(string("")),
    parent(p), elderBro(0), littleBro(0), eldestChild(0)
{
    if(p != 0){//add to parent's children list
        if(p->eldestChild != 0){//already have children
            Node * i =p->eldestChild;
            while( i->littleBro != 0)
                i = i->littleBro;
            i->littleBro = this;
            elderBro = i;
        }else{//previous no children
            p->eldestChild = this;
        }
    }
}

/// @brief destructor
Node::~Node(){
    //remove from parent's children list
    if(littleBro != 0) littleBro->elderBro = elderBro;
    if(elderBro != 0) elderBro->littleBro = littleBro;
    //delete all children
    if(eldestChild != 0){
            while(eldestChild->littleBro != 0)
                delete eldestChild->littleBro;
        delete eldestChild;
    }
    //debug("Node ["<<tag<<"] delete!")
}

/// @brief output
void Node::print_info(bool recur){
    cout<<"DISPLAY Node ["<<tag<<"]\'s attributes:"<<endl;
    for(info_p iter = attributes.begin();
        iter != attributes.end(); ++iter)
    {
        cout<<"    "<<iter->first<<": "<<iter->second<<endl;
    }
    if(contents != string("")){
        cout<<"DISPLAY Node ["<<tag<<"]\'s contents:"<<endl;
        cout<<"    "<< contents <<endl;
    }
    cout<<"END OF DESPLAY"<<endl;
    if(recur){
        if(eldestChild !=0) eldestChild->print_info(true);
        if(littleBro != 0) littleBro->print_info(true);
    }

}

/// @brief add or change attribute's value
void Node::setAttr(string attr, string val){
    attributes[attr] = val;
}
void Node::setContent(string cont){
    contents += cont;
}
///
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/// @brief constructor
XML::XML(char * const & xmlsrc): Node(string("xml")),
    src(new char[1]),readp(0),putp(buffer), root(0)
{
    std::fstream xmlin;
    xmlin.open(xmlsrc, std::ios::binary|std::ios::in);
    if(!xmlin){
        debug("can not open the file!")
        return;
    }
    delete src;
    xmlin.seekg(0, std::ios::end);
    int size = xmlin.tellg();
    xmlin.seekg(0, std::ios::beg);
    src = new char[size];
    xmlin.read(src, size);//read file;
    xmlin.close();
    readp = src;
    
    while(*(readp++) != '<');//tag not start
    buf_node();
    if(putattr != string("?xml")){
        debug("It's not XML file")
        return;
    }

    while(*readp != '?' || *(readp+1) != '>')//tag not close
    {
    buf_attr();
    buf_val();
    attributes[putattr] = string(buffer);
    while(*(++readp)==' ');//get to next non-space
    }
    readp += 2;
    
    //read root node
    while(*(readp++) != '<');
    buf_node();
    root = new Node(putattr);
    tags.push_back(putattr);//add root tag
    num.push_back(1);//increase tag number
    stack.push_back(root);  //current tag
    debug( "add node:"<<(root->Tag()))
    read_node();
    readp += 1;
    //
    construct();
    print_info();
}

/// @brief destructor
XML::~XML(){
    delete root;
    delete src;
}

/// @brief construct parsing tree
int XML::construct(){
    while(stack.size()>0){//root tag not closing
        
        if(buf_cont()) stack.back()->setContent(putattr);
        ++readp;
        if(*readp == '/'){//closing tag
            ++readp;
            buf_node();
            if( putattr != stack.back()->Tag())
                debug("closing error: not "<< putattr)
            //debug("close node:"<<(stack.back()->Tag()))
            stack.pop_back();
            ++readp;//move out of '>'
        }else{          //opening tag
            buf_node();
            int i = find(tags, putattr);
            if(i == -1){//add new tag
                tags.push_back(putattr);
                num.push_back(1);
                debug("add tag: "<<tags.back())
            }else
                debug(putattr<<"->"<<++num[i]);//increase the number of this kind of tag
            stack.push_back(new Node(putattr, stack.back()));//open new node
            //debug( "open node:"<<(stack.back()->Tag()))
            if(*readp != '/')
                read_node();
            if(*readp == '/'){
                //debug("close node:"<<stack.back()->Tag())
                stack.pop_back();
                readp += 2;
            }else
                readp += 1;//move out of '>'
        }
    }
    
    
    return tags.size();
}

void XML::read_node(){
    while(*readp != '>' && *readp != '/')//tag not close
    {
        buf_attr();
        buf_val();
        stack.back()->setAttr(putattr, string(buffer));
        while(*(++readp)==' ');//get to next non-space
    }
    

}
void XML::buf_node(){
    while(*readp==' ') ++readp;
    putp = buffer;
    do
        *(putp++) = *(readp++);
    while(*readp != ' ' && *readp != '>' && *readp != '/');
    *putp = '\0';
    putattr = string(buffer);
    
}
void XML::buf_attr(){
    while(*readp==' ') ++readp;
    putp = buffer;
    do
        *(putp++) = *(readp++);
    while(*readp != '=' && *readp != ' ');
    *putp = '\0';
    putattr = string(buffer);
    //debug("attr:"<<putattr)
    
}
void XML::buf_val(){
    while(*(readp++) != '=');
    putp = buffer;
    while(*(readp++)!='\"');
    while(*readp != '\"')
        *(putp++) = *(readp++);
    ;
    *putp = '\0';
}
bool XML::buf_cont(){
    
    if(*readp == '<') return false;
    putp = buffer;
    do{
        if(*readp == '\n')
            *(putp++) = ' ';
        else
            *(putp++) = *readp;
    }while(*(++readp) != '<');
    *putp = '\0';
    putattr = string(buffer);
    //debug("attr:"<<putattr)
    return true;
}
void XML::print_info(bool flag){
    
    Node::print_info();
    cout<<"There's "<<tags.size()<<" kinds of nodes in all:"<<endl;
    for(int i=0;i<tags.size();++i){
        cout<<"    "<<tags[i]<<" : "<<num[i]<<endl;
    }
    root->print_info(flag);
}

///
////////////////////////////////////////////////////////////

//******** test ***********
int main(int argc, char *argv[]){
    
/*    char test[] =   "<?xml version = \"1.0\"  encoding = \"UTF-8\"?>\n"
    "<root name = \"father\" age=\"45\">"
    "<bar width= \"120\">\n love you"
    "<note pitch = \"88\" duration = \"12\">lyrics</note>"
    "! hate you?</bar><bar></bar><note/>"
    "<note pitch = \"100\" duration = \"9\" />"
    "</root>"
    ;
 */
    char xmlsrc[] = "../scoreV3.xml";

    if(argc>1)
        XML xml(argv[1]);
    else
        XML xml(xmlsrc);
    
    return 0;
    
}




