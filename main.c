#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

#define PI 3.14592653589f

static glm::mat4 proj;
static float delta_time = 0.0f;
static float last_frame = 0.0f;
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

struct Sphere {
  std::vector<float>        vertices;
  std::vector<float>        normals;
  std::vector<unsigned int> indices;
  unsigned int sphereID;
  unsigned int VBO;
  unsigned int VAO;
  unsigned int normalBuffer;
  unsigned int EBO;
};

struct Lines {
  std::vector<float> vertices;
  unsigned int lineID;
  unsigned int VBO;
  unsigned int VAO;
};

void setup_gl_for_lines(Lines *lines) {
  unsigned int VBO;
  unsigned int VAO;
  std::vector<float> &vertices = lines->vertices;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glUseProgram(lines->lineID);

  lines->VBO = VBO;
  lines->VAO = VAO;
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

void render_lines(Lines *lines) {
  glUseProgram(lines->lineID);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, lines->VAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_LINES, 0, lines->vertices.size()/6);
  glDisableVertexAttribArray(0);
  glUseProgram(0);
}

void init_sphere_gl_buffers(struct Sphere *sphere) {
  std::vector<float> &vertices = sphere->vertices;
  std::vector<float> &normals  = sphere->normals;
  std::vector<unsigned int> &indices = sphere->indices;

  glUseProgram(sphere->sphereID);
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);

  unsigned int normalBuffers;
  glGenBuffers(1, &normalBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, normalBuffers);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normals.size(), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);

  sphere->VAO = VAO;
  sphere->VBO = VBO;
  sphere->EBO = EBO;

}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0,0,width,height);
  float w = (float)width;
  float h = (float)height;
  proj = glm::perspective(glm::radians(45.0f), w / h, 0.1f, 100.0f);
}

void processInput(GLFWwindow *window) {
  return;
  float camera_speed = 2.5f * delta_time;
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_pos += camera_speed * camera_front;
  }
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_pos -= camera_speed * camera_front;
  }
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
  }
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
  }
}

static float lastx = 400;
static float lasty = 300;
static bool first_mouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  return;
  if(first_mouse) {
    lastx = xpos;
    lasty = ypos;
    first_mouse = false;
  }

  float xoffset = xpos - lastx;
  float yoffset = lasty - ypos;
  lastx = xpos;
  lasty = ypos;
  float sensitivity = 0.1f;
  xoffset += sensitivity;
  yoffset += sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if(pitch > 89.0f)
    pitch = 89.0f;
  if(pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera_front = glm::normalize(direction);
}


int create_shader(const char *vertex_file_name, const char *fragment_file_name) {
  char *vertex_glsl;
  char *fragment_glsl;
  {
    FILE *file = fopen(vertex_file_name, "r");
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    vertex_glsl = (char *)malloc(fsize + 1);
    fread(vertex_glsl, 1, fsize, file);
    fclose(file);
    vertex_glsl[fsize] = 0;
  }

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const char **)&vertex_glsl, 0);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
    printf("%s\n", infoLog);
  }

  {
    FILE *file = fopen(fragment_file_name, "r");
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fragment_glsl = (char *)malloc(fsize + 1);
    fread(fragment_glsl, 1, fsize, file);
    fclose(file);
    fragment_glsl[fsize] = 0;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char **)&fragment_glsl, 0);
  glCompileShader(fragmentShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
    printf("%s\n", infoLog);
  }

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, 0, infoLog);
    printf("%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  free(vertex_glsl);
  free(fragment_glsl);

  return shaderProgram;
}

void create_sphere(struct Sphere *sphere, float radius, int stackCount, int sectorCount) {
  std::vector<float> &vertices = sphere->vertices; 
  std::vector<float>&normals  = sphere->normals;
  std::vector<unsigned int> &indices  =sphere->indices;

  float lengthInv  = 1.0f / radius;
  float sectorStep = 2 * PI / sectorCount;
  float stackStep  = 2 * PI / stackCount;

  for(int i=0; i<=stackCount; i++) {
    float stackAngle = PI / 2.0 - i * stackStep;
    float xy = radius * cosf(stackAngle);
    float z  = radius * sinf(stackAngle);
    for(int j=0; j<=sectorCount; j++) {
      float sectorAngle = j*sectorStep;
      float x  = xy * cosf(sectorAngle);
      float y  = xy * sinf(sectorAngle);
      float nx = x * lengthInv;
      float ny = y * lengthInv;
      float nz = z * lengthInv;
      //float s  = (float)j / sectorCount;
      //float t  = (float)i / stackCount;
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
      normals.push_back(nx);
      normals.push_back(ny);
      normals.push_back(nz);
    }
  }

  for(int i=0; i<stackCount; i++) {
    int k1 = i * (sectorCount + 1);
    int k2 = k1 + sectorCount + 1;
    for(int j=0; j<sectorCount; j++) {
      if(i!=0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1+1);
      }
      if(i!=(stackCount-1)) {
        indices.push_back(k1+1);
        indices.push_back(k2);
        indices.push_back(k2+1);
      }

      k1++;
      k2++;
    }
  }

}

void render_sphere(struct Sphere* sphere) {
  //model = glm::translate(glm::mat4(1.0f), position);
  //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  glUseProgram(sphere->sphereID);
  //vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, sphere->VAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  //normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, sphere->normalBuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  //draw
  glDrawElements(GL_TRIANGLES, sphere->vertices.size(), GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
}



void parse_file(char *file_name, 
                std::vector<glm::vec3> &alpha_positions, 
                std::vector<glm::vec3> &beta_positions, 
                std::vector<glm::vec3> &other_positions) {
  char buffer[512];
  FILE *file = fopen(file_name, "r");
  if(!file) {
      printf("Error. Invalid file\n");
      return;
  }

  while(!feof(file)) {
    fscanf(file, "%s", buffer);
    if(strcmp(buffer, "HEADER")==0) {

    } else if(strcmp(buffer, "ATOM")==0) {
      int number;
      float x; 
      float y; 
      float z; 
      float a;
      float aa;
      fscanf(file, "%d", &number);
      fscanf(file, "%s", buffer);
      fscanf(file, "%s", buffer);
      fscanf(file, "%s", buffer);
      fscanf(file, "%d", &number);
      fscanf(file, "%f %f %f", &x, &y, &z);
      fscanf(file, "%f", &a);
      fscanf(file, "%f", &aa);
      fscanf(file, "%s", buffer);
      printf("ATOM: x=%5f, y=%5f, z=%5f\n", x, y, z);
      alpha_positions.push_back(glm::vec3(x, y, z));
    } else if(strcmp(buffer, "HETATM")==0) {
      int number;
      float x; 
      float y; 
      float z; 
      float a;
      float aa;
      fscanf(file, "%d", &number);
      fscanf(file, "%s", buffer);
      fscanf(file, "%s", buffer);
      fscanf(file, "%s", buffer);
      fscanf(file, "%d", &number);
      fscanf(file, "%f %f %f", &x, &y, &z);
      fscanf(file, "%f", &a);
      fscanf(file, "%f", &aa);
      fscanf(file, "%s", buffer);
      printf("HETATM: x=%5f, y=%5f, z=%5f\n", x, y, z);
      beta_positions.push_back(glm::vec3(x, y, z));

    } else if(strcmp(buffer, "TER")==0) {
      int serial_number;
      int residue_seq;

      fscanf(file, "%d", &serial_number); // serial number
      fscanf(file, "%s", buffer);         // residue name
      fscanf(file, "%s", buffer);         // chain identifier
      fscanf(file, "%d", &residue_seq);   // residue sequence number
      //fscanf(file, "%s", buffer);         // code for insertions of residues

    } else if(strcmp(buffer, "HELIX")==0) {
      /*int serial_number;
      int residue_seq;
      int residue_seq_n;
      int type_helix;
      int length;
      fscanf(file, "%d", &serial_number); // serial number
      fscanf(file, "%s", buffer);         // helix identifier
      fscanf(file, "%s", buffer);         // initial residue name
      fscanf(file, "%s", buffer);         // chain identifier
      fscanf(file, "%d", &residue_seq);   // residue sequence number
      fscanf(file, "%s", buffer);         // code for insertions of residues
      fscanf(file, "%s", buffer);         // terminal residue name
      fscanf(file, "%s", buffer);         // chain identifier
      fscanf(file, "%d", &residue_seq_n); // residue sequence number
      fscanf(file, "%s", buffer);         // code for insertions of residues


      fscanf(file, "%d", &type_helix);    // type of helix 
          -right-naded alpha, 1
          -right-handed omega 2
          -right-handed pi    3
          -right-handed gamma 4
          -right-handed 310   5
          -left-handed alpha, 6
          -left-handed omega, 7
          -left-handed gamma, 8
          -27 ribbon/helix,   9
          -polyproline       10


      fscanf(file, "%s", buffer);         // comment
      fscanf(file, "%d", &length);        // length of helix*/

    } else if(strcmp(buffer, "SHEET")==0) {
      /*
      SHEET                 STRING
      STRAND                INTEGER
      SHEETID               STRING
      NUM STRANDS           INTEGER
      INIT RES NAME         STRING
      INIT CHAIN ID         CHARACTER
      INIT SEQ NUM          INTEGER
      INIT I CODE           ACHAR
      END RES NAME          STRING
      END CHAIN ID          CHARACTER
      END SEQ NUM           INTEGER
      END I CODE            ACHAR
      SENSE                 INTEGER
      CUR ATOM              ATOM
      CUR RESIDUE NAME
      CUR CHAIN ID          CHARACTER
      CUR RES SEQ           INTEGER
      CUR I CODE            ACHAR
      PREV ATOM             ATOM
      PREV RES NAME         RESIDUE NAME
      PREV CHAIN ID         CHARACTER
      PREV RES SEQ          INTEGER
      PREV I CODE           ACHAR          
      */
    } else if(strcmp(buffer, "SSBOND")==0) {

    }
  }
  fclose(file);


}


int main(int argc, char **argv) {
  srand(time(0));
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(800, 600, "PDB Viewer", 0, 0);
  if(window == 0) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }
  Sphere sphere;
  Lines  lines;
  for(int i=0; i<20; i++) {
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
    lines.vertices.push_back((float)(rand() % 200) / 100.0f - 1.0f);
  }
  lines.vertices.push_back(0.0f);
  lines.vertices.push_back(0.0f);
  lines.vertices.push_back(0.0f);
  lines.vertices.push_back(2.0f);
  lines.vertices.push_back(2.0f);
  lines.vertices.push_back(2.0f);


  lines.lineID = create_shader("lines.vsh", "lines.fsh");
  setup_gl_for_lines(&lines);


  sphere.sphereID = create_shader("triangle.vsh", "triangle.fsh");
  float radius = 0.25f;
  int stackCount = 16;
  int sectorCount = 16;
  create_sphere(&sphere, radius, stackCount, sectorCount);
  init_sphere_gl_buffers(&sphere);


  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // model matrix
  glm::mat4 model = glm::mat4(1.0f);

  // view matrix
  glm::mat4 view;

  // projection matrix
  proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
  glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 light_pos = glm::vec3(5.0f, 3.0f, 5.0f);


  // transform
  std::vector<glm::vec3> alpha_positions;
  std::vector<glm::vec3> beta_positions;
  std::vector<glm::vec3> other_positions;


  if(argc==2) {
    parse_file(argv[1], alpha_positions, beta_positions, other_positions);
  }

  /*for(int i=0; i<5; i++) {
    float t = i / 2.0f;
    alpha_positions.push_back(glm::vec3(t, t, -t));
  }
  for(int i=0; i<2; i++) {
    float x = (-3.0+i) / 1.5f;
    beta_positions.push_back(glm::vec3(x, 0.5f, x));
  }
  for(int i=0; i<2; i++) {
    float x = i / 1.5f - 3.0f;
    other_positions.push_back(glm::vec3(x, 0.5f, x));
  }*/

  int modelLoc = glGetUniformLocation(sphere.sphereID,"model");
  glUseProgram(sphere.sphereID);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  int viewLoc = glGetUniformLocation(sphere.sphereID,"view");
  glUseProgram(sphere.sphereID);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  int projLoc = glGetUniformLocation(sphere.sphereID,"projection");
  glUseProgram(sphere.sphereID);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
  int objectColorLoc = glGetUniformLocation(sphere.sphereID, "objectColor");
  glUseProgram(sphere.sphereID);
  glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
  int lightColorLoc = glGetUniformLocation(sphere.sphereID, "lightColor");
  glUseProgram(sphere.sphereID);
  glUniform3f(lightColorLoc, light_color.x, light_color.y, light_color.z);
  int viewPosLoc = glGetUniformLocation(sphere.sphereID, "viewPos");
  glUseProgram(sphere.sphereID);
  glUniform3f(viewPosLoc, camera_pos.x, camera_pos.y, camera_pos.z);
  int lightPosLoc = glGetUniformLocation(sphere.sphereID, "lightPos");
  glUseProgram(sphere.sphereID);
  glUniform3f(lightPosLoc, light_pos.x, light_pos.y, light_pos.z);

  unsigned int modelLocLine = glGetUniformLocation(lines.lineID,"model");
  glUseProgram(lines.lineID);
  glUniformMatrix4fv(modelLocLine, 1, GL_FALSE, glm::value_ptr(model));
  int viewLocLine = glGetUniformLocation(lines.lineID,"view");
  glUseProgram(lines.lineID);
  glUniformMatrix4fv(viewLocLine, 1, GL_FALSE, glm::value_ptr(view));
  int projLocLine = glGetUniformLocation(lines.lineID,"projection");
  glUseProgram(lines.lineID);
  glUniformMatrix4fv(projLocLine, 1, GL_FALSE, glm::value_ptr(proj));

  
  glfwSetCursorPosCallback(window, mouse_callback);
  glm::vec3 up;
  glm::vec3 camera_right;
  glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 camera_direction = glm::normalize(camera_pos - camera_target);


  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  while(!glfwWindowShouldClose(window)) {
    // delta calculation
    glUseProgram(0);
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    //printf("delta time: %f\n", delta_time);

    // view and camera calculation
    float radius = 100.0f;
    float cam_x = sin(0.01*glfwGetTime()) * radius;
    float cam_z = cos(0.01*glfwGetTime()) * radius;

    view = glm::lookAt(glm::vec3(cam_x, 0.0f, cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));


    // input
    processInput(window);
    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    float r;
    float g;
    float b;

    r = 1.0f;
    g = 0.2f;
    b = 0.2f;
   
    glUseProgram(sphere.sphereID);
    glUniform3f(objectColorLoc, r, g, b);
    for(int i=0; i<alpha_positions.size(); i++) {
      glUseProgram(sphere.sphereID);
      model = glm::translate(glm::mat4(1.0f), alpha_positions[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      render_sphere(&sphere);
    }

    r = 0.2f;
    g = 0.2f;
    b = 1.0f;
    glUseProgram(sphere.sphereID);
    glUniform3f(objectColorLoc, r, g, b);
    for(int i=0; i<beta_positions.size(); i++) {
      glUseProgram(sphere.sphereID);
      model = glm::translate(glm::mat4(1.0f), beta_positions[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      render_sphere(&sphere);
    }

    r = 0.5f;
    g = 0.5f;
    b = 0.5f;
    glUseProgram(sphere.sphereID);
    glUniform3f(objectColorLoc, r, g, b);
    for(int i=0; i<other_positions.size(); i++) {
      glUseProgram(sphere.sphereID);
      model = glm::translate(glm::mat4(1.0f), other_positions[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      render_sphere(&sphere);
    }

    // model
    glUseProgram(sphere.sphereID);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(viewPosLoc, camera_pos.x, camera_pos.y, camera_pos.z);
    glUniform3f(lightColorLoc, light_color.x, light_color.y, light_color.z);
    glUniform3f(lightPosLoc, light_pos.x, light_pos.y, light_pos.z);
    model = glm::mat4(1.0f);
    glUseProgram(lines.lineID);
    glUniformMatrix4fv(viewLocLine, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLocLine, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(modelLocLine, 1, GL_FALSE, glm::value_ptr(model));
    // draw lines
    render_lines(&lines);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
