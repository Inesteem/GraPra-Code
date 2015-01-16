#include "mouseactions.h"
#include "clientside-networking.h"
#include "label.h"
#include "gameobject.h"
#include "game.h"

namespace moac {
	
	void getScreenpos(vec2f* in,vec2f* out){

		float w = (float)render_settings::screenres_x;
		float h = (float)render_settings::screenres_y;			
		camera_ref cam = current_camera();
		matrix4x4f proj = *(projection_matrix_of_cam(cam));
		matrix4x4f view = *(gl_view_matrix_of_cam(cam));
		matrix4x4f vp = proj * view;		

		vec4f v(in->x,0,in->y,1);
		vec4f v_screen;
		multiply_matrix4x4f_vec4f(&v_screen,&vp,&v);	
		v_screen = v_screen / v_screen.w;

		v_screen.x = (v_screen.x +1)/2 * w;
		v_screen.y = (1-v_screen.y )/2 * h;

		out->x = v_screen.x; 
		out->y = v_screen.y; 

	}

    vec3f ClickWorldPosition(int screenX, int screenY)
    {
        int *viewport = new int[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        camera_ref cam = current_camera();
        matrix4x4f *view = gl_view_matrix_of_cam(cam);
        matrix4x4f *projection = projection_matrix_of_cam(cam);
        float *fProj = projection->col_major;
        float *fView = view->col_major;
        double *dProj = new double[16];
        double *dView = new double[16];
        for(int i = 0; i < 16; i++) {
            dProj[i] = (double) fProj[i];
            dView[i] = (double) fView[i];
        }

        GLfloat winX = (float) screenX;
        GLfloat winY = (float) viewport[3] - (float) screenY;
        GLfloat winZ;
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        GLdouble outX, outY, outZ;
        gluUnProject(winX, winY, winZ, dView, dProj, viewport, &outX, &outY, &outZ);

        return vec3f(outX, outY, outZ);
    }

	Action::Action(Game *game){

		this->game = game;
		eb_set = false;
		ob_set = false;
		prepare_attack = false;
		slidebar = new SlideBar();	
		slidebar->initialize_slidebar();			
		
	}


	bool Action::handle_enemys_base(float x, float y){
		
        vec3f wp = ClickWorldPosition(x,y);
        Building *building = game->get_building_at(wp);
 		//TODO: get own id
        
		//keine Auswahl, falls bereits Gebaeude ausgewaehlt, verfaellt diese Wahl
	//	if (!ob_set || building == nullptr || building->get_owner_id() == own_id){
		if (!ob_set || building == nullptr){
			eb_set = false;
			return false;
		}		
		slidebar->reset_bar();
		eb_set = true;
		enemys_building = building;
		prepare_attack = true;	
		start(x,y);
		return true;	
	}
	
	bool Action::handle_base_selection(float x, float y){
		
        vec3f wp = ClickWorldPosition(x,y);
        Building *building = game->get_building_at(wp);
        
		//TODO: get own id

		//keine Auswahl, falls bereits Gebaeude ausgewaehlt, verfaellt diese Wahl
//		if (building == nullptr || building->get_owner_id() != own_id){
		if (building == nullptr){
			ob_set = false;
			return false;
		}
		
		ob_set = true;
		own_building = building;
		
		return true;
	}
	
	void Action::start(float x, float y){
		slidebar->update_pos(x,y);
		slidebar->set_max_count(own_building->get_unit_count());
	}
	
	void Action::finish(){
		if(prepare_attack){
			int units = slidebar->get_unit_count();
			if(units != 0){
				
				msg::spawn_troup_client stc = make_message<msg::spawn_troup_client>();
				// TODO use own player id
				stc.playerId = 0;
				stc.sourceId = own_building->get_id();
				stc.destinationId = enemys_building->get_id();
				stc.unitCount = units;
				game->m_messageReader->send_message(stc);
				slidebar->dec_max_count(units);
				cout << "owner: (" << own_building->get_pos().x << " ' " << own_building->get_pos().y << ")" << endl;
				cout << "enemy: (" << enemys_building->get_pos().x << " ' " << enemys_building->get_pos().y << ")" << endl;
				cout << "troups: (" << units << ")" << endl;
			}
			slidebar->reset_bar();
			prepare_attack = false;
		}
	}

	void Action::draw(){
		if(prepare_attack)
			slidebar->render_slidebar();		
	}
	
	
	void Action::update_mouse_pos(float x, float y){
		if(prepare_attack)
			slidebar->update_mouse_pos(x,y);	
	}

}


