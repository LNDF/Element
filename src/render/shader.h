#ifndef __SHADER_H__
#define __SHADER_H__

#include <unordered_map>
#include <string>
#include <vector>

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
        Shader* getShader();
        
        template<typename T>
        void setProperty(const std::string& property, T data);
        T getProperty(const std::string& property);
    private:
        Shader* shader;
        std::unordered_map<std::string, void*> properties;
};

#endif