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
		if(texture(tex, tc).r >= 0.5 || texture(tex, tc).g >= 0.5 || texture(tex, tc).b >= 0.5)
			out_col = vec4(texture(tex, tc).rgb, 1.);
		else
			discard;
		
	}
}
#:inputs (list "in_pos" "in_tc")>

#<make-shader "heightmap_shader"
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
	in vec2 in_tc;

	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;

	out vec4 pos_wc;
	out vec3 norm_wc;
	out vec2 tc;
	void main() {
		pos_wc = vec4(in_pos, 1.0);
		//norm_wc = transpose(inverse(mat3x3(model))) * in_norm;
		//norm_wc = (model_normal * vec4(in_norm,0)).xyz;
		norm_wc = in_norm;
		tc = in_tc;
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
		color = visibility * 0.5 * color + visibility * diff + visibility * spec;
		out_col = vec4(color, 1.0);
	}
}
#:inputs (list "in_pos" "in_norm" "in_tc")>
