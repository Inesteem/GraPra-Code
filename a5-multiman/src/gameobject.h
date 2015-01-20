#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "drawelement.h"
#include "simple_heightmap.h"
#include "objloader.h"
#include "wall-timer.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
using namespace std;

class Label;

struct Obj {

    Obj(string name, int id, string filename, shader_ref shader);
    Obj(string name, int id, string filename, shader_ref shader, vec3f scale);
    Obj(string name, int id, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max);
    Obj(string name, int id, mesh_ref mesh, texture_ref tex, shader_ref shader);

    vector<drawelement*> *drawelements;
    mesh_ref mesh;
    texture_ref tex;
    int id;
    string name;
    vec3f bb_min;
    vec3f bb_max;
    shader_ref shader;


};

class ObjHandler{
public:
    ObjHandler();
    //adds .obj and tries to scale to fit map
    void addObj(string name, string filename, shader_ref shader);
    void addObj(string name, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max);
    //adds .obj with custom scale
    void addObj_withScale(string name, string filename, shader_ref shader, vec3f scale);
    void addMeshObj(string name, mesh_ref mesh, shader_ref shader, texture_ref tex);
    Obj *getObjByName(string name);
    Obj *getObjByID(int id);
    Obj *get_selection_circle();
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
    vec2f get_pos(){
        return m_pos;
    }

protected:
    GameObject(Obj *obj, std::string name, shader_ref shader, float height);
    string m_name;
    char identifier;
    Obj *m_obj;
    matrix4x4f m_model;
    vec2f m_pos;
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
    Building(Obj *obj, Obj *selection_circle, Obj *upgrade_arrow, string name, int x, int y, unsigned int owner, int size, float height, unsigned int id);
    void upgrade(Obj *obj, int state);
    float dist_to(vec3f &pos);
    void draw();
    void draw_selection_circle();
    void draw_state_1();
    void draw_selection_circle(int size);
	unsigned int get_owner_id();
	unsigned int get_id();
	void update_unit_count(int count);
	int get_unit_count();
	int get_state();
    void change_owner(unsigned int owner);
    bool check_for_upgrade(bool up);

    int m_size;

private:
    int m_owner;
    unsigned int unit_count;
    int state;
    Obj *selection_circle;
    Obj *upgrade_arrow;
    matrix4x4f arrow_model;
    Label *label;
    
    unsigned int id;
    
};

class UnitGroup: public GameObject{
public:

    UnitGroup(Obj *obj,simple_heightmap *sh, string name, vec2f start, vec2f end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height, unsigned m_id);
    void update();
    void draw();
    void move_to(vec2f pos, float time_to_reach);
    void force_position(vec2f pos);

    unsigned int m_id;
    unsigned int m_unit_count;

    bool reached_dest = false;
private:
     void spawn_unit_row(unsigned int size);
     void remove_unit_row();
     void update_model_matrices();
     void update_cur_heights();
     void update_dest_heights();
     unsigned int time_to_spawn = 100;

    unsigned int m_spawned;
    unsigned int m_owner;
    unsigned int m_rows = 0;
    vector<unsigned int> m_row_size;
    bool m_reached = false;

    simple_heightmap *m_sh;
    wall_time_timer m_timer;
    wall_time_timer m_spawn_timer;
    vec2f m_start,m_end;
    vec2f m_view_dir;
    float m_time_to_reach_end;
    vector<matrix4x4f> m_modelmatrices;
    vector<float> m_dest_heights;
    vector<float> m_cur_heights;
    vector<float> m_up_speed;

};



#endif // GAMEOBJECT_H
