#version 330 core
	out vec4 FragColor;

	uniform float time;
	uniform vec3 player_pos;
	uniform vec3 dir;
	uniform vec2 resolution;

	const float PI = 3.14159265358979323846;

	vec3 sph_pos[21] = vec3[21](
		vec3(1.0, 0, 0),
		vec3(-1.0, 0.5, 0.5),
		vec3(-1.0, -0.5, -0.5),
		vec3(0.5, 1, 0.5),
		vec3(0, 1, 0),
		vec3(-0.5, 1, -0.5), 
		vec3(0.5, -1, 0.5), 
		vec3(0.5, -1, -0.5), 
		vec3(-0.5, -1, 0.5), 
		vec3(-0.5, -1, -0.5), 
		vec3(0.5, 0.5, -1), 
		vec3(0.5, -0.5, -1), 
		vec3(0, 0, -1), 
		vec3(-0.5, 0.5, -1), 
		vec3(-0.5, -0.5, -1), 
		vec3(0.5, 0.5, 1), 
		vec3(0.5, 0, 1), 
		vec3(0.5, -0.5, 1), 
		vec3(-0.5, 0.5, 1), 
		vec3(-0.5, 0, 1), 
		vec3(-0.5, -0.5, 1)
	);
	
	vec2 boxIntersection(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN, out vec3 light)  {
		vec3 angle = vec3(radians(20 + time / 5), radians(50 - time / 5), radians(-20 + time / 5));
		mat3 rot_x = mat3(
			1, 0, 0,
			0, cos(angle.x), -sin(angle.x),
			0, sin(angle.x), cos(angle.x)
		);
		mat3 rot_y = mat3(
			cos(angle.y), 0, sin(angle.y),
			0, 1, 0,
			-sin(angle.y), 0, cos(angle.y)
		);
		mat3 rot_z = mat3(
			cos(angle.z), -sin(angle.z), 0,
			sin(angle.z), cos(angle.z), 0,
			0, 0, 1
		);
		ro *= rot_x * rot_y * rot_z;
		rd *= rot_x * rot_y * rot_z;
		light = vec3(-5, 6, 10) * rot_x * rot_y * rot_z;

		vec3 m = 1.0 / rd;
		vec3 n = m * ro;
		vec3 k = abs(m) * rad;
		vec3 t1 = -n - k;
		vec3 t2 = -n + k;
		float tN = max(max(t1.x, t1.y), t1.z);
		float tF = min(min(t2.x, t2.y), t2.z);
		if(tN > tF || tF < 0.0) return vec2(-1.0);
		oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
		return vec2(tN, tF);
	}
	vec2 sphIntersect(in vec3 ro, in vec3 rd, float ra) {
		float b = dot(ro, rd);
		float c = dot(ro, ro) - ra * ra;
		float h = b * b - c;
		if(h < 0.0) return vec2(-1.0);
		h = sqrt(h);
		return vec2(-b - h, -b + h);
	}
	float plaIntersect(in vec3 ro, in vec3 rd, in vec4 p) {
		return -(dot(ro, p.xyz) + p.w) / dot(rd, p.xyz);
	}


	void main() {
    	vec2 uv = (gl_FragCoord.xy / resolution - 0.5);
		uv.x *= resolution.x / resolution.y;
		vec3 rayDirection = normalize(vec3(1.0, uv));
		vec3 norm;
		
		vec3 box_pos = vec3(20, 0, 5);
		vec3 plane_pos = vec3(0, 0, 0);
		vec3 light;
		vec3 plane_normal = vec3(0, 0, 1);
		float dist1 = boxIntersection(player_pos - box_pos, rayDirection, vec3(2), norm, light).x;
		float dist2 = plaIntersect(player_pos - plane_pos, rayDirection, vec4(plane_normal, 1));
		vec3 static_light = vec3(-5, 6, 10);
		float dist3 = -1;

		for (int i = 0; i < 21; ++i){
			float cur_dist = sphIntersect(player_pos - (box_pos + sph_pos[i]), rayDirection, 0.3);
			if (dist3 == -1 || cur_dist < dist3) dist3 = cur_dist;
		}
		


		if (dist2 < 0){
			if (dist1 != -1) {
				if (dist3 < dist1) FragColor = vec4(1, 0, 0, 1);
				else FragColor = vec4(vec3(dot(normalize(light), norm)), 1.0);
			}
			else FragColor = vec4(vec3(0.05), 1.0);
		}
		else{
			if (dist1 != -1 && dist1 < dist2) {
				if (dist3 < dist1 && dist3 != -1) FragColor = vec4(1, 0, 0, 1);
				else FragColor = vec4(vec3(dot(normalize(light), norm)), 1.0);
			}
			else{
				vec3 new_or = player_pos + rayDirection * dist2;
				vec3 new_dir = static_light - new_or;
				if (boxIntersection(new_or - box_pos, new_dir, vec3(2), norm, light).x != -1) FragColor = vec4(vec3(0.2 - dist2 / 200), 1);
				else FragColor = vec4(vec3(10 / dist2), 1.0);	
			}
		} 
		
	}