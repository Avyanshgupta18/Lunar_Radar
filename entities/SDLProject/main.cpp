/**
* Author: Avyansh Gupta
* Assignment: Lunar Lander
* Date due: 2024-10-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#include <vector>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, DISPLAY_MESSAGE, TERMINATED };

// Constants for gravity and movement
constexpr float GRAVITY = -0.25f;
constexpr float HORIZONTAL_ACCELERATION = 0.6f;
constexpr float FRICTION = 1.0f;
constexpr float DISPLAY_DELAY_SECONDS = 2.0f;

// Declare variables for mission status and display delay
float end_time = 0.0f;
GLuint mission_failed_texture_id;
GLuint mission_accomplished_texture_id;
bool mission_success = false;

 constexpr int MAX_FIRE_STRENGTH = 120;
     constexpr float UPWARD_ACCELERATION = 0.6f;
int fire_strength = MAX_FIRE_STRENGTH;

     GLuint g_fire_strength_full_texture_id;
     GLuint g_fire_strength_medium_texture_id;
     GLuint g_fire_strength_low_texture_id;
     GLuint g_fire_strength_empty_texture_id;
// Acceleration vector
glm::vec3 m_acceleration = glm::vec3(0.0f, GRAVITY, 0.0f);

// Window dimensions and background color
constexpr float WINDOW_SIZE_MULT = 1.0f;
constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT;
constexpr int WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;
constexpr float BG_RED     = 0.9f;
constexpr float BG_GREEN   = 0.9f;
constexpr float BG_BLUE    = 1.0f;
constexpr float BG_OPACITY = 1.0f;
constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH  = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;
constexpr char MANGROVE_SPRITE_FILEPATH[] = "assets/mangrove.png";

// Filepaths for shader programs
constexpr char WATER_SPRITE_FILEPATH[] = "assets/water_drop.jpg";
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr char MONKEY_SPRITE_FILEPATH[] = "assets/monkey.jpg";
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;
constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char WALL_SPRITE_FILEPATH[] = "assets/wall.jpeg";



GLuint g_mangrove_texture_id;
// Set object positions and scales
std::vector<glm::vec3> mangrove_positions = {
    glm::vec3(-4.0f, 0.7f, 0.0f),
    glm::vec3(3.0f, 0.5f, 0.0f),
    glm::vec3(5.0f, 1.3f, 0.0f)
};
GLuint g_water_texture_id;
glm::mat4 g_wall_matrix;
// Variables for window, shader program, and matrices

constexpr glm::vec3 INIT_SCALE_WALL = glm::vec3(0.5f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_POS_MONKEY    = glm::vec3(1.0f, 4.0f, 0.0f);
 constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;

     constexpr glm::vec3 INIT_SCALE_MONKEY  = glm::vec3(0.5f, 1.0f, 0.0f);
    
     constexpr glm::vec3 INIT_POS_WALL   = glm::vec3(4.0f, -3.0f, 0.0f);

std::vector<glm::vec3> wall_positions = {
        glm::vec3(-3.0f, -3.0f, 0.0f), glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(4.5f, -3.0f, 0.0f), glm::vec3(1.0f, -3.0f, 0.0f)
    };
SDL_Window* display_window;
AppStatus app_status = RUNNING;
ShaderProgram shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix, g_monkey_matrix;

// Trackers for timing
float previous_ticks = 0.0f;

// Texture IDs for elements
GLuint g_wall_texture_id;
GLuint g_monkey_texture_id;

glm::vec3 monkey_position = glm::vec3(0.0f, 2.0f, 0.0f);
// Set movement speed and other object attributes
constexpr float MONKEY_SPEED = 3.5f;
glm::vec3 g_wall_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_wall_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 monkey_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_drops_size  = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 monkey_scale = glm::vec3(1.3f, 1.3f, 0.0f);
void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        std::cout << filepath << std::endl;
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}

void mission_failed() {
    mission_success = false;
    app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / 1000.0f;
}

void mission_accomplished() {
    mission_success = true;
    app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / 1000.0f;
}

void initialise()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG("SDL Initialization failed: " << SDL_GetError());
        return;
    }

    display_window = SDL_CreateWindow("Lunar Lander - Water and Monkey Theme",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    if (!display_window) {
        LOG("Window creation failed: " << SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    if (!context) {
        LOG("OpenGL Context creation failed: " << SDL_GetError());
        SDL_DestroyWindow(display_window);
        SDL_Quit();
        return;
    }
    SDL_GL_MakeCurrent(display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_wall_matrix = glm::mat4(1.0f);
    g_monkey_matrix = glm::mat4(1.0f);
    g_monkey_matrix = glm::translate(g_monkey_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_projection_matrix = glm::ortho(-6.0f, 6.0f, -4.75f, 4.75f, -2.0f, 2.0f);
    monkey_position += monkey_movement;
    g_view_matrix = glm::mat4(1.0f);
    shader_program.set_projection_matrix(g_projection_matrix);
    shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(shader_program.get_program_id());
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    // Load textures
    g_water_texture_id = load_texture(WATER_SPRITE_FILEPATH);
    g_monkey_texture_id = load_texture(MONKEY_SPRITE_FILEPATH);
    g_wall_texture_id = load_texture(WALL_SPRITE_FILEPATH);
    g_mangrove_texture_id = load_texture(MANGROVE_SPRITE_FILEPATH);
    mission_failed_texture_id = load_texture("assets/lose.png");
    mission_accomplished_texture_id = load_texture("assets/win.png");
     g_fire_strength_full_texture_id = load_texture("assets/fire_high.png");
             g_fire_strength_medium_texture_id = load_texture("assets/fire_medium.png");
             g_fire_strength_low_texture_id = load_texture("assets/fire_low.png");
            g_fire_strength_empty_texture_id = load_texture("assets/fire_empty.png");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
        m_acceleration.x = 0.0f;
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    app_status = TERMINATED;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                            app_status = TERMINATED;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_A] && fire_strength>0) {
        m_acceleration.x = -HORIZONTAL_ACCELERATION;
        fire_strength--;
    }
    else if (key_state[SDL_SCANCODE_D]&& fire_strength>0) {
        m_acceleration.x = HORIZONTAL_ACCELERATION;
        fire_strength--;
    }
    m_acceleration.y = GRAVITY;
}

void update()
{
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;

    m_acceleration.y = GRAVITY;
    monkey_movement += m_acceleration * delta_time;
    monkey_movement.x *= FRICTION;
    monkey_position += monkey_movement * delta_time;

    g_monkey_matrix = glm::mat4(1.0f);
    g_monkey_matrix = glm::translate(g_monkey_matrix, monkey_position);

    for (const auto& mangrove_position : mangrove_positions) {
        float x_distance = fabs(monkey_position.x - mangrove_position.x);
        float y_distance = fabs(monkey_position.y - mangrove_position.y);
        if (x_distance < 1.5f && y_distance < 1.0f) {
            mission_failed();
            return;
        }
    }
    for (const auto& wall_position : wall_positions) {
        float x_distance = fabs(monkey_position.x - wall_position.x);
        float y_distance = fabs(g_wall_position.y - wall_position.y);
        if (g_wall_position.y <= -3.0f && x_distance < 0.5f && y_distance < 0.1f) {
                    mission_failed();
                    return;
                }
            }

    if (monkey_position.y <= -4.75f) {
        mission_accomplished();
    }
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
                -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
                -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
            };

            float texture_coordinates[] = {
                0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            };
    glVertexAttribPointer(shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(shader_program.get_position_attribute());

    glVertexAttribPointer(shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(shader_program.get_tex_coordinate_attribute());

    if (app_status == DISPLAY_MESSAGE) {
        glm::mat4 message_matrix = glm::mat4(1.0f);
        message_matrix = glm::scale(message_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
        GLuint message_texture_id = (mission_success) ? mission_accomplished_texture_id : mission_failed_texture_id;
        draw_object(message_matrix, message_texture_id);
    } else if (app_status == RUNNING) {
        glm::mat4 water_matrix = glm::mat4(1.0f);
        water_matrix = glm::translate(water_matrix, glm::vec3(0.0f, -2.8f, 0.0f));
        water_matrix = glm::scale(water_matrix, glm::vec3(10.0f, 3.5f, 1.0f));
        draw_object(water_matrix, g_water_texture_id);

        for (const auto& mangrove_position : mangrove_positions) {
            glm::mat4 mangrove_matrix = glm::mat4(1.0f);
            mangrove_matrix = glm::translate(mangrove_matrix, mangrove_position);
            mangrove_matrix = glm::scale(mangrove_matrix, glm::vec3(1.6f, 1.1f, 1.0f));
            draw_object(mangrove_matrix, g_mangrove_texture_id);
        }
        for (const auto& mangrove_position : mangrove_positions) {
                        glm::mat4 mangrove_matrix = glm::mat4(1.0f);
                        mangrove_matrix = glm::translate(mangrove_matrix, mangrove_position);
                        mangrove_matrix = glm::scale(mangrove_matrix, glm::vec3(1.5f, 1.0f, 1.0f));
                        draw_object(mangrove_matrix, g_mangrove_texture_id);
                    }

        draw_object(g_monkey_matrix, g_monkey_texture_id);
        GLuint fire_texture_id;
        if (fire_strength > 2) {
            fire_texture_id = g_fire_strength_full_texture_id;
        } else if (fire_strength > 1) {
            fire_texture_id = g_fire_strength_medium_texture_id;
        } else if (fire_strength > 0) {
            fire_texture_id = g_fire_strength_low_texture_id;
                    } else {
                        fire_texture_id = g_fire_strength_empty_texture_id;
                    }
                    glm::mat4 wing_strength_matrix = glm::mat4(1.0f);
                    wing_strength_matrix = glm::translate(wing_strength_matrix, glm::vec3(4.0f, 3.0f, 0.0f));
                    wing_strength_matrix = glm::scale(wing_strength_matrix, glm::vec3(1.5f, 1.5f, 1.0f));
        draw_object(wing_strength_matrix, fire_texture_id);
    }
    glDisableVertexAttribArray(shader_program.get_position_attribute());
    glDisableVertexAttribArray(shader_program.get_tex_coordinate_attribute());
    SDL_GL_SwapWindow(display_window);
}

void shutdown() {
    LOG("Shutting down...");
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialise();

    while (app_status == RUNNING || app_status == DISPLAY_MESSAGE)
    {
        if (app_status == RUNNING) {
            process_input();
            update();
        }
        render();

        if (app_status == DISPLAY_MESSAGE) {
            float current_time = SDL_GetTicks() / 1000.0f;
            if (current_time - end_time >= DISPLAY_DELAY_SECONDS) {
                app_status = TERMINATED;
            }
        }
    }
    shutdown();
    return 0;
}
//
//#define GL_SILENCE_DEPRECATION
//#define STB_IMAGE_IMPLEMENTATION
//#define LOG(argument) std::cout << argument << '\n'
//#define GL_GLEXT_PROTOTYPES 1
//
//#ifdef _WINDOWS
//#include <GL/glew.h>
//#endif
//
//#include <SDL2/SDL.h>
//#include <SDL_opengl.h>
//#include "glm/mat4x4.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "ShaderProgram.h"
//#include "stb_image.h"
//
//enum AppStatus { RUNNING, TERMINATED };
//
//constexpr int WINDOW_WIDTH  = 640 * 2,
//              WINDOW_HEIGHT = 480 * 2;
//
//constexpr float BG_RED     = 0.9765625f,
//                BG_GREEN   = 0.97265625f,
//                BG_BLUE    = 0.9609375f,
//                BG_OPACITY = 1.0f;
//
//constexpr int VIEWPORT_X      = 0,
//              VIEWPORT_Y      = 0,
//              VIEWPORT_WIDTH  = WINDOW_WIDTH,
//              VIEWPORT_HEIGHT = WINDOW_HEIGHT;
//
//constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
//               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
//
//constexpr float MILLISECONDS_IN_SECOND = 1000.0;
//
//constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
//                LEVEL_OF_DETAIL    = 0, // mipmap reduction image level
//                TEXTURE_BORDER     = 0; // this value MUST be zero
//
//// Make sure the paths are correct on your system
//constexpr char KIMI_SPRITE_FILEPATH[]    = "/Users/avyanshgupta/Desktop/kimi.png",
//               TOTSUKO_SPRITE_FILEPATH[] = "/Users/avyanshgupta/Desktop/totsuko.png";
//
//constexpr glm::vec3 INIT_SCALE       = glm::vec3(5.0f, 5.98f, 0.0f),
//                    INIT_POS_KIMI    = glm::vec3(2.0f, 0.0f, 0.0f),
//                    INIT_POS_TOTSUKO = glm::vec3(-2.0f, 0.0f, 0.0f);
//
//constexpr float ROT_INCREMENT     = 1.0f;
//constexpr float TRAN_VALUE        = 0.025f;
//constexpr float G_GROWTH_FACTOR   = 1.01f;
//constexpr float G_SHRINK_FACTOR   = 0.99f;
//constexpr float ROT_ANGLE         = glm::radians(1.5f); // Smaller rotation angle
//constexpr int   G_MAX_FRAME       = 40;
//
//SDL_Window* g_display_window;
//AppStatus g_app_status = RUNNING;
//ShaderProgram g_shader_program = ShaderProgram();
//
//glm::mat4 g_view_matrix,
//          g_kimi_matrix,
//          g_totsuko_matrix,
//          g_projection_matrix;
//
//float g_previous_ticks = 0.0f;
//int g_frame_counter = 0;
//bool g_is_growing = true;
//
//GLuint g_kimi_texture_id,
//       g_totsuko_texture_id;
//
//constexpr float CIRCLE_RADIUS = 2.0f; // Radius for circular motion
//float g_totsuko_angle = 0.0f; // Angle for circular motion
//
//GLuint load_texture(const char* filepath)
//{
//    int width, height, number_of_components;
//    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
//
//    if (image == NULL)
//    {
//        LOG("Unable to load image. Make sure the path is correct.");
//        assert(false);
//    }
//
//    GLuint textureID;
//    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    stbi_image_free(image);
//
//    return textureID;
//}
//
//void initialise()
//{
//    SDL_Init(SDL_INIT_VIDEO);
//
//    g_display_window = SDL_CreateWindow("Hello, Transformations!",
//                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//                                      WINDOW_WIDTH, WINDOW_HEIGHT,
//                                      SDL_WINDOW_OPENGL);
//
//    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
//    SDL_GL_MakeCurrent(g_display_window, context);
//
//    if (g_display_window == nullptr)
//    {
//        std::cerr << "Error: SDL window could not be created.\n";
//        SDL_Quit();
//        exit(1);
//    }
//
//#ifdef _WINDOWS
//    glewInit();
//#endif
//
//    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
//
//    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
//
//    g_kimi_matrix       = glm::mat4(1.0f); // Start upright, no initial rotation
//    g_totsuko_matrix    = glm::mat4(1.0f);
//    g_view_matrix       = glm::mat4(1.0f);
//    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
//
//    g_shader_program.set_projection_matrix(g_projection_matrix);
//    g_shader_program.set_view_matrix(g_view_matrix);
//
//    glUseProgram(g_shader_program.get_program_id());
//
//    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
//
//    g_kimi_texture_id   = load_texture(KIMI_SPRITE_FILEPATH);
//    g_totsuko_texture_id = load_texture(TOTSUKO_SPRITE_FILEPATH);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//void process_input()
//{
//    SDL_Event event;
//    while (SDL_PollEvent(&event))
//    {
//        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
//        {
//            g_app_status = TERMINATED;
//        }
//    }
//}
//constexpr glm::vec3 TOTSUKO_SCALE = glm::vec3(0.99f, 0.99f, 1.0f);
//constexpr glm::vec3 KIMI_SCALE = glm::vec3(1.0f, 1.0f, 1.0f); // Fixed scale for Kimi
//constexpr float ORBIT_SPEED = 1.0f; // Adjust this for speed of orbit
//constexpr float RADIUS = 2.0f;       // Distance from Kimi to Totsuko
//
//float g_angle = 0.0f; // Angle for Totsuko's orbit
//float g_x_offset = 0.0f; // X offset for Totsuko's position
//float g_y_offset = 0.0f; // Y offset for Totsuko's position
//
//
//
//
//void update()
//{
//
//    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//        float delta_time = ticks - g_previous_ticks;
//        g_previous_ticks = ticks;
//
//        // Step 1: Update for Kimi's scaling behavior (pumping effect)
//        g_frame_counter += 1;
//
//        // Update for scaling behavior (pumping effect)
//        if (g_frame_counter >= G_MAX_FRAME)
//        {
//            g_is_growing = !g_is_growing;
//            g_frame_counter = 0;
//        }
//
//        glm::vec3 scale_vector = glm::vec3(
//            g_is_growing ? G_GROWTH_FACTOR : G_SHRINK_FACTOR,
//            g_is_growing ? G_GROWTH_FACTOR : G_SHRINK_FACTOR,
//            1.0f);
//
//        // Create transformation matrix for Kimi (no translation or rotation)
//        g_kimi_matrix = glm::mat4(1.0f); // Reset model matrix for Kimi
//        g_kimi_matrix = glm::scale(g_kimi_matrix, scale_vector * KIMI_SCALE); // Apply scaling effect
//
//        // Step 2: Update Totsuko's angle for orbit
//        g_angle += ORBIT_SPEED * delta_time; // Increment angle for orbit
//
//        // Step 3: Calculate new x, y position using trigonometry for Totsuko's orbit
//        g_x_offset = RADIUS * glm::cos(g_angle);
//        g_y_offset = RADIUS * glm::sin(g_angle);
//
//        // Step 4: Update Totsuko's transformation matrix
//        g_totsuko_matrix = glm::mat4(1.0f); // Reset model matrix for Totsuko
//        g_totsuko_matrix = glm::translate(g_kimi_matrix, glm::vec3(g_x_offset, g_y_offset, 0.0f)); // Orbit around Kimi
//        g_totsuko_matrix = glm::scale(g_totsuko_matrix, TOTSUKO_SCALE); // Scale Totsuko
//
//
//
//}
//
//void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
//{
//    g_shader_program.set_model_matrix(object_model_matrix);
//    glBindTexture(GL_TEXTURE_2D, object_texture_id);
//    glDrawArrays(GL_TRIANGLES, 0, 6); // Drawing the two triangles for each object
//}
//
//void render()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    float vertices[] = {
//        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
//        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
//    };
//
//    // Textures
//    float texture_coordinates[] = {
//        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
//        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
//    };
//
//    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
//                          0, vertices);
//    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
//
//    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
//                          false, 0, texture_coordinates);
//    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
//
//    draw_object(g_kimi_matrix, g_kimi_texture_id);
//    draw_object(g_totsuko_matrix, g_totsuko_texture_id);
//
//    // We disable two attribute arrays now
//    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
//    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
//
//    SDL_GL_SwapWindow(g_display_window);
//}
//
//int main(int argc, char* argv[])
//{
//    initialise();
//
//    while (g_app_status == RUNNING)
//    {
//        process_input();
//        update();
//        render();
//    }
//
//    SDL_Quit();
//    return 0;
//}
//
//
//


