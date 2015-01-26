#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "drawelement.h"
#include "simple_heightmap.h"
#include "objloader.h"
#include "wall-timer.h"
#include "effect.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>


using namespace std;

class Label;

struct Obj {

    Obj(string name, int id, string filename, shader_ref shader);
    Obj(string name, int id, string filename, shader_ref shader, int changes);
    Obj(string name, int id, string filename, shader_ref shader, vec3f scale);
    Obj(string name, int id, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max);
    Obj(string name, int id, mesh_ref mesh, texture_ref tex, shader_ref shader);
    Obj(string name, int id, vector<string> filenames, shader_ref shader);
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
    void addObj_changeable(string name, string filename, shader_ref shader, int changes);
    void addObj(string name, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max);
    //adds .obj with custom scale
    void addObj_withScale(string name, string filename, shader_ref shader, vec3f scale);
    void addMeshObj(string name, mesh_ref mesh, shader_ref shader, texture_ref tex);
    void makeObjFMS(vector<string> filenames, string name, shader_ref shader);
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
    vec3f get_center(){return m_center;}
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
    ~Building();

    void upgrade(Obj *obj, int state);
    float dist_to(vec3f &pos);
    void draw();
    void draw_selection_circle();
    void draw_state_house_1();
    void draw_state_turret_1();
    void draw_selection_circle(int size);
    void change_size(int size);
	unsigned int get_owner_id();
	unsigned int get_id();
	void update_unit_count(int count);
	int get_unit_count();
	int get_state();
	int get_type();
	int get_level();
    void change_owner(unsigned int owner);
    bool check_for_upgrade_turret(int state);
    bool check_for_upgrade_settlement(int state);


    int m_size;


private:
    Obj *selection_circle;
    Obj *upgrade_arrow;
    matrix4x4f arrow_model;
    Label *label;
    
    bool settlement, turret;
    
    unsigned int unit_count;
    unsigned int id;
    int m_owner;
    int state;
    
    // upgrade effect for building
    UpgradeEffect *m_upgradeEffect;
};


class Unit {

public:
    Unit(vec2f pos, vec2f view_dir, vec2f pos_group, vec2f start, vec2f end, simple_heightmap *sh, float base_height);
    matrix4x4f *getModel();
    void update(vec2f new_pos,float height);
        vec2f m_pos_group;
          bool move = false;
private:
    wall_time_timer movement_timer;
//    wall_time_timer rotations_timer;
    simple_heightmap* m_sh;
    vec2f m_pos, m_view_dir, m_start, m_end;
    float m_angle = 0;
    float m_speed;
    const float BASE_SPEED = 0.1;
    float m_base_height;
    float m_up_speed;
    float m_dest_height;
    float m_cur_height;
    float m_rot_speed;
    bool last_step;

    matrix4x4f m_model;

};


class UnitGroup: public GameObject{
public:

    UnitGroup(Obj *obj,simple_heightmap *sh, string name, vec2f start, vec2f end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height, unsigned m_id, float scale);
    void update();
    void draw_drawelement();
    void draw_mesh();
    void move_to(vec2f pos, float time_to_reach);
    void force_position(vec2f pos);
    float get_height(float x, float y){
        m_sh->get_height(x,y);
    }

    unsigned int m_id;
    unsigned int m_unit_count;

    bool reached_dest = false;
private:
     void spawn_unit_row(unsigned int size);
     void remove_unit_row();
     void update_model_matrices();
     void update_cur_heights();
     void update_dest_heights();
     void update_units();
     unsigned int time_to_spawn = 100;

    unsigned int m_spawned;
    unsigned int m_owner;
    unsigned int m_rows = 0;
    vector<unsigned int> m_row_size;
    bool m_reached = false;
    bool move = false;
    simple_heightmap *m_sh;
    wall_time_timer m_timer;
    wall_time_timer m_another_timer;
    wall_time_timer m_spawn_timer;
    vec2f m_start,m_end;
    vec2f m_view_dir;
    vec2f m_start_b, m_end_b;
    float m_time_to_reach_end;
    vector<matrix4x4f> m_modelmatrices;
    vector<float> m_dest_heights;
    vector<float> m_cur_heights;
    vector<float> m_up_speed;
    float m_scale;
    vector<Unit> m_units;

};

class Pacman: public GameObject{
public:

    Pacman(Obj *obj, unsigned int owner, unsigned m_id, int height, float time);
	
	void draw();
	
private:
	float time;
	unsigned int owner;
	unsigned int m_id;

};




#endif // GAMEOBJECT_H
