#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "drawelement.h"
#include "label.h"
#include "simple_heightmap.h"
#include "objloader.h"
#include "wall-timer.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
using namespace std;


struct Obj {

    Obj(string name, int id, string filename, shader_ref shader);
    Obj(string name, int id, string filename, shader_ref shader, vec3f scale);
    vector<drawelement*> *drawelements;

    int id;
    string name;
    vec3f bb_min;
    vec3f bb_max;


};

class ObjHandler{
public:
    ObjHandler();
    //adds .obj and tries to scale to fit map
    void addObj(string name, string filename, shader_ref shader);
    //adds .obj with custom scale
    void addObj_withScale(string name, string filename, shader_ref shader, vec3f scale);
    Obj *getObjByName(string name);
    Obj *getObjByID(int id);
private:

    vector<Obj> objs;
};



class GameObject
{
public:

    void update();
    void draw();
    void set_model_matrix(matrix4x4f new_model);
    matrix4x4f get_model_matrix();
    void set_height(float height);
    vec2i get_pos(){
        return m_pos;
    }

protected:
    GameObject(Obj *obj, std::string name, shader_ref shader, float height);
    string m_name;
    char identifier;
    Obj *m_obj;
    matrix4x4f m_model;
    vec2i m_pos;
    shader_ref m_shader;
    vec3f m_center;
    vector<texture_ref> textures;
    float m_height;

};

class Tree: public GameObject{
public:
    Tree(Obj *obj, string name , int x, int y, float height);
};

class Building:public GameObject{
public:
Label label;
    Building(Obj *obj, string name, int x, int y, unsigned int owner, int size, float height );
    void upgrade();

	void draw_label();

private:
    unsigned int m_owner;
    int m_size;
    
    
};

class UnitGroup: public GameObject{
public:

    UnitGroup(Obj *obj,simple_heightmap *sh, string name, vec2i start, vec2i end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height);
    void update();
    void draw();
    void move_to(vec2i pos, float time_to_reach);
    void force_position(vec2i pos);
    void update_model_matrices();
private:
    const unsigned int TIME_TO_SPAWN = 100;

    unsigned int m_unit_count;
    unsigned int m_spawned;
    unsigned int m_owner;


    simple_heightmap *m_sh;
    wall_time_timer m_timer;
    wall_time_timer m_spawn_timer;
    vec2i m_start,m_end;
    vec2f m_view_dir;
    float m_time_to_reach_end;
    vector<matrix4x4f> m_modelmatrices;


};

#endif // GAMEOBJECT_H
