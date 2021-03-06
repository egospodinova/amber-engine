#include "OpenGL4Texture.h"

#include <cassert>
#include <stdexcept>

namespace Amber
{
    namespace Rendering
    {
        namespace GL4
        {
            OpenGL4Texture::OpenGL4Texture(Type type, DataFormat dataFormat, std::size_t width, std::size_t height, std::size_t depth, std::size_t mipMapLevels, const std::uint8_t *data)
                : type(type),
                  dataFormat(dataFormat),
                  width(width),
                  height(height),
                  depth(depth),
                  mipMapLevels(mipMapLevels),
                  bindSlot(0)
            {
                glGenTextures(1, &handle);
                bind();

                if (width > 0 || height > 0 || depth > 0)
                {
                    setSize(width, height, depth);

                    if (data != nullptr)
                    {
                        setImageData(data);
                    }
                }
            }

            OpenGL4Texture::OpenGL4Texture(OpenGL4Texture &&other) noexcept
                : OpenGL4Object(other.handle),
                  type(other.type),
                  dataFormat(other.dataFormat),
                  width(other.width),
                  height(other.height),
                  depth(other.depth),
                  mipMapLevels(other.mipMapLevels)
            {
                other.handle = 0;
            }

            OpenGL4Texture::~OpenGL4Texture()
            {
                if (handle != 0)
                {
                    glDeleteTextures(1, &handle);
                }
            }

            OpenGL4Texture &OpenGL4Texture::operator =(OpenGL4Texture &&other) noexcept
            {
                if (this != &other)
                {
                    handle = other.handle;
                    type = other.type;
                    dataFormat = other.dataFormat;
                    width = other.width;
                    height = other.height;
                    depth = other.depth;
                    mipMapLevels = other.mipMapLevels;

                    other.handle = 0;
                }

                return *this;
            }

            void OpenGL4Texture::bind()
            {
                glActiveTexture(GL_TEXTURE0 + bindSlot);
                glBindTexture(getGLType(type), handle);
                this->bound = true;
            }

            void OpenGL4Texture::unbind()
            {
                glActiveTexture(GL_TEXTURE0 + bindSlot);
                glBindTexture(getGLType(type), 0);
                this->bound = false;
            }

            IBindable::BindType OpenGL4Texture::getBindType() const
            {
                return BindType::Texture;
            }

            std::uint32_t OpenGL4Texture::getBindSlot() const
            {
                return bindSlot;
            }

            void OpenGL4Texture::setBindSlot(std::uint32_t bindSlot)
            {
                if (bound)
                {
                    throw std::runtime_error("Cannot change bind slot while texture is bound");
                }

                this->bindSlot = bindSlot;
            }

            std::size_t OpenGL4Texture::getWidth() const
            {
                return width;
            }

            std::size_t OpenGL4Texture::getHeight() const
            {
                return height;
            }

            std::size_t OpenGL4Texture::getDepth() const
            {
                return depth;
            }

            ITexture::Type OpenGL4Texture::getType() const
            {
                return type;
            }

            void OpenGL4Texture::setSize(std::size_t width, std::size_t height, std::size_t depth)
            {
                this->width = width;
                this->height = height;
                this->depth = depth;

                bind();
                switch (type)
                {
                    case Type::Texture1D:
                        if (!(width > 0 && height == 0 && depth == 0))
                        {
                            throw std::runtime_error("Unsupported dimensions for this texture type");
                        }
                        glTexStorage1D(getGLType(type), mipMapLevels, getGLInternalFormat(dataFormat), width);
                        break;
                    case Type::Texture2D:
                    case Type::TextureCube:
                        if (!(width > 0 && height > 0 && depth == 0))
                        {
                            throw std::runtime_error("Unsupported dimensions for this texture type");
                        }
                        glTexStorage2D(getGLType(type), mipMapLevels, getGLInternalFormat(dataFormat), width, height);
                        break;
                    case Type::Texture3D:
                        if (!(width > 0 && height > 0 && depth > 0))
                        {
                            throw std::runtime_error("Unsupported dimensions for this texture type");
                        }
                        glTexStorage3D(getGLType(type), mipMapLevels, getGLInternalFormat(dataFormat), width, height, depth);
                        break;
                    default:
                        throw std::runtime_error("Unsupported texture type.");
                }
                unbind();
            }

            void OpenGL4Texture::setImageData(const std::uint8_t *data)
            {
                std::size_t channels = getChannels(dataFormat);
                GLenum format = getGLFormat(dataFormat);
                GLenum pixelType = getGLPixelType(dataFormat);

                bind();
                switch (type)
                {
                    case Type::Texture1D:
                        glTexSubImage1D(getGLType(type), 0, 0, width, format, pixelType, data);
                        break;
                    case Type::Texture2D:
                        glTexSubImage2D(getGLType(type), 0, 0, 0, width, height, format, pixelType, data);
                        break;
                    case Type::TextureCube:
                    {
                        std::size_t stride = width * height * channels;
                        for (std::size_t side = 0; side < 6; side++)
                        {
                            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, 0, 0, width, height, format, pixelType, data + side * stride);
                        }
                        break;
                    }
                    case Type::Texture3D:
                        glTexSubImage3D(getGLType(type), 0, 0, 0, 0, width, height, depth, format, pixelType, data);
                        break;
                    default:
                        throw std::runtime_error("Unsupported texture type.");
                }

                glGenerateMipmap(getGLType(type));
                unbind();
            }

            void OpenGL4Texture::setFilterMode(ITexture::FilterMode mode)
            {
                bind();
                switch (mode)
                {
                    case FilterMode::Nearest:
                        glTexParameteri(getGLType(type), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(getGLType(type), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case FilterMode::Linear:
                        glTexParameteri(getGLType(type), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(getGLType(type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    default:
                        throw std::runtime_error("Unsupported filter mode.");
                }
                unbind();
            }

            void OpenGL4Texture::setWrapMode(ITexture::WrapMode mode)
            {
                bind();
                switch (mode)
                {
                    case WrapMode::Repeat:
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_R, GL_REPEAT);
                        break;
                    case WrapMode::MirroredRepeat:
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                        break;
                    case WrapMode::ClampToEdge:
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                        break;
                    case WrapMode::ClampToBorder:
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                        glTexParameteri(getGLType(type), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                        break;
                    default:
                        throw std::runtime_error("Unsupported wrap mode.");
                }

                unbind();
            }

            GLenum OpenGL4Texture::getGLType(ITexture::Type type) const
            {
                switch (type)
                {
                    case Type::Texture1D:
                        return GL_TEXTURE_1D;
                    case Type::Texture2D:
                        return GL_TEXTURE_2D;
                    case Type::Texture3D:
                        return GL_TEXTURE_3D;
                    case Type::TextureCube:
                        return GL_TEXTURE_CUBE_MAP;
                    default:
                        throw std::invalid_argument("Unsupported texture type.");
                }
            }

            GLenum OpenGL4Texture::getGLInternalFormat(ITexture::DataFormat dataFormat) const
            {
                switch (dataFormat)
                {
                    case DataFormat::RGB8:
                        return GL_RGB8;
                    case DataFormat::RGB16:
                        return GL_RGB16;
                    case DataFormat::RGB16F:
                        return GL_RGB16F;
                    case DataFormat::RGB32F:
                        return GL_RGB32F;
                    case DataFormat::RGBA8:
                        return GL_RGBA8;
                    case DataFormat::RGBA16:
                        return GL_RGBA16;
                    case DataFormat::RGBA16F:
                        return GL_RGBA16F;
                    case DataFormat::RGBA32F:
                        return GL_RGBA32F;
                    case DataFormat::Depth32:
                        return GL_DEPTH_COMPONENT32F;
                    case DataFormat::Depth24Stencil8:
                        return GL_DEPTH24_STENCIL8;
                    default:
                        throw std::invalid_argument("Unsupported data format.");
                }
            }

            GLenum OpenGL4Texture::getGLFormat(ITexture::DataFormat dataFormat) const
            {
                switch (dataFormat)
                {
                    case DataFormat::RGB8:
                    case DataFormat::RGB16:
                    case DataFormat::RGB16F:
                    case DataFormat::RGB32F:
                        return GL_RGB;
                    case DataFormat::RGBA8:
                    case DataFormat::RGBA16:
                    case DataFormat::RGBA16F:
                    case DataFormat::RGBA32F:
                        return GL_RGBA;
                    case DataFormat::Depth32:
                        return GL_DEPTH_COMPONENT;
                    case DataFormat::Depth24Stencil8:
                        return GL_DEPTH_STENCIL;
                    default:
                        throw std::invalid_argument("Unsupported data format.");
                }
            }

            GLenum OpenGL4Texture::getGLPixelType(ITexture::DataFormat dataFormat) const
            {
                switch (dataFormat)
                {
                    case DataFormat::RGB8:
                    case DataFormat::RGBA8:
                        return GL_UNSIGNED_BYTE;
                    case DataFormat::RGB16:
                    case DataFormat::RGBA16:
                        return GL_UNSIGNED_SHORT;
                    case DataFormat::RGB16F:
                    case DataFormat::RGBA16F:
                    case DataFormat::RGB32F:
                    case DataFormat::RGBA32F:
                    case DataFormat::Depth32:
                        return GL_FLOAT;
                    case DataFormat::Depth24Stencil8:
                        return GL_UNSIGNED_INT_24_8;
                    default:
                        throw std::invalid_argument("Unsupported data format.");
                }
            }

            std::size_t OpenGL4Texture::getChannels(ITexture::DataFormat dataFormat) const
            {
                switch (dataFormat)
                {
                    case DataFormat::RGB8:
                    case DataFormat::RGB16:
                    case DataFormat::RGB16F:
                    case DataFormat::RGB32F:
                        return 3;
                    case DataFormat::RGBA8:
                    case DataFormat::RGBA16:
                    case DataFormat::RGBA16F:
                    case DataFormat::RGBA32F:
                        return 4;
                    case DataFormat::Depth32:
                        return 1;
                    case DataFormat::Depth24Stencil8:
                        return 2;
                    default:
                        throw std::invalid_argument("Unsupported data format.");
                }
            }
        }
    }
}
