#ifndef OPENGL4PROGRAM_H
#define OPENGL4PROGRAM_H

#include "Amber/Rendering/Backend/IProgram.h"
#include "Amber/Rendering/Backend/OpenGL4/OpenGL4Object.h"

#include <map>
#include <memory>
#include <vector>

#include "Amber/Rendering/Backend/Layout.h"
#include "Amber/Rendering/Backend/OpenGL4/OpenGL4Includes.h"

namespace Amber
{
    namespace Rendering
    {
        namespace GL4
        {
            class OpenGL4Shader;

            class OpenGL4Program : public IProgram, public OpenGL4Object
            {
                public:
                    OpenGL4Program();
                    OpenGL4Program(const OpenGL4Program &other) = delete;
                    OpenGL4Program(OpenGL4Program &&other) noexcept;
                    virtual ~OpenGL4Program();

                    OpenGL4Program &operator =(const OpenGL4Program &other) = delete;
                    OpenGL4Program &operator =(OpenGL4Program &&other) noexcept;

                    virtual void bind() override final;
                    virtual void unbind() override final;

                    virtual BindType getBindType() const override final;
                    virtual std::uint32_t getBindSlot() const override final;

                    virtual void link() override final;
                    virtual bool isLinked() const override final;

                    const std::vector<Reference<OpenGL4Shader>> &getShaders() const;
                    virtual void addShader(Reference<IShader> shader) override final;

                    virtual const Layout &getLayout() const override final;
                    virtual void setLayout(Layout layout) override final;

                    virtual void setConstant(std::string name, std::int32_t value) override final;
                    virtual void setConstant(std::string name, std::uint32_t value) override final;
                    virtual void setConstant(std::string name, float value) override final;

                    virtual void setConstant(std::string name, Eigen::Matrix4f value) override final;
                    virtual void setConstant(std::string name, Eigen::Matrix3f value) override final;
                    virtual void setConstant(std::string name, Eigen::Vector2f value) override final;
                    virtual void setConstant(std::string name, Eigen::Vector3f value) override final;
                    virtual void setConstant(std::string name, Eigen::Vector4f value) override final;

                private:
                    void introspect();
                    GLint findConstantByName(const std::string &name);

                    std::vector<Reference<OpenGL4Shader>> shaders;
                    Layout layout;
                    std::map<std::string, std::uint32_t> constantBindLocationsByName;
                    bool linked;
            };
        }
    }
}

#endif // OPENGL4PROGRAM_H
