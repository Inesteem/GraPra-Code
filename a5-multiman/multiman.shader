;; just a few default shaders.

#<make-shader "pos-only"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos,1.);
	}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	void main() {
		out_col = vec4(1.);
	}
}
#:inputs (list "in_pos")>

#<make-shader "compute-shader"
#:compute-shader #{
#version 430 core
        layout (local_size_x = 16, local_size_y = 16) in;
        uniform float height;
        uniform float radius;
        uniform float tile_size_x;
        uniform float tile_size_y;
        uniform float g_height;
        uniform float g_width;
        uniform ivec2 pos;
        uniform layout(rgba8) image2D height_map;

        bool hit(ivec2 hit){
            ivec2 t = imageSize(height_map);
            float offset = (t/64)*radius;
            if(hit.x > pos.x + offset/2 + offset ) return false;
            if(hit.x < pos.x + offset/2 - offset) return false;
            if(hit.y > pos.y + offset/2 + offset ) return false;
            if(hit.y < pos.y + offset/2 - offset) return false;
            return true;
        }
        bool outerhit(ivec2 hit){
            float offset = radius*15;
            if(hit.x > pos.x + offset/2 + offset + offset ) return false;
            if(hit.x < pos.x + offset/2 - offset + offset) return false;
            if(hit.y > pos.y + offset/2 + offset + offset) return false;
            if(hit.y < pos.y + offset/2 - offset + offset) return false;
            return true;
        }

        void main() {
            ivec2 storePos = ivec2(gl_GlobalInvocationID.yx);
            ivec2 size = imageSize(height_map);
            if( hit(storePos) ){
//                ivec2 t = imageSize(height_map);
                imageStore(height_map, storePos.yx, vec4(height, height, height, 1));
            }
        }
}

#:inputs (list "")>


#<make-shader "pos+norm"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec3 in_norm;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	uniform mat4 model_normal;
	out vec4 pos_wc;
	out vec3 norm_wc;
	void main() {
		pos_wc = model * vec4(in_pos, 1.0);
		norm_wc = (model_normal * vec4(in_norm,0)).xyz;
		gl_Position = proj * view * pos_wc;
	}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	uniform vec3 color;
	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 eye_pos;
	in vec4 pos_wc;
	in vec3 norm_wc;
	void main() {
		out_col = vec4(0.,0.,0.,1.);

		float n_dot_l = max(0, dot(norm_wc, -light_dir));
		out_col += vec4(color * light_col * n_dot_l, 0.);
	}
}
#:inputs (list "in_pos" "in_norm")>

#<make-shader "particle-flare-shader"
#:vertex-shader #{
#version 150 core
        in vec3 in_pos;
        in float in_lifetime;
        uniform mat4 proj;
        uniform mat4 view;
        uniform vec2 screenres;
        uniform float particle_size;
        uniform float max_lifetime;
        uniform vec2 near_far;
        out float factor;
// 	out float depth;

        float compute_pointsize(float light_radius, vec4 position)
        {
                // see bf::splats.

                float d = position.z;

                mat4 m = proj;
                vec4 v = vec4(light_radius, light_radius, d, 1.0);

                vec4 tmp = vec4(v.x*m[0].x, v.y*m[1].y, v.z*m[2].z+m[3].z, -v.z);
                vec3 tmp2 = tmp.xyz / tmp.w;
                tmp2.x = tmp2.x * screenres.x;
                tmp2.y = tmp2.y * screenres.y;

                float size = tmp2.x * 1.15;
                return size;
        }

        void main() {
                vec4 pos_ec = view * vec4(in_pos,1.0);
// 		depth = (-pos_ec.z - near_far.x) / (near_far.y - near_far.x);
                gl_Position = proj * pos_ec;
                if (in_lifetime <= 0.0)
                        gl_PointSize = 0.0;
                else
                        gl_PointSize = compute_pointsize(particle_size, pos_ec);
                factor = max(0.0 , in_lifetime / max_lifetime);
        }
}
#:fragment-shader #{
#version 150 core
        in float factor;
// 	in float depth;
        out vec4 out_col;
        uniform sampler2D depthtex;
        uniform vec2 screenres;
        uniform vec2 near_far;
        uniform vec3 color;
        float LinearDepth(in float depth, in float near, in float far) { return (2.0 * near) / (far + near - depth * (far - near)); }
        void main() {
            //out_col = vec4(1,0,0,1);

                vec2 pos = (gl_PointCoord.xy-vec2(0.5,0.5)) * 2.0;
                float val = length(pos);
                out_col = max(0., 1. - val) * vec4(color, 1.);
                out_col *= max(0., pow(16. , -val));
                //float d = texture(depthtex, gl_FragCoord.st/screenres).r;
                //d = LinearDepth(d, near_far.x, near_far.y);
                //float depth = LinearDepth(gl_FragCoord.z, near_far.x, near_far.y);
                const float fade_dist = 0.005;
                //float dist = clamp(d-depth, 0, fade_dist);
                //out_col = mix(vec4(0,0,0,0), out_col, dist/fade_dist);
                //gl_FragDepth = depth;
        }
}
#:inputs (list "in_pos" "in_lifetime")>

#<make-shader "selection_circle_shader"
#:vertex-shader #{
#version 150 core
        in vec3 in_pos;
        in vec2 in_tc;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        out vec2 tc;
        void main() {
                gl_Position = proj * view * model * vec4(in_pos,1.);

                tc = in_tc;
        }
}

#:fragment-shader #{
#version 150 core
        in vec2 tc;
        uniform vec3 color;
        out vec4 out_col;
        uniform sampler2D tex;

        void main() {
            vec4 tex_col = texture(tex,tc);


          if(tex_col.r >= 0.5 && tex_col.g >= 0.5 && tex_col.b >= 0.5){
                out_col = vec4(color,0.5);
				gl_FragDepth = 0.3;
          }
            else if(!(tex_col.r >= 0.9 || tex_col.g <= 0.1 || tex_col.b >= 0.9))  {
                out_col = tex_col;
          } else {

                 discard;
          }

        }
}
#:inputs (list "in_pos" "in_tc")>


#<make-shader "pos+norm+tc"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec3 in_norm;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	uniform mat4 model_normal;
	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		pos_wc = model * vec4(in_pos, 1.0);
		norm_wc = (model_normal * vec4(in_norm,0)).xyz;
		tc = in_tc;
		gl_Position = proj * view * pos_wc;
	}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	uniform sampler2D diffuse_tex;
	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 eye_pos;
	in vec4 pos_wc;
	in vec3 norm_wc;
	in vec2 tc;
	void main() {
		out_col = vec4(0.,0.,0.,1.);
		vec3 color = texture(diffuse_tex, tc.st).rgb;

		float n_dot_l = max(0, dot(norm_wc, -light_dir));
		out_col += vec4(color * light_col * n_dot_l, 0.);
//		out_col = vec4(color.r,color.g,color.b,1);
//		if(out_col.x <= 0.2 && out_col.y <= 0.2 &&out_col.z <= 0.2)
//			discard;
	}
}
#:inputs (list "in_pos" "in_norm" "in_tc")>


#<make-shader "unit-shader"
#:vertex-shader #{
#version 150 core
        in vec3 in_pos;
        in vec3 in_norm;
        in vec2 in_tc;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        uniform mat4 model_normal;
        out vec4 pos_wc;
        out vec3 norm_wc;
        out vec2 tc;
        void main() {
                pos_wc = model * vec4(in_pos, 1.0);
// 		norm_wc = transpose(inverse(mat3x3(model))) * in_norm;
                norm_wc = (model_normal * vec4(in_norm,0)).xyz;
                tc = in_tc;
                gl_Position = proj * view * pos_wc;

        }
}
#:fragment-shader #{
#version 150 core
        out vec4 out_col;
        uniform sampler2D diffuse_tex;
        uniform vec3 light_dir;
        uniform vec3 light_col;
        uniform vec3 eye_pos;
        uniform vec3 p_color = vec3(1,1,1);
        in vec4 pos_wc;
        in vec3 norm_wc;
        in vec2 tc;
        void main() {
                out_col = vec4(0.,0.,0.,1.);
                vec3 color = texture(diffuse_tex, tc.st).rgb;

                float n_dot_l = max(0, dot(norm_wc, -light_dir));
                out_col += vec4(p_color*color * light_col * n_dot_l, 0.);
//		out_col = vec4(color.r,color.g,color.b,1);
//		if(out_col.x <= 0.2 && out_col.y <= 0.2 &&out_col.z <= 0.2)
//			discard;
        }
}
#:inputs (list "in_pos" "in_norm" "in_tc")>


#<make-shader "text-shader"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 tc;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos,1.);
		tc = in_tc;
	}
}
#:fragment-shader #{
#version 150 core
	in vec2 tc;
	uniform vec3 color;
	out vec4 out_col;
	uniform sampler2D tex;
	void main() {


        gl_FragDepth = 0.001;

        if(texture(tex, tc).r >= 0.5 || texture(tex, tc).g >= 0.5 || texture(tex, tc).b >= 0.5)
             out_col = vec4(color.x, color.y, color.z, 1.);
         else
             discard;
		
	}
}
#:inputs (list "in_pos" "in_tc")>

#<make-shader "heightmap_shader"
#:vertex-shader #{
#version 400 core
        in vec3 in_pos;
        in vec3 in_norm;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        out vec3 out_pos;
        void main() {
                gl_Position =  vec4(in_pos,1.);
                out_pos = in_pos;
        }
}
#:tess-control-shader #{
#version 400 core


        layout(vertices = 4) out;
        in vec3 out_pos[];
        out vec3 tcPosition[];

        uniform mat4 view;
        uniform mat4 model;
        uniform float lod = 0.1;
        #define ID gl_InvocationID

        float level(float d){
                return clamp(lod *2000/d, 1, 64);
        }
        void main()
        {

            tcPosition[ID] = out_pos[ID];

            vec3 pos = out_pos[ID];

            if(ID == 0){
//            float dis = length(view*model*vec4(tcPosition[0],1));

            float d0 = length(view*model*vec4(tcPosition[0],1));
            float d1 = length(view*model*vec4(tcPosition[1],1));
            float d2 = length(view*model*vec4(tcPosition[2],1));
            float d3 = length(view*model*vec4(tcPosition[3],1));



                gl_TessLevelOuter[0] = level(mix(d3,d0,.5));
                gl_TessLevelOuter[1] = level(mix(d0,d1,.5));
                gl_TessLevelOuter[2] = level(mix(d1,d2,.5));
                gl_TessLevelOuter[3] = level(mix(d2,d3,.5));
                float l = max(max(gl_TessLevelOuter[0],gl_TessLevelOuter[1]),max(gl_TessLevelOuter[2],gl_TessLevelOuter[3]));
                 gl_TessLevelInner[0] = l;
                 gl_TessLevelInner[1] = l;


        }
        }
}
#:tess-eval-shader #{
#version 400 core


        layout(quads, equal_spacing, ccw) in;
        in vec3 tcPosition[];
        out vec3 out_pos;
        out vec3 tePatchDistance;

        uniform vec2 tex_res;
        uniform mat4 proj;
        uniform mat4 model;
        uniform mat4 view;
        uniform sampler2D height_map;
        vec3 interpolate(vec3 bl, vec3 br, vec3 tr, vec3 tl){
                float u = gl_TessCoord.x;
                float v = gl_TessCoord.y;

                vec3 b = mix(bl,br,u);
                vec3 t = mix(tl,tr,u);
                return mix(b,t,v);
        }
        void main()
        {
//            vec3 p0 = gl_TessCoord.x * tcPosition[0];
//            vec3 p1 = gl_TessCoord.y * tcPosition[1];
//            vec3 p2 = gl_TessCoord.z * tcPosition[2];
            tePatchDistance = gl_TessCoord;

            out_pos = interpolate(tcPosition[0],tcPosition[1],tcPosition[2],tcPosition[3]);
            out_pos.y = texture(height_map,vec2(out_pos.z, out_pos.x));
            out_pos = (model*vec4(out_pos, 1)).xyz;
            gl_Position = proj * view * vec4(out_pos, 1);
        }


}
#:geometry-shader #{
#version 400 core

        //used to calculate normals
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;
        layout(triangles) in;
        layout(triangle_strip, max_vertices = 3) out;
        in vec3 out_pos[3];
        in vec3 tePatchDistance[3];
         out vec3 gFacetNormal;
        out vec3 gPatchDistance;
        out vec3 gTriDistance;
        out vec3 pos;

        void main()
        {
            mat3 tmp1 = mat3(model);
            mat3 tmp2 = mat3(view);
            mat3 normal = tmp2 * tmp1;
            normal = transpose(inverse(mat3x3(view*model)));
            vec3 A = out_pos[2] - out_pos[0];
            vec3 B = out_pos[1] - out_pos[0];
            gFacetNormal = normal * normalize(cross(A, B));


            gPatchDistance = tePatchDistance[0];
            gTriDistance = vec3(1, 0, 0);
            gl_Position = gl_in[0].gl_Position;
            pos = out_pos[0]; EmitVertex();

//            A = out_pos[0] - out_pos[1];
//            B = out_pos[2] - out_pos[1];
//            gFacetNormal = normal * normalize(cross(A, B));
            gPatchDistance = tePatchDistance[1];
            gTriDistance = vec3(0, 1, 0);
            gl_Position = gl_in[1].gl_Position;
            pos = out_pos[1]; EmitVertex();

//            A = out_pos[1] - out_pos[2];
//            B = out_pos[0] - out_pos[2];
//            gFacetNormal = normal * normalize(cross(A, B));
            gPatchDistance = tePatchDistance[2];
            gTriDistance = vec3(0, 0, 1);
            gl_Position = gl_in[2].gl_Position;
            pos = out_pos[2]; EmitVertex();

            EndPrimitive();
        }
}

#:fragment-shader #{
#version 400 core

        in vec3 pos;
        in vec3 tePatchDistance;
        in vec3 gFacetNormal;
        in vec3 gPatchDistance;
        in vec3 gTriDistance;
        uniform sampler2D grass;
        uniform sampler2D stone;
        uniform sampler2D water;
        uniform sampler2D snow;
        uniform float height_factor;
        uniform vec3 light_dir;
        uniform vec3 light_col;
        out vec4 out_col;

        float amplify(float d, float scale, float offset){
            d = scale*d + offset;
            d = clamp(d,0,1);
            d = 1-exp2(-2*d*d);
            return d;
        }

        void main() {
            float border_water = 0.0 * height_factor;
                     float border_water_grass = 0.01 * height_factor;
                     float border_grass = 0.2 * height_factor;
                     float border_grass_rock = 0.4 * height_factor;
                     float border_rock = 0.6 * height_factor;
                     float border_rock_snow = 0.85 * height_factor;
                     vec2 tc = vec2(pos.z/4,pos.x/4);
                     vec4 color_water = vec4(texture(water, tc).rgb, 1.0);
                     vec4 color_grass = vec4(texture(grass, tc).rgb, 1.0);
                     vec4 color_rock = vec4(texture(stone,tc).rgb, 1.0);
                     vec4 color_snow = vec4(texture(snow, tc).rgb, 1.0);


                     if (pos.y <= border_water) {
                             out_col = color_water;
                     } else if (pos.y <= border_water_grass) {
                             out_col = mix(color_water, color_grass, smoothstep(border_water, border_water_grass, pos.y));
                     } else if (pos.y <= border_grass) {
                             out_col = color_grass;
                     } else if (pos.y <= border_grass_rock) {
                             out_col = mix(color_grass, color_rock, smoothstep(border_grass, border_grass_rock, pos.y));
                     } else if (pos.y <= border_rock) {
                             out_col = color_rock;
                     } else if (pos.y <= border_rock_snow) {
                             out_col = mix(color_rock, color_snow, smoothstep(border_rock, border_rock_snow, pos.y));
                     } else {
                             out_col = color_snow;
                     }

                     vec3 N = normalize(gFacetNormal);
                     vec3 L = light_dir;
                     float df = abs(dot(N, L));
//                     float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
//                     float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
                     vec3 color = df*light_col;




                     out_col *= (df)*vec4(color,1);



        }
}
#:inputs (list "in_pos")>

#<make-shader "building-shader"
#:vertex-shader #{
#version 150 core
        in vec3 in_pos;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        out vec3 out_pos;
        void main() {
                gl_Position = proj * view * model * vec4(in_pos,1.);
                out_pos = in_pos;
        }
}
#:fragment-shader #{
#version 150 core

        in vec3 out_pos;
        out vec4 out_col;

        void main() {
            out_col = vec4(0.5,0.5,0.5, 1.0);

        }
}
#:inputs (list "in_pos")>


#<make-shader "tree-shader"
#:vertex-shader #{
#version 150 core
        in vec3 in_pos;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        out vec3 out_pos;
        void main() {
                gl_Position = proj * view * model * vec4(in_pos,1.);
                out_pos = in_pos;
        }
}
#:fragment-shader #{
#version 150 core

        in vec3 out_pos;
        out vec4 out_col;

        void main() {
            out_col = vec4(0.0,1,0, 1.0);

        }
}
#:inputs (list "in_pos")>

#<make-shader "terrain"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec3 in_norm;

	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;

	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		pos_wc = vec4(in_pos, 1.0);
		norm_wc = in_norm;
		tc = vec2(in_pos.x, in_pos.y);
		gl_Position = proj * view * pos_wc;
	}
}
#:fragment-shader #{
#version 150 core
	in vec4 pos_wc;
	in vec3 norm_wc;
	in vec2 tc;
	uniform mat4 T;


	uniform sampler2D water;
	uniform sampler2D grass;
	uniform sampler2D stone;
	uniform sampler2D snow;
	uniform sampler2D shadowmap;

	uniform float map_height;

	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 light_col_water;
	uniform vec3 eye_pos;

	out vec4 out_col;
	void main() {
		//vec3 light_col_water = vec3(.4, .4, .4) * 2;

		vec4 ShadowCoord =  T * pos_wc;
		float visibility = 1.0;
		vec3 tex_coords = ShadowCoord.xyz/ShadowCoord.w;
		float depth = texture(shadowmap, tex_coords.xy).r;
	//	float inShadow = (depth < tex_coords.z) ? 1.0 : 0.0;
		if(depth != 1)
			visibility =0;


		vec3 water_color = texture(water, pos_wc.xz/45.).rgb;
		vec3 grass_color = texture(grass, pos_wc.xz/12.).rgb;
		vec3 stone_color = texture(stone, pos_wc.xz/8.).rgb;
		vec3 snow_color = texture(snow, pos_wc.xz/20).rgb; 
		
		vec3 color;

		//some borders
		//1.5 is supposed to be zero
		//values determined by testing with height 25

		float pure_water  = 0.0/25.0 * map_height;
		float blend_water = 0.5/25.0 * map_height;
		float pure_grass  = 8.5/25.0 * map_height;
		float blend_grass = 21.0/25.0 * map_height;
		float pure_stone  = 21.0/25.0 * map_height;
		float blend_stone = 24.0/25.0 * map_height;

		if (pos_wc.y <= pure_water){
			color = water_color;

		}else if (pos_wc.y < blend_water){
			color = mix(water_color, grass_color, smoothstep(pure_water, blend_water, pos_wc.y));

		}else if (pos_wc.y < pure_grass){
			color = grass_color;

		}else if (pos_wc.y < blend_grass){
			color = mix(grass_color, stone_color, smoothstep(pure_grass, blend_grass, pos_wc.y));


		}else if (pos_wc.y < pure_stone){
			color = stone_color;

		}else if (pos_wc.y < blend_stone){
			color = mix(stone_color, snow_color, smoothstep(pure_stone, blend_stone, pos_wc.y));

		}else{
			color = snow_color;

		}
		vec3 r = reflect(light_dir, norm_wc);
		vec3 spec = vec3(0);
		if(pos_wc.y < blend_water) {
			spec = light_col_water * 0.7 * pow(max(0, dot(r, normalize(eye_pos - pos_wc.xyz))), 10);
		}
		if(pos_wc.y > blend_grass) {
			spec = light_col * 0.4 * pow(max(0, dot(r, normalize(eye_pos - pos_wc.xyz))), 10);
		}
		vec3 diff = color * light_col * max(0, dot(normalize(norm_wc), normalize(-light_dir)));
	//	color = visibility * 0.5 * color + visibility * diff + visibility * spec;
		color = 0.5 * color + diff + spec;
		out_col = vec4(color, 1.0);
	}
}
#:inputs (list "in_pos" "in_norm" "in_tc")>


#<make-shader "ip2-shader"
#:vertex-shader #{
#version 150 core

        in vec3 in_pos_0;
        in vec3 in_pos_1;
        in vec3 in_norm_0;
        in vec3 in_norm_1;
        in vec2 in_tc;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        uniform float time;
        out vec2 tc;
        out vec3 norm_wc;
        out vec3 pos_wc;
        void main() {
            float t = (sin(time/100)+1)/2.0;
            norm_wc = mix(in_norm_0,in_norm_1,t);
            vec3 pos = mix(in_pos_0,in_pos_1,t);
            pos_wc = vec3(model * vec4(pos, 1.0));
            //            pos_wc = model * vec4(in_pos_1, 1.0);
            norm_wc = transpose(inverse(mat3x3(model))) * norm_wc;
//            norm_wc = (model_normal * vec4(in_norm,0)).xyz;
            tc = in_tc;
            gl_Position = proj * view * vec4(pos_wc,1);
        }

}
#:geometry-shader #{
#version 150 core

        //used to calculate normals
        uniform mat4 model;
        uniform mat4 view;
        layout(triangles) in;
        layout(triangle_strip, max_vertices = 3) out;
        in vec3 pos_wc[3];

         out vec3 gFacetNormal;


        out vec3 pos;

        void main()
        {
            mat3 tmp1 = mat3(model);
            mat3 tmp2 = mat3(view);
            mat3 normal = tmp2 * tmp1;
            normal = transpose(inverse(normal));
            vec3 A = pos_wc[2] - pos_wc[0];
            vec3 B = pos_wc[1] - pos_wc[0];
            gFacetNormal = normal * normalize(cross(A, B));


//            gPatchDistance = tePatchDistance[0];
//            gTriDistance = vec3(1, 0, 0);
            gl_Position = gl_in[0].gl_Position;
            pos = pos_wc[0]; EmitVertex();

//            A = out_pos[0] - out_pos[1];
//            B = out_pos[2] - out_pos[1];
//            gFacetNormal = normal * normalize(cross(A, B));

            gl_Position = gl_in[1].gl_Position;
            pos = pos_wc[1]; EmitVertex();

//            A = out_pos[1] - out_pos[2];
//            B = out_pos[0] - out_pos[2];
//            gFacetNormal = normal * normalize(cross(A, B));

            gl_Position = gl_in[2].gl_Position;
            pos = pos_wc[2]; EmitVertex();

            EndPrimitive();
        }
}
#:fragment-shader #{
#version 150 core

        out vec4 out_col;
        uniform sampler2D diffuse_tex;
        uniform vec3 light_dir;
        uniform vec3 light_col;
        uniform vec3 eye_pos;
        uniform vec3 color;
        in vec3 pos;
        in vec3 gFacetNormal;
        in vec2 tc;
        void main() {
                out_col = vec4(0.,0.,0.,1.);
                vec3 color1 = texture(diffuse_tex, tc.st).rgb;
//                out_col = vec4(norm_wc,1);
                float n_dot_l = max(0, dot(gFacetNormal, -light_dir));
                out_col += vec4(color * color1 * light_col * n_dot_l, 0.);
//                out_col = vec4(0,1,1,1);
        }
}

#:inputs (list "in_pos_0" "in_pos_1" "in_tc")>


#<make-shader "count-shader"
#:vertex-shader #{
#version 150 core

	in vec3 in_pos;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 tc;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos, 1.);
		float newy= in_tc.y -1;
		if(newy < 0) newy=-newy;
		vec2 texc = vec2(in_tc.x, newy);
		tc = texc;
	}

}
#:fragment-shader #{
#version 150 core

in vec2 tc;
out vec4 out_col;
uniform sampler2D tex;
uniform float LifeLevel;
uniform float down;

		void main(){
		
			out_col = texture2D(tex, tc );
			out_col.w = 0.3;

			if (out_col.x > 0.5 && out_col.y > 0.5 && out_col.z > 0.5)
				discard;

			else if(down == 0 && LifeLevel > 0){
				if (tc.y < LifeLevel && tc.x > 0.3 && tc.x < 0.7 && tc.y > 0.04 )
					//out_col = vec4(0.2, 0.8, 0.2, .7); // Opaque green
					out_col = vec4(1-tc.y, tc.y, 0., .7); 
			}
			else if(down == 1 && LifeLevel < 0){
				float temp_ll = 1+LifeLevel;
				if (tc.y > temp_ll && tc.x > 0.3 && tc.x < 0.7 && tc.y < 0.96 )
						out_col = vec4(tc.y, 1-tc.y, 0., .7); 
			}
		
		}
			



		}

#:inputs (list "in_pos" "in_tc")>


#<make-shader "menu-shader"
#:vertex-shader #{
#version 150 core

	in vec3 in_pos;
	in vec3 in_norm;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	uniform mat4 model_normal;
	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		gl_Position = vec4(in_pos.x, in_pos.y,in_pos.z, 1.);
		float newy= in_tc.y -1;
		if(newy < 0) newy=-newy;
		vec2 texc = vec2(in_tc.x, newy);
		tc = texc;
		norm_wc = (model_normal * vec4(in_norm,0)).xyz;
	//	tc = in_tc;
	}

}
#:fragment-shader #{
#version 150 core

	out vec4 out_col;
	uniform sampler2D diffuse_tex;
	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 eye_pos;
	in vec4 pos_wc;
	in vec3 norm_wc;
	in vec2 tc;
		void main(){
		
			gl_FragDepth = 0.0001;
			out_col = vec4(texture2D(diffuse_tex, tc ).x,texture2D(diffuse_tex, tc ).y, texture2D(diffuse_tex, tc ).z ,1);
		//	out_col = vec4(tc.x,tc.y, 0 ,1);
		
		}

}

#:inputs (list "in_pos" "in_tc")>

#<make-shader "alpha-color-shader"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec3 in_norm;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	uniform mat4 model_normal;
	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		pos_wc = model * vec4(in_pos, 1.0);
		norm_wc = (model_normal * vec4(in_norm,0)).xyz;
		tc = in_tc;
		gl_Position = proj * view * pos_wc;
	}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	uniform sampler2D diffuse_tex;
	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 eye_pos;
	uniform vec3 color;
	uniform float use_alpha;
	uniform float use_lighting;
	uniform float depth;
	in vec4 pos_wc;
	in vec3 norm_wc;
	in vec2 tc;
	void main() {
		
		if(depth != 0 && depth < 1 && depth > 0){
			gl_FragDepth = depth;
		}	else {
			gl_FragDepth = gl_FragCoord.z;
		}
		
		vec3 sun_dir = vec3(0,-1,0);
		
		out_col = vec4(color,1.);
		float n_dot_l = max(0, dot(norm_wc, -light_dir));
		float n_dot_l_2 = max(0, dot(norm_wc, -sun_dir));
		
		if(use_lighting != 0.0 && use_alpha != 2){
			out_col += vec4(0.2 *color * light_col * (n_dot_l+n_dot_l_2), 0.);
		}
		vec3 col = texture(diffuse_tex, tc.st).rgb;
		
		if(col.x >= 0.9 && col.y <= 0.1 && col.z >= 0.9){
			discard;
		}
	
		
		//texcol
		if(!(col.x >= 0.99 && col.y >= 0.99 && col.z >= 0.99)){
			out_col = vec4(col,1.);
			
			if(use_lighting != 0.0)
				out_col += vec4(0.2 * col * (light_col + 0.2*color) * (n_dot_l+n_dot_l_2), 0.);
		
			if(use_alpha == 1){
				
				vec2 l = vec2(0.5,0.5) - tc;
				float dis = sqrt(l.x * l.x + l.y * l.y);
				out_col.a = max(0,1 - (dis*2));

			}
		} else if(use_alpha == 2){
			out_col.a = 0.4;
		}
			
	} 
}
#:inputs (list "in_pos" "in_norm" "in_tc")>



#<make-shader "game_over"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 tc;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos, 1.);
		float newy= in_tc.y -1;
		if(newy < 0) newy=-newy;
		vec2 texc = vec2(in_tc.x, newy);
		tc = texc;
	}
}
#:fragment-shader #{
#version 150 core
	in vec2 tc;
	out vec4 out_col;
	uniform sampler2D tex;
	void main() {
		vec3 color= texture(tex,tc).rgb;			
		out_col = vec4(texture(tex, tc).rgb, 1.);
	}
}
#:inputs (list "in_pos" "in_tc")>


#<make-shader "simple-menu-shader"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 tc;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos, 1.);
		float newy= in_tc.y -1;
		if(newy < 0) newy=-newy;
		vec2 texc = vec2(in_tc.x, newy);
		tc = texc;
	}
}
#:fragment-shader #{
#version 150 core
	in vec2 tc;
	out vec4 out_col;
	uniform vec3 color;
	uniform vec3 p_color = vec3(1,1,1);
	uniform sampler2D tex;
	uniform float depth;
	void main() {
		gl_FragDepth = depth;
		if(color.x == -1)
			out_col = vec4(texture(tex, tc).rgb, 1);
		
		else{
            vec4 tex_col = vec4(texture(tex,tc).x,texture(tex,tc).y,texture(tex,tc).z,1);
			if(	tex_col.x >= (color.x-0.3) && tex_col.x <= (color.x+0.3) &&
				tex_col.y >= (color.y-0.3) && tex_col.y <= (color.y+0.3) &&
				tex_col.z >= (color.z-0.3) && tex_col.z <= (color.z+0.3)){
					discard;
			} else
				out_col = tex_col * vec4(p_color.x,p_color.y, p_color.z,0.8);
		}
		
	}
}
#:inputs (list "in_pos" "in_tc")>

        #<make-shader "text-shader"
        #:vertex-shader #{
        #version 150 core

        in vec3 in_pos;
        in vec2 in_tc;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
        uniform vec3 CameraRight_worldspace;
        uniform vec3 CameraUp_worldspace;
        uniform vec3 BillboardPos;
        uniform vec2 BillboardSize;
        out vec2 tc;

                       void main() {
                               vec3 vp_w =
                                       BillboardPos
                                       + CameraRight_worldspace * in_pos.x  * BillboardSize.x
                                       + CameraUp_worldspace * in_pos.y * BillboardSize.y;

                               gl_Position = proj * view * vec4(vp_w , 1.);
                               tc = in_tc;
                       }
        }
        #:fragment-shader #{
        #version 150 core

        in vec2 tc;
        out vec4 out_col;
        uniform vec3 color;
        uniform sampler2D tex;
        //uniform float LifeLevel;


                       void main(){

                               gl_FragDepth = 0.01;

                               if(texture(tex, tc).r >= 0.5 || texture(tex, tc).g >= 0.5 || texture(tex, tc).b >= 0.5)
                                       out_col = vec4(color.r,color.g,color.b, 1.);
                               else
                                       discard;


                       }
        }

         #:inputs (list "in_pos" "in_tc")>



#<make-shader "menu-building-shader"
#:vertex-shader #{
#version 150 core
	in vec3 in_pos;
	in vec3 in_norm;
	in vec2 in_tc;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	uniform mat4 model_normal;
	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		gl_Position = proj * view * model * vec4(in_pos, 1.);	
		norm_wc = (model_normal * vec4(in_norm,0)).xyz;
		tc = in_tc;		
		pos_wc = model * vec4(in_pos, 1.0);
		gl_Position = proj * view * pos_wc;		
	}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	uniform sampler2D diffuse_tex;
	uniform vec3 light_dir;
	uniform vec3 light_col;
	uniform vec3 eye_pos;
	in vec4 pos_wc;
	in vec3 norm_wc;
	in vec2 tc;
	void main() {
		out_col = vec4(0.,0.,0.,1.);
		vec3 color = texture(diffuse_tex, tc.st).rgb;

		float n_dot_l = max(0, dot(norm_wc, -light_dir));
		out_col += vec4(color * light_col * n_dot_l, 0.);
	}
}
#:inputs (list "in_pos" "in_norm" "in_tc")>
