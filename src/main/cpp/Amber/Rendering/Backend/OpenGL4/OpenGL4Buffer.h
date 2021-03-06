#ifndef OPENGL4BUFFER_H
#define OPENGL4BUFFER_H

#include "Amber/Rendering/Backend/IBuffer.h"
#include "Amber/Rendering/Backend/OpenGL4/OpenGL4Object.h"

#include <functional>
#include <utility>
#include <vector>

#include "Amber/Rendering/Backend/OpenGL4/OpenGL4Includes.h"

namespace Amber
{
    namespace Rendering
    {
        namespace GL4
        {
            class OpenGL4Buffer : public IBuffer, public OpenGL4Object
            {
                public:
                    enum class UsagePattern : GLenum
                    {
                        StreamDraw = GL_STREAM_DRAW,
                        StreamRead = GL_STREAM_READ,
                        StreamCopy = GL_STREAM_COPY,
                        StaticDraw = GL_STATIC_DRAW,
                        StaticRead = GL_STATIC_READ,
                        StaticCopy = GL_STATIC_COPY,
                        DynamicDraw = GL_DYNAMIC_DRAW,
                        DynamicRead = GL_DYNAMIC_READ,
                        DynamicCopy = GL_DYNAMIC_COPY
                    };

                    OpenGL4Buffer(Type type, std::size_t capacity = 0, void *data = nullptr, UsagePattern usagePattern = UsagePattern::StaticDraw);
                    OpenGL4Buffer(const OpenGL4Buffer &other) = delete;
                    OpenGL4Buffer(OpenGL4Buffer &&other) noexcept;
                    virtual ~OpenGL4Buffer();

                    OpenGL4Buffer &operator =(const OpenGL4Buffer &other) = delete;
                    OpenGL4Buffer &operator =(OpenGL4Buffer &&other) noexcept;

                    OpenGL4Buffer clone();

                    virtual void assign(std::size_t offset, std::size_t size, const void *data);

                    virtual void migrate(IBuffer &otherStorage);

                    virtual void resize(std::size_t newCapacity);

                    virtual Utilities::ScopedDataPointer data();

                    virtual std::size_t getCapacity() const;
                    virtual bool isNull() const;

                    virtual void clear();

                    virtual void bind();
                    virtual void unbind();

                    virtual BindType getBindType() const;
                    virtual std::uint32_t getBindSlot() const;
                    virtual void setBindSlot(std::uint32_t bindSlot);

                    virtual Type getType() const;
                    virtual void setType(Type type);

                private:
                    GLenum getGLType(Type type) const;

                    bool isMultiSlotSupported() const;

                    Type type;
                    std::size_t capacity;
                    UsagePattern usagePattern;
                    std::uint32_t bindSlot;
                    bool bound;
            };
        }
    }
}

#endif // OPENGL4BUFFER_H
