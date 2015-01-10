#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "drawelement.h"

#include "objloader.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
using namespace std;


struct Obj {

    Obj(string name, int id, string filename, shader_ref shader, float scale);

    vector<drawelement*> *drawelements;

    int id;
    string name;
    vec3f bb_min;
    vec3f bb_max;


};

class ObjHandler{
public:
    ObjHandler();

    void addObj(string name, string filename, shader_ref shader, float scale);
    Obj *getObjByName(string name);
    Obj getObjByID(int id);
private:

    vector<Obj> objs;
};



class GameObject
{
public:

    void update();
    void draw();
      void multiply_model_matrix(matrix4x4f other);


protected:
    GameObject(Obj *obj, std::string name, shader_ref shader);
    string m_name;
    char identifier;
    Obj *m_obj;
    matrix4x4f m_model;
    vec2i m_pos;
    shader_ref m_shader;
    vector<texture_ref> textures;
};

class Tree: public GameObject{
public:
    Tree(Obj *obj, string name , int x, int y);
};

class Building:public GameObject{
public:
    Building(Obj *obj, string name, int x, int y, unsigned int owner );
private:
    unsigned int m_owner;
};

#endif // GAMEOBJECT_H