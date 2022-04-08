#ifndef __SHADER_H__
#define __SHADER_H__

#include <unordered_map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "../texture/texture.h"

enum MaterialPropertyType {
    INT,
    FLOAT,
    DOUBLE,
    BOOL,
    RGB, RGBA,
    VEC2, VEC3, VEC4,
    MAT3, MAT4,
    TEXTURE,
    ENUM
};

class Material;

class Shader {
    public:
        virtual ~Shader() = 0;
        virtual void bind() = 0;
        virtual void use(Material* material) = 0;
        virtual void useMultiple(const std::vector<Material>& materials) = 0;
};

class Material {
    public:
        ~Material();
        inline Shader* getShader() const {return this->shader;}
        
        void deleteProperty(const std::string& property);
        int getInt(const std::string& property);
        float getFloat(const std::string& property);
        double getDouble(const std::string& property);
        glm::vec2 getVec2(const std::string& property);
        glm::vec3 getVec3(const std::string& property);
        glm::vec4 getVec4(const std::string& property);
        glm::mat3 getMat3(const std::string& property);
        glm::mat4 getMat4(const std::string& property);
        Texture* getTexture(const std::string& property);
        void setInt(const std::string& property, int value);
        void setFloat(const std::string& property, float value);
        void setDouble(const std::string& property, double value);
        void setVec2(const std::string& property, glm::vec2 value);
        void setVec3(const std::string& property, glm::vec3 value);
        void setVec4(const std::string& property, glm::vec4 value);
        void setMat3(const std::string& property, glm::mat3 value);
        void setMat4(const std::string& property, glm::mat4 value);
        void setTexture(const std::string& property, Texture* value);
    private:
        Shader* shader;
        std::unordered_map<std::string, std::pair<int, void*>> properties;

        void freeProperty(int type, void* property);
};

#endif