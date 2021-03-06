#include "shadermanager.h"
#include <stdexcept>
#include <filesystem>
#include <iostream>

#ifdef EMBEDDED_SHADERS
#ifdef CMAKE
static char DEFAULT_VS[] = {R"MSTR(@default.vs@)MSTR"};
static char DEFAULT_FS[] = {R"MSTR(@default.fs@)MSTR"};
static char SCREEN_VS[] = {R"MSTR(@screen.vs@)MSTR"};
static char SCREEN_FS[] = {R"MSTR(@screen.fs@)MSTR"};
static char SLICE_IMAGE_FS[] = {R"MSTR(@slice-image.fs@)MSTR"};
static char VOLUME_FS[] = {R"MSTR(@volume.fs@)MSTR"};
static char AXIS_VS[] = {R"MSTR(@axis.vs@)MSTR"};
static char AXIS_FS[] = {R"MSTR(@axis.fs@)MSTR"};
static char NODE_VS[] = {R"MSTR(@node.vs@)MSTR"};
static char NODE_GS[] = {R"MSTR(@node.gs@)MSTR"};
static char NODE_FS[] = {R"MSTR(@node.fs@)MSTR"};
static char PLANE_VS[] = {R"MSTR(@plane.vs@)MSTR"};
static char PLANE_GS[] = {R"MSTR(@plane.gs@)MSTR"};
static char PLANE_FS[] = {R"MSTR(@plane.fs@)MSTR"};
static char SPLINE_GS[] = {R"MSTR(@spline.gs@)MSTR"};
static char SPLINE_FS[] = {R"MSTR(@spline.fs@)MSTR"};
static char GLOBE_VS[] = {R"MSTR(@globe.vs@)MSTR"};
static char GLOBE_GS[] = {R"MSTR(@globe.gs@)MSTR"};
static char GLOBE_FS[] = {R"MSTR(@globe.fs@)MSTR"};
#elif QMAKE
#include <QFile>

#define DEFAULT_VS fetchStrFromFile(":/shaders/default.vs")
#define DEFAULT_FS fetchStrFromFile(":/shaders/default.fs")
#define SCREEN_VS fetchStrFromFile(":/shaders/screen.vs")
#define SCREEN_FS fetchStrFromFile(":/shaders/screen.fs")
#define SLICE_IMAGE_FS fetchStrFromFile(":/shaders/slice-image.fs")
#define VOLUME_FS fetchStrFromFile(":/shaders/volume.fs")
#define AXIS_VS fetchStrFromFile(":/shaders/axis.vs")
#define AXIS_FS fetchStrFromFile(":/shaders/axis.fs")
#define NODE_VS fetchStrFromFile(":/shaders/node.vs")
#define NODE_GS fetchStrFromFile(":/shaders/node.gs")
#define NODE_FS fetchStrFromFile(":/shaders/node.fs")
#define PLANE_VS fetchStrFromFile(":/shaders/plane.vs")
#define PLANE_GS fetchStrFromFile(":/shaders/plane.gs")
#define PLANE_FS fetchStrFromFile(":/shaders/plane.fs")
#define SPLINE_GS fetchStrFromFile(":/shaders/spline.gs")
#define SPLINE_FS fetchStrFromFile(":/shaders/spline.fs")
#define GLOBE_VS fetchStrFromFile(":/shaders/globe.vs")
#define GLOBE_GS fetchStrFromFile(":/shaders/globe.gs")
#define GLOBE_FS fetchStrFromFile(":/shaders/globe.fs")

#endif
#endif

namespace fs = std::filesystem;

ShaderManager::ShaderManager() {
    #ifdef QMAKE
        std::string shaderFileContent;
        const auto fetchStrFromFile = [&](const auto& fileName) -> const char* {
            QFile file{fileName};
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream in{&file};
                shaderFileContent = QString{in.readAll()}.toStdString();
                return shaderFileContent.c_str();
            }
            return nullptr;
        };

        Q_INIT_RESOURCE(shaders);
    #endif

    const auto shaderpath = fs::absolute(fs::path{SHADERPATH});

    // Compile default shader (if it doesn't exist)
    if (!valid("default")) {
        auto& shaderProgram = shader("default");

        // Note: While throwing from a constructor is bad practice, I don't know many other better alternatives here.
        #ifdef EMBEDDED_SHADERS
            if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, DEFAULT_VS))
                throw std::runtime_error{"Failed to compile vertex shader"};

            if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, DEFAULT_FS))
                throw std::runtime_error{"Failed to compile fragment shader"};
        #else
            if (!shaderProgram.addSourceRelative(QOpenGLShader::Vertex, "default.vs"))
                throw std::runtime_error{"Failed to compile vertex shader"};

            if (!shaderProgram.addSourceRelative(QOpenGLShader::Fragment, "default.fs"))
                throw std::runtime_error{"Failed to compile fragment shader"};
        #endif

        if (!shaderProgram.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }


    if (!valid("screen")) {
        auto& shaderProgram = shader("screen");

        #ifdef EMBEDDED_SHADERS
            if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, SCREEN_VS))
                throw std::runtime_error{"Failed to compile vertex shader"};

            if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, SCREEN_FS))
                throw std::runtime_error{"Failed to compile fragment shader"};

        #else
            if (!shaderProgram.addSourceRelative(QOpenGLShader::Vertex, "screen.vs"))
                throw std::runtime_error{"Failed to compile vertex shader"};

            if (!shaderProgram.addSourceRelative(QOpenGLShader::Fragment, "screen.fs"))
                throw std::runtime_error{"Failed to compile fragment shader"};
        #endif

        if (!shaderProgram.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }

#ifdef EMBEDDED_SHADERS
    // Create volume shader
    if (!valid("slice")) {
        auto& sh = shader("slice");
        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, SCREEN_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, SLICE_IMAGE_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};

        if (!sh.link())
            qDebug() << "Failed to link shader!";
    }

    // Create volume shader
    if (!valid("volume")) {
        auto& sh = shader("volume");
        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, SCREEN_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, VOLUME_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};

        if (!sh.link())
            qDebug() << "Failed to link shader!";
    }

    if (!valid("axis")) {
        auto& sh = shader("axis");

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, AXIS_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, AXIS_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};

        if (!sh.link())
            throw std::runtime_error{"Failed to link shader!"};
    }

    if (!valid("node")) {
        auto& sh = shader("node");

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, NODE_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Geometry, NODE_GS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, NODE_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};

        if (!sh.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }

    if (!valid("plane")) {
        auto& sh = shader("plane");

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, PLANE_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Geometry, PLANE_GS))
            throw std::runtime_error{"Failed to compile geometry shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, PLANE_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};


        if (!sh.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }

    if (!valid("spline")) {
        auto& sh = shader("spline");

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, NODE_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Geometry, SPLINE_GS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, SPLINE_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};

        if (!sh.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }

    if (!valid("globe")) {
        auto& sh = shader("globe");

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Vertex, GLOBE_VS))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Geometry, GLOBE_GS))
            throw std::runtime_error{"Failed to compile geometry shader"};

        if (!sh.addShaderFromSourceCode(QOpenGLShader::Fragment, GLOBE_FS))
            throw std::runtime_error{"Failed to compile fragment shader"};


        if (!sh.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }
#endif
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

bool Shader::addSourceRelative(QOpenGLShader::ShaderType type, const std::string &filePath) {
    return addSource(type, fs::path{SHADERPATH} / filePath);
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
