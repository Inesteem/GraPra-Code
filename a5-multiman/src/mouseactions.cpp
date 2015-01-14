#include "mouseactions.h"

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
}
