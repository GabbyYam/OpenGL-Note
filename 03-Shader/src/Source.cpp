#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

/**
 * @param source -> a vertexShader or fragmentShader we created
 * @param type -> figure out the type we pass in & compile our source and return 
 * 
 * @return shader id in GPU
 */

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderProgramSource parseShader(const std::string& shaderFilePath) {
    std::ifstream stream(shaderFilePath);
    
    enum class ShaderType {
        NONE = -1, VERTEX = 0 , FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE; //default type is none

    while (std::getline(stream,line)) {
        if (line.find("#shader") != std::string::npos) { //if find "#shader"
            if (line.find("vertex") != std::string::npos) { //set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) { //set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else ss[(int)type] << line << '\n';
    }
    return { ss[0].str(),ss[1].str()}; //return 2 segement of shader source

}

static unsigned int CompileShader(const std::string& source, unsigned int type) { 
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);


    // error handle segment
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv ->int vector
    if (result == GL_FALSE) {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* message = (char*) alloca( sizeof(char) * len); //allocate memory dynamicaly
        glGetShaderInfoLog(id, sizeof(message), &len, message); //message alredy is a pointer exactly
        std::cout << "Failed to compile"<<
            (type==GL_VERTEX_SHADER ? "vertex" : "fragment")<<"shader!"<< std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return -1;
    }


    return id;
}

// 1.compile our shader code 
// 2.binding with program 
// 3.then link our program to the GPU
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    //binding compiled shader to program
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    //---What is this?--------------
    glLinkProgram(program);
    glValidateProgram(program);
    //----------------------------

    glDeleteShader(vs);  //delete the shader because both of vertex&fragmentShader are binding into our program
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)std::cout << "error" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    float position[6] = {
        -0.5f, -0.5f,
         0.f,   0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;//buffer binding id in VM
    glGenBuffers(1, &buffer); //real memo in windows
    glBindBuffer(GL_ARRAY_BUFFER, buffer); //bind the above of them

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    
    ShaderProgramSource source = parseShader("res/shaders/Basic.shader");
    
    std::cout << source.vertexSource << std::endl;
    std::cout << source.fragmentSource << std::endl;

    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    glUseProgram(shader);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}