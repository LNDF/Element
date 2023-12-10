#pragma once

#include <scenegraph/node_ref.h>
#include <scenegraph/transform.h>
#include <scenegraph/node_type_info.h>
#include <scenegraph/node_storage.h>
#include <utils/uuid.h>
#include <utils/pre_exec.h>
#include <serialization/defs.h>
#include <vector>
#include <string>
#include <utility>
#include <typeindex>
#include <type_traits>

#define ELM_REGISTER_NODE_TYPE(type, name) ELM_PRE_EXECUTE(element::__detail::__scenegraph_register_node_type_info<type>, name)

namespace element {
    namespace __detail {
        template<typename T, typename = std::enable_if<std::is_base_of_v<scenegraph::node, T>>>
        void __scenegraph_register_node_type_info(std::string&& name) {
            __scenegraph_get_node_type_map().try_emplace(std::type_index(typeid(T)), std::move(name), scenegraph::node_storage_factory<T>);
        }
    } // namespace __detail
    

    namespace scenegraph {
        class scene;

        class node {
            private:
                std::string name;
                uuid id;
                transform transf;
                node_ref parent;
                std::vector<node_ref> children;
                scene* owner_scene = nullptr;
                
                virtual void node_setup();
                virtual void node_cleanup();

                inline void set_owner_scene(scene* owner_scene) {this->owner_scene = owner_scene;}

                friend node_storage_base;
            public:
                node();
                virtual ~node();
                node(const uuid& id, const std::string& name, const node_ref& parent);
                node(const uuid& id, std::string&& name, const node_ref& parent);
                node(const uuid& id, const std::string& name, scene* owner_scene);
                node(const uuid& id, std::string&& name, scene* owner_scene);

                void destroy();
                void add_child(const node_ref& child);
                node_ref add_child(std::type_index type, const std::string& name);
                node_ref add_child(std::type_index type, std::string&& name);
                
                template<typename T, typename = std::enable_if<std::is_base_of_v<node, T>>>
                node_ref add_child(const std::string& name) {
                    return add_child(std::type_index(typeid(T)), name);
                }

                template<typename T, typename = std::enable_if<std::is_base_of_v<node, T>>>
                node_ref add_child(std::string&& name) {
                    return add_child(std::type_index(typeid(T)), std::move(name));
                }

                inline const std::string& get_name() const {return name;}
                inline const uuid& get_id() const {return id;}
                inline const node_ref& get_parent() const {return parent;}
                inline const std::vector<node_ref>& get_children() const {return children;}
                inline scene* get_owner_scene() const {return owner_scene;}
                inline transform& get_transform() {return transf;}
                inline const transform& get_transform() const {return transf;}
                inline void set_name(const std::string& new_name) {name = new_name;}
                inline void set_name(std::string&& new_name) {name = std::move(new_name);}

                inline void __set_id(const uuid& id) {if (owner_scene == nullptr) this->id = id;}
                inline void __set_parent(const node_ref& parent) {if (owner_scene == nullptr) this->parent = parent;}
                inline void __set_children(std::vector<node_ref>&& children) {if (owner_scene == nullptr) this->children = std::move(children);}
        };

    } // namespace scenegraph
} // namespace element