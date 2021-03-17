#include "shadermanager.h"
#include <stdexcept>
#include <filesystem>
#include <iostream>

#ifdef EMBEDDED_SHADERS
#include "default.vs.h"
#include "default.fs.h"
#endif

#pragma message(SHADERPATH)

namespace fs = std::filesystem;

ShaderManager::ShaderManager() {
    const auto shaderpath = fs::absolute(fs::path{SHADERPATH});

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
            const auto vspath = QString::fromStdString((shaderpath / "default.vs").string());
            const auto fspath = QString::fromStdString((shaderpath / "default.fs").string());
            if (!defaultShader.addSource(QOpenGLShader::Vertex, vspath)) {
                throw std::runtime_error{"Failed to compile vertex shader"};
            }

            if (!defaultShader.addSource(QOpenGLShader::Fragment, fspath)) {
                throw std::runtime_error{"Failed to compile fragment shader"};
            }
        #endif

        if (!defaultShader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }


    if (!valid("screen")) {
        auto& screenShader = shader("screen");

        const auto vspath = QString::fromStdString((shaderpath / "screen.vs").string());
        const auto fspath = QString::fromStdString((shaderpath / "screen.fs").string());
        if (!screenShader.addSource(QOpenGLShader::Vertex, vspath)) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!screenShader.addSource(QOpenGLShader::Fragment, fspath)) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!screenShader.link()) {
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

bool ShaderManager::reloadShaders() {
    bool bRet{true};
    for (auto& [name, program] : mShaders) {
        if (!program.reload())
            bRet = false;
    }
    return bRet;
}

bool Shader::addSource(QOpenGLShader::ShaderType type, const std::filesystem::path &filePath) {
    const auto str = QString::fromStdString(std::filesystem::absolute(filePath).string());
    return addSource(type, str);
}

bool Shader::addSource(QOpenGLShader::ShaderType type, const QString &fileName) {
    mPath[type] = fileName;
    return addShaderFromSourceFile(type, fileName);
}

bool Shader::reload() {
    removeAllShaders();

    for (const auto& [type, path] : mPath)
        if (!addShaderFromSourceFile(type, path))
            return false;
    
    return link();
}