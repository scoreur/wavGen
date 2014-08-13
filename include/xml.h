/////////////////////////////////////////////////////////////////
/// @file xml.h
///
/// @brief provide simple xml analysis
///
/// for constructing score 3.0 based on xml.\n
/// support double quotation mark only and treat "</","/>" as a whole.\n
/// -update history:\n
///  -# Created  on 8/13/2014.
///
/// @author wyj, https://github.com/scoreur
/////////////////////////////////////////////////////////////////

#ifndef ____xml__
#define ____xml__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#define debug(XXX)  std::cout<< XXX <<std::endl;

const int BUFFERSIZE = 100;

class Node{
    
protected:
    std::string tag;    //info
    std::map<std::string, std::string> attributes;
    std::string contents;   //currently allow sigle block ONLY
    
    Node *parent;       // =0 for root node
    Node *elderBro;     // =0 for first child
    Node *littleBro;    // =0 for last child
    Node *eldestChild;  // =0 for leaf node
    
public:
    Node(std::string _tag = std::string(""), Node *p = 0);
    ~Node();
    void setAttr(std::string, std::string);
    void setContent(std::string);
    virtual void print_info(bool recur = false);
    std::string Tag(int a = 0){return tag;}
    virtual void toFile(std::fstream &xmlout);
};


class XML : public Node{
    
public:
    XML(char * const & xmlsrc);   //read file
    ~XML();
    void print_info(bool = false);  //< display all nodes' info recursively or not
    void toFile(char* const &xmlsrc);      //< output to xml file
    int construct();                //< construct xml tree, return how many tags
    void clear(bool flag = false);  //< delete src, optional: clear the whole tree
private:
    std::vector<std::string>tags;   //< store the tag names
    std::vector<int>num;            //< store the number of each kind of tag
    std::vector<Node *>stack; //< store the current node relations
    char buffer[BUFFERSIZE];        //< store temporary contents to write
    std::string putattr;            //< store the current attribute name
    char * src;                     //< store the start of xml
    char * readp;                   //< read-pointer
    char * putp;                    //< buffer write-pointer
    Node * root;                    //< root node of xml tree
    
    void buf_attr();                //< put attribute into buffer
    void buf_val();                 //< put value into buffer
    void buf_node();                //< put node tag into buffer
    bool buf_cont();                //< put contents into buffer
    void read_node();
    
    
    
};

#endif /* defined(____xml__) */
