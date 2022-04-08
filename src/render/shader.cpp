#include "shader.h"

Shader::~Shader() {}

Material::~Material() {
    for (std::pair<std::string, std::pair<int, void*>> p : this->properties) {
        this->freeProperty(p.second.first, p.second.second);
    }
}

void Material::deleteProperty(const std::string& property) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
        this->properties.erase(property);
    }
}

int Material::getInt(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::INT || pair.first == MaterialPropertyType::BOOL || pair.first == MaterialPropertyType::ENUM)
            return *static_cast<int*>(pair.second);
    }
    return 0;
}

float Material::getFloat(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::FLOAT)
            return *static_cast<float*>(pair.second);
    }
    return 0;
}

double Material::getDouble(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::DOUBLE)
            return *static_cast<double*>(pair.second);
    }
    return 0;
}

glm::vec2 Material::getVec2(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::VEC2)
            return *static_cast<glm::vec2*>(pair.second);
    }
    return glm::vec2();
}

glm::vec3 Material::getVec3(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::VEC3 || pair.first == MaterialPropertyType::RGB)
            return *static_cast<glm::vec3*>(pair.second);
    }
    return glm::vec3();
}

glm::vec4 Material::getVec4(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::VEC4 || pair.first == MaterialPropertyType::RGBA)
            return *static_cast<glm::vec4*>(pair.second);
    }
    return glm::vec4();
}

glm::mat3 Material::getMat3(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::MAT3)
            return *static_cast<glm::mat3*>(pair.second);
    }
    return glm::mat3();
}

glm::mat4 Material::getMat4(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::MAT4)
            return *static_cast<glm::mat4*>(pair.second);
    }
    return glm::mat4();
}

Texture* Material::getTexture(const std::string& property) {
    if (this->properties.count(property)) {
        const std::pair<int, void*>& pair = this->properties[property];
        if (pair.first == MaterialPropertyType::TEXTURE)
            //TODO: resource management
            return static_cast<Texture*>(pair.second);
    }
    return 0;
}

void Material::setInt(const std::string& property, int value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    int* ptr = (int*) malloc(sizeof(int));
    *ptr = value;
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::INT;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setFloat(const std::string& property, float value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    float* ptr = (float*) malloc(sizeof(float));
    *ptr = value;
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::FLOAT;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setDouble(const std::string& property, double value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    double* ptr = (double*) malloc(sizeof(double));
    *ptr = value;
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::DOUBLE;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setVec2(const std::string& property, glm::vec2 value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    glm::vec2* ptr = new glm::vec2(value);
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::VEC2;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setVec3(const std::string& property, glm::vec3 value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    glm::vec3* ptr = new glm::vec3(value);
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::VEC3;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setVec4(const std::string& property, glm::vec4 value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    glm::vec4* ptr = new glm::vec4(value);
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::VEC4;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setMat3(const std::string& property, glm::mat3 value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    glm::mat3* ptr = new glm::mat3(value);
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::MAT3;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setMat4(const std::string& property, glm::mat4 value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    glm::mat4* ptr = new glm::mat4(value);
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::MAT4;
    pair.second = ptr;
    this->properties[property] = pair;
}

void Material::setTexture(const std::string& property, Texture* value) {
    if (this->properties.count(property)) {
        std::pair<int, void*>& old = this->properties[property];
        this->freeProperty(old.first, old.second);
    }
    //TODO: resource manager
    std::pair<int, void*> pair;
    pair.first = MaterialPropertyType::TEXTURE;
    pair.second = value;
    this->properties[property] = pair;
}

void Material::freeProperty(int type, void* property) {
    if (type == MaterialPropertyType::VEC2) {
        glm::vec2* v2 = static_cast<glm::vec2*>(property);
        delete v2;
    } else if (type == MaterialPropertyType::RGB || type == MaterialPropertyType::VEC3) {
        glm::vec3* v3 = static_cast<glm::vec3*>(property);
        delete v3;
    } else if (type == MaterialPropertyType::RGBA || type == MaterialPropertyType::VEC4) {
        glm::vec4* v4 = static_cast<glm::vec4*>(property);
        delete v4;
    } else if (type == MaterialPropertyType::MAT3) {
        glm::mat3* m3 = static_cast<glm::mat3*>(property);
        delete m3;
    } else if (type == MaterialPropertyType::MAT4) {
        glm::mat4* m4 = static_cast<glm::mat4*>(property);
        delete m4;
    } else if (type == MaterialPropertyType::TEXTURE) {
        //TODO: resource manager
    } else {
        free(property);
    }
}