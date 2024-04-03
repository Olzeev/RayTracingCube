// STARTER CODE FROM learnopengl.com

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <fstream>
#include <sstream>


#include <iostream>
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";


const char *fragmentShaderSource = R"(
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
	vec3 angle = vec3(radians(time / 5), radians(time / 5), radians(0));
	
	vec2 boxIntersection(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN, out vec3 light)  {
		
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
	vec2 sphIntersect(in vec3 ro, in vec3 pos, in vec3 rd, float ra, in vec3 box_pos) {
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
		//ro.z -= 10;
		rd *= rot_x * rot_y * rot_z;
		ro *= rot_x * rot_y * rot_z;
		ro += pos;
			
		


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
		vec3 sphPos;

		for (int i = 0; i < 21; ++i){
			vec3 angle1 = vec3(angle.x, angle.y, angle.z);
			
			mat3 rot_x = mat3(
				1, 0, 0,
				0, cos(angle1.x), -sin(angle1.x),
				0, sin(angle1.x), cos(angle1.x)
			);
			mat3 rot_y = mat3(
				cos(angle1.y), 0, sin(angle1.y),
				0, 1, 0,
				-sin(angle1.y), 0, cos(angle1.y)
			);
			mat3 rot_z = mat3(
				cos(angle1.z), -sin(angle1.z), 0,
				sin(angle1.z), cos(angle1.z), 0,
				0, 0, 1
			);
		

			float cur_dist = sphIntersect(-player_pos + box_pos, sph_pos[i] * 2, -rayDirection, 0.2, box_pos).x;
			if (cur_dist != -1 && (dist3 == -1 || cur_dist < dist3)) {
				dist3 = cur_dist;
				sphPos = sph_pos[i];
			}
		}
		


		if (dist2 < 0){
			if (dist1 != -1) {
				if (dist3 < dist1 && dist3 != -1) FragColor = vec4(1, 0, 0, 1);
				else FragColor = vec4(vec3(dot(normalize(light), norm)), 1.0);
			} else if (dist3 != -1)FragColor = vec4(1, 0, 0, 1);
			else FragColor = vec4(vec3(0.05), 1.0);
		}
		else{
			if (dist1 != -1 && dist1 < dist2) {
				if (dist3 < dist1 && dist3 != -1) FragColor = vec4(1, 0, 0, 1);
				else FragColor = vec4(vec3(dot(normalize(light), norm)), 1.0);
			}
			else{
				if (dist3 != -1 && dist3 < dist2) FragColor = vec4(1, 0, 0, 1);
				else{
					vec3 new_or = player_pos + rayDirection * dist2;
					vec3 new_dir = static_light - new_or;
					if (boxIntersection(new_or - box_pos, new_dir, vec3(2), norm, light).x != -1) FragColor = vec4(vec3(0.2 - dist2 / 200), 1);
					else FragColor = vec4(vec3(10 / dist2), 1.0);
				}
					
			}
		} 
		//if (dist3 != -1) FragColor = vec4(1, 0, 0, 1);
	}
)";


float time1 = 0;

std::string readShaderFile(const std::string& filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::stringstream stringBuffer;
    stringBuffer << fileStream.rdbuf();
    content = stringBuffer.str();
	
    fileStream.close();
    return content;
}
//const char *fragmentShaderSource = readShaderFile("src/fragment_shader.txt").c_str();

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GTA 7", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	
    // Определение вершин треугольника
    float vertices[] = {
       -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f
   };
   unsigned int indices[] = {
       0, 1, 2, // Первый треугольник
       2, 3, 0  // Второй треугольник
   };
	unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	

	float playerx = 0, playery = 0, playerz = 5;
	float dirx = 1, diry = 0, dirz = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


		GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
		// Устанавливаем значение переменной
		glUniform1f(timeLocation, time1);
		GLint playerPosLocation = glGetUniformLocation(shaderProgram, "player_pos");

		// Передаем значения массива в шейдер
		glUniform3f(playerPosLocation, playerx, playery, playerz);

		GLint playerDirLocation = glGetUniformLocation(shaderProgram, "player_dir");

		// Передаем значения массива в шейдер
		glUniform3f(playerDirLocation, dirx, diry, dirz);


		GLint ResolutionLocation = glGetUniformLocation(shaderProgram, "resolution");
		glUniform2f(ResolutionLocation, SCR_WIDTH, SCR_HEIGHT);

		time1 += 1;

        // Использование шейдерной программы и рисование
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}