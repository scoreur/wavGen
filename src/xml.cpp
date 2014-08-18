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
#define debug(XXX)  //std::cout<< XXX <<std::endl;

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
void Node::toFile(std::fstream &xmlout){
    xmlout<<"<"<<tag<<" ";
    for(info_p iter = attributes.begin();
        iter != attributes.end(); ++iter)
    {
        xmlout<<iter->first<<"=\""<<iter->second<<"\" ";
    }
    xmlout<<">";
    if(contents != string("")){
        xmlout<<contents;
    }
    if(eldestChild !=0) eldestChild->toFile(xmlout);
    xmlout<<"</"<<tag<<">";
    if(littleBro != 0) littleBro->toFile(xmlout);
    

    
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
    
    //
    construct();
    print_info(true);
}

/// @brief destructor
XML::~XML(){
    delete root;
    delete src;
}

/// @brief construct parsing tree
int XML::construct(){
    readp = src;
    while(*(readp++) != '<');//tag not start
    buf_node();
    if(putattr != string("?xml")){
        cout<<"It's not XML file"<<endl;
        return 0;
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

    while(stack.size()>0){//root tag not closing
        if(buf_cont()) stack.back()->setContent(putattr);
        ++readp;
        while( *readp =='!'){//opposite: sole '<'
            //skip comments (simple kmp)
            readp += 3;//!--
            int f=0;
            while(f<3){
                if(f==-1 || *readp=='-'){
                    ++f; ++readp;
                    if(f==2 && *readp=='>')
                        ++f;
                }else
                    f=-1;
            }
            ++readp;
            debug("skip comments")
        while(*(readp++)!='<');
        }
        if(*readp == '/'){//closing tag
            ++readp;
            buf_node();
            if( putattr != stack.back()->Tag()){
                cout<<"closing error: NOT "<< putattr<<endl;
                return 0;
            }
            debug("close node:"<<(stack.back()->Tag()))
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
                ++num[i];//increase the number of this kind of tag
            stack.push_back(new Node(putattr, stack.back()));//open new node
            debug( "open node:"<<(stack.back()->Tag()))
            if(*readp != '/' && *readp != '>')
                read_node();
            if(*readp == '/'){
                debug("close node:"<<stack.back()->Tag())
                stack.pop_back();
                readp += 2;
            }else
                readp += 1;//move out of '>'
        }
    }
    return tags.size();
}
void XML::clear(bool flag){
    
    delete src;
    src = new char[1];
    readp = 0;
    putattr = string("");
    if(flag){
        delete root;
        root = 0;
        tags.clear();
        num.clear();
    }
}

void XML::read_node(){
    while(*readp != '>' && *readp != '/')//tag not close
    {
        buf_attr();
        buf_val(); ++readp;
        stack.back()->setAttr(putattr, string(buffer));
        while(*readp==' ') ++readp;//get to next non-space
    }
    

}
void XML::buf_node(){
    while(*readp==' ' || *readp=='\t') ++readp;
    putp = buffer;
    do
        *(putp++) = *(readp++);
    while(*readp != ' ' && *readp != '>' && (*readp != '/'||*(readp+1)!='>'));
    *putp = '\0';
    putattr = string(buffer);
    debug("node:"<<putattr)
}
void XML::buf_attr(){
    while(*readp==' ' || *readp=='\t') ++readp;
    putp = buffer;
    do
        *(putp++) = *(readp++);
    while(*readp != '=' && *readp != ' ' && *readp!='\t');
    *putp = '\0';
    putattr = string(buffer);
    debug("attribute:"<<putattr)
    
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
    while(*readp == '\n' || *readp == '\t' || *readp == ' ')
        ++readp;
    if(*readp == '<') return false;
    putp = buffer;
    do{
        if(*readp == '\n' || *readp == '\t')
            *(putp++) = ' ';
        else
            *(putp++) = *readp;
    }while(*(++readp) != '<');
    *putp = '\0';
    putattr = string(buffer);
    debug("content:"<<putattr)
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
void XML::toFile(char * const &xmlsrc){
    std::fstream xmlout;
    xmlout.open(xmlsrc, std::ios::out);
    xmlout<<"<?xml version=\""<<attributes[string("version")]
          <<"\" encoding=\""<<attributes[string("encoding")]
          <<"\"?>";
    root->toFile(xmlout);
    
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
    char xmlsrc2[]="../scoreV3_out.xml";

    if(argc>1)
    {XML xml(argv[1]); xml.toFile(xmlsrc2);}
    else{
        XML xml(xmlsrc);
        xml.toFile(xmlsrc2);
    }
    
     
    
    return 0;
    
}




