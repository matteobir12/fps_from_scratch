#include "Scene.h"
#include "HUD.h"

void Scene::update(GLFWwindow* window) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera->translateGame(glm::vec3(0,0,1));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera->translateGame(glm::vec3(0,0,-1));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera->translateGame(glm::vec3(1,0,0));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera->translateGame(glm::vec3(-1,0,0));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        camera->translateGame(glm::vec3(0,1,0));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        camera->translateGame(glm::vec3(0,-1,0));
    }
}

void Scene::render() {
    for (GLenum error = glGetError(); error; error = glGetError()) {
        std::cerr << "OpenGL Error at start? (" << error << "): " << std::endl;
    }
    drawBackground();
    setLightUniforms();
    for (GameObject* object : objects) {
        object->draw(camera);
    }
    drawPlayers();
    if (drawHud){
        HUD::Render();
    }
}

Scene::Scene(Camera* camera, std::vector<GameObject*> objects, GameObject* player_model, ShaderProgram* backgroundProgram, GLuint backgroundTex) 
: objects(std::move(objects)), player_model(player_model), backgroundProgram(backgroundProgram), backgroundTex(backgroundTex), camera(camera) {
    
    GLuint backVertexBuffer;

    glGenVertexArrays(1, &backgroundVao);
    glBindVertexArray(backgroundVao);

    glGenBuffers(1, &backVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, backVertexBuffer);

    std::vector<float> backgroundPositions = BackgroundPositions();
    glBufferData(GL_ARRAY_BUFFER, backgroundPositions.size() * sizeof(float), backgroundPositions.data(), GL_STATIC_DRAW);

    // Can abstract
    if (backgroundProgram) {
        GLuint backaPositionLocation = glGetAttribLocation(backgroundProgram->getID(), "aPosition");

        glEnableVertexAttribArray(backaPositionLocation);
        glVertexAttribPointer(backaPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        backgroundProgram->setUniform("uSkybox", 0);
    }



    // TEMP
    sceneLights.emplace_back(glm::vec3(10,10,10), glm::vec3(), glm::vec3(1.0f, 0.f, 0.f), false, true);
 
}


// only will need to set vao once. 
// can cache matrix
// probably only need depthfn call once
void Scene::drawBackground() {
    if (!backgroundProgram || !backgroundVao || !backgroundTex) return;

    backgroundProgram->use();

    glBindVertexArray(backgroundVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, backgroundTex);

    glm::mat4 viewMatrix = glm::mat4(camera->getViewMatrix());
    viewMatrix[3][0] = 0;
    viewMatrix[3][1] = 0;
    viewMatrix[3][2] = 0;

    glm::mat4 viewDirectionProjectionMatrix = camera->getProjectionMatrix() * viewMatrix;
    glm::mat4 viewDirectionProjectionInverseMatrix = glm::inverse(viewDirectionProjectionMatrix);

    backgroundProgram->setUniform("uViewDirectionProjectionInverse", viewDirectionProjectionInverseMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    for (GLenum error = glGetError(); error; error = glGetError()) {
        std::cerr << "OpenGL Error in game object when trying draw background " << " at draw array (" << error << "): " << std::endl;
    }
}

// all sorts of caching and what not needs to happen here
// only 3 lights because I'm an idiot, use vbos or something
void Scene::setLightUniforms() {
    if (!lightProgram)
        return;
    int lightCount = sceneLights.size();
    glm::vec3 lightPosition[3];
    glm::vec3 lightDirection[3];
    glm::vec3 lightColor[3];
    bool lightIsDirectional[3];
    bool lightIsOn[3];
    for(int i = 0; i < 3; ++i) {
        if (i >= lightCount) {
            lightIsOn[i] = false;
            break;
        }
        lightPosition[i] = sceneLights[i].position;
        lightDirection[i] = sceneLights[i].lightDirection;
        lightColor[i] = sceneLights[i].lightColor;
        lightIsDirectional[i] = sceneLights[i].lightIsDirectional;
        lightIsOn[i] = sceneLights[i].lightIsOn;
    }
    lightProgram->use();
    lightProgram->setUniform("uLightPosition", lightPosition, lightCount);
    lightProgram->setUniform("uLightDirection", lightDirection, lightCount);
    lightProgram->setUniform("uLightColor", lightColor, lightCount);
    lightProgram->setUniform("uLightIsDirectional", lightIsDirectional, lightCount);
    lightProgram->setUniform("uLightIsOn", lightIsOn, lightCount);

    for (GLenum error = glGetError(); error; error = glGetError()) {
        std::cerr << "OpenGL Error in game object when trying set lights " << " at draw array (" << error << "): " << std::endl;
    }
}

void Scene::drawPlayers() {
    if (player_model && server) {
        server->FuncOnObjects([this] (const Networking::ClientToServerNetworkData d) {
            player_model->setPosition(glm::vec3(d.outGoingPoint.x, d.outGoingPoint.y, d.outGoingPoint.z));
            player_model->draw(camera);
        });

    }
}

std::vector<float> Scene::BackgroundPositions() {
        std::vector<float> a = {
            -1, -1, 
            1, -1, 
            -1,  1, 
            -1,  1,
            1, -1,
            1,  1,
        };
    return a;
}