#include "shadermanager.h"

#ifdef EMBEDDED_SHADERS
#include "default.vs.h"
#include "default.fs.h"
#endif

ShaderManager::ShaderManager() {
    // Compile default shader (if it doesn't exist)
    if (!valid("default")) {
        auto& defaultShader = shader("default");

        // Note: While throwing from a constructor is bad practice, I don't know many other better alternatives here.
        #ifdef EMBEDDED_SHADERS
            if (!defaultShader.addShaderFromSourceCode(QOpenGLShader::Vertex, DEFAULT_VS)) {
                throw std::runtime_error{"Failed to compile vertex shader"};
            }

            if (!defaultShader.addShaderFromSourceCode(QOpenGLShader::Fragment, DEFAULT_FS)) {
                throw std::runtime_error{"Failed to compile fragment shader"};
            }
        #else
            if (!defaultShader.addShaderFromSourceFile(QOpenGLShader::Vertex, "./src/shaders/default.vs")) {
                throw std::runtime_error{"Failed to compile vertex shader"};
            }

            if (!defaultShader.addShaderFromSourceFile(QOpenGLShader::Fragment, "./src/shaders/default.fs")) {
                throw std::runtime_error{"Failed to compile fragment shader"};
            }
        #endif

        if (!defaultShader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }
}

ShaderManager& ShaderManager::get() {
    static ShaderManager instance{};
    return instance;
}

// const QOpenGLShaderProgram& Renderer::ShaderManager::shader(const std::string& name) const {
//     const auto& it = mShaders.find(name);
//     if (it != mShaders.end())
//         return it->second;
//     else
//         return QOpenGLShaderProgram{}; // Note: Should maybe throw instead of creating a temporary invalid object
// }

bool ShaderManager::valid(const std::string& name) const {
    const auto it = mShaders.find(name);
    if (it == mShaders.end())
        return false;

    const auto& s = it->second;

    return s.isLinked();
}